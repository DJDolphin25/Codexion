<<<<<<< HEAD
#include <codexion.h>

static void	request_deadline_from_coder(t_coder *coder, struct timespec *deadline)
{
	pthread_mutex_lock(&coder->global->finished_mutex);
	*deadline = coder->last_compile_start;
	pthread_mutex_unlock(&coder->global->finished_mutex);
	add_milliseconds(deadline, coder->global->time_to_burnout);
}

static t_fifo_request	*create_request(t_coder *coder)
=======
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
>>>>>>> 68f497b8d68ae8cbedd2c0b0e8af99a9d9a6761f
{
	t_fifo_request	*request;

	request = malloc(sizeof(t_fifo_request));
	if (request == NULL)
		return (NULL);
<<<<<<< HEAD
	if (pthread_cond_init(&request->request_cond, NULL) != 0)
	{
		free(request);
		return (NULL);
	}
	request_deadline_from_coder(coder, &request->deadline);
	request->order = 0;
	return (request);
}

int	take_dongle(t_coder *coder, t_dongle *dongle)
=======
	request->next = NULL;
	return (request);
}

int	take_dongle(t_dongle *dongle)
>>>>>>> 68f497b8d68ae8cbedd2c0b0e8af99a9d9a6761f
{
	t_fifo_queue	*queue;
	t_fifo_request	*request;
	struct timespec	target;
<<<<<<< HEAD
	t_fifo_request	*previous_top;
	t_fifo_request	*top;

	queue = scheduler_get_queue(dongle);
	request = create_request(coder);
	if (request == NULL)
		return (0);
	pthread_mutex_lock(&dongle->state_lock);
	if (simulation_is_finished(dongle->global))
	{
		scheduler_queue_remove(queue, request);
		pthread_mutex_unlock(&dongle->state_lock);
		pthread_cond_destroy(&request->request_cond);
		free(request);
		return (0);
	}
	previous_top = scheduler_queue_top(queue);
	scheduler_queue_push(queue, request);
	top = scheduler_queue_top(queue);
	if (previous_top != NULL && previous_top != top)
		pthread_cond_signal(&previous_top->request_cond);
	while (1)
	{
		if (simulation_is_finished(dongle->global))
		{
			scheduler_queue_remove(queue, request);
			pthread_mutex_unlock(&dongle->state_lock);
			pthread_cond_destroy(&request->request_cond);
			free(request);
			return (0);
		}
		top = scheduler_queue_top(queue);
		if (top != request)
		{
			pthread_cond_wait(&request->request_cond, &dongle->state_lock);
			continue ;
		}
		if (dongle->state == TAKEN)
		{
			pthread_cond_wait(&request->request_cond, &dongle->state_lock);
			continue ;
		}
		if (dongle->state == COOLDOWN)
		{
			if (!cooldown_expired(dongle, &target))
			{
				pthread_cond_timedwait(&request->request_cond,
=======

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
>>>>>>> 68f497b8d68ae8cbedd2c0b0e8af99a9d9a6761f
					&dongle->state_lock, &target);
				continue ;
			}
			else
				dongle->state = FREE;
		}
		if (dongle->state == FREE && scheduler_queue_is_first(queue, request))
		{
<<<<<<< HEAD
			request = scheduler_queue_pop_front(queue);
			dongle->state = TAKEN;
			pthread_mutex_unlock(&dongle->state_lock);
			pthread_cond_destroy(&request->request_cond);
			free(request);
			return (1);
		}
		pthread_cond_wait(&request->request_cond, &dongle->state_lock);
=======
			scheduler_queue_pop_front(queue);
			dongle->state = TAKEN;
			pthread_mutex_unlock(&dongle->state_lock);
			return (1);
		}
		pthread_cond_wait(&dongle->available_cond, &dongle->state_lock);
>>>>>>> 68f497b8d68ae8cbedd2c0b0e8af99a9d9a6761f
	}
}

int	drop_dongle(t_dongle *dongle)
{
<<<<<<< HEAD
	t_fifo_request	*top;
	t_fifo_queue	*queue;

	pthread_mutex_lock(&dongle->state_lock);
	dongle->state = COOLDOWN;
	clock_gettime(CLOCK_REALTIME, &dongle->last_release);
	queue = scheduler_get_queue(dongle);
	top = scheduler_queue_top(queue);
	if (top != NULL)
		pthread_cond_signal(&top->request_cond);
=======
	pthread_mutex_lock(&dongle->state_lock);
	dongle->state = COOLDOWN;
	clock_gettime(CLOCK_REALTIME, &dongle->last_release);
	pthread_cond_broadcast(&dongle->available_cond);
>>>>>>> 68f497b8d68ae8cbedd2c0b0e8af99a9d9a6761f
	pthread_mutex_unlock(&dongle->state_lock);
	return (1);
}