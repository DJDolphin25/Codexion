/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 00:00:00 by theoppon          #+#    #+#             */
/*   Updated: 2026/10/09 00:00:00 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

static int	cooldown_expired(t_dongle *dongle, struct timespec *target)
{
	struct timespec	current;

	clock_gettime(CLOCK_REALTIME, &current);
	dongle->target_time = dongle->last_release;
	dongle->target_time.tv_nsec += dongle->global->dongle_cooldown * 1000000;
	if (dongle->target_time.tv_nsec >= 1000000000)
	{
		dongle->target_time.tv_sec += dongle->target_time.tv_nsec / 1000000000;
		dongle->target_time.tv_nsec %= 1000000000;
	}
	*target = dongle->target_time;
	if (current.tv_sec < target->tv_sec)
		return (0);
	if (current.tv_sec > target->tv_sec)
		return (1);
	if (current.tv_nsec < target->tv_nsec)
		return (0);
	return (1);
}

static t_fifo_request	*create_request(void)
{
	t_fifo_request	*request;

	request = malloc(sizeof(t_fifo_request));
	if (request == NULL)
		return (NULL);
	request->next = NULL;
	return (request);
}

int	take_dongle(t_dongle *dongle)
{
	t_fifo_queue	*queue;
	t_fifo_request	*request;
	struct timespec	target;

	queue = scheduler_get_queue(dongle);
	request = create_request();
	if (request == NULL)
		return (0);
	pthread_mutex_lock(&dongle->state_lock);
	scheduler_queue_push(queue, request);
	while (1)
	{
		if (dongle->state == TAKEN)
		{
			pthread_cond_wait(&dongle->available_cond, &dongle->state_lock);
			continue ;
		}
		else if (dongle->state == COOLDOWN)
		{
			if (!cooldown_expired(dongle, &target))
			{
				pthread_cond_timedwait(&dongle->available_cond,
					&dongle->state_lock, &target);
				continue ;
			}
			else
				dongle->state = FREE;
		}
		if (dongle->state == FREE && scheduler_queue_is_first(queue, request))
		{
			scheduler_queue_pop_front(queue);
			dongle->state = TAKEN;
			pthread_mutex_unlock(&dongle->state_lock);
			return (1);
		}
		pthread_cond_wait(&dongle->available_cond, &dongle->state_lock);
	}
}

int	drop_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->state_lock);
	dongle->state = COOLDOWN;
	clock_gettime(CLOCK_REALTIME, &dongle->last_release);
	pthread_cond_broadcast(&dongle->available_cond);
	pthread_mutex_unlock(&dongle->state_lock);
	return (1);
}