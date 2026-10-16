/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 21:07:09 by theoppon          #+#    #+#             */
/*   Updated: 2026/10/15 21:07:11 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

static void	request_deadline_from_coder(t_coder *coder,
	struct timespec *deadline)
{
	pthread_mutex_lock(&coder->global->finished_mutex);
	*deadline = coder->last_compile_start;
	pthread_mutex_unlock(&coder->global->finished_mutex);
	add_milliseconds(deadline, coder->global->time_to_burnout);
}

static t_fifo_request	*create_request(t_coder *coder)
{
	t_fifo_request	*request;

	request = malloc(sizeof(t_fifo_request));
	if (request == NULL)
		return (NULL);
	if (pthread_cond_init(&request->request_cond, NULL) != 0)
	{
		free(request);
		return (NULL);
	}
	request_deadline_from_coder(coder, &request->deadline);
	request->order = 0;
	return (request);
}

static int	wait_for_dongle(t_coder *coder, t_dongle *dongle,
	t_fifo_queue *queue, t_fifo_request *request)
{
	struct timespec	target;
	t_fifo_request	*top;

	(void)coder;
	while (!simulation_is_finished(dongle->global))
	{
		top = scheduler_queue_top(queue);
		if (top == request && dongle->state == FREE
			&& scheduler_queue_is_first(queue, request))
			return (1);
		if (dongle->state == COOLDOWN)
		{
			if (cooldown_expired(dongle, &target))
				dongle->state = FREE;
			else
				pthread_cond_timedwait(&request->request_cond,
					&dongle->state_lock, &target);
		}
		else
			pthread_cond_wait(&request->request_cond,
				&dongle->state_lock);
	}
	return (0);
}

static int	queue_dongle_request(t_dongle *dongle,
	t_fifo_queue *queue, t_fifo_request *request)
{
	t_fifo_request	*previous_top;
	t_fifo_request	*top;

	if (simulation_is_finished(dongle->global))
	{
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
	return (1);
}

int	take_dongle(t_coder *coder, t_dongle *dongle)
{
	t_fifo_queue	*queue;
	t_fifo_request	*request;

	queue = scheduler_get_queue(dongle);
	request = create_request(coder);
	if (request == NULL)
		return (0);
	pthread_mutex_lock(&dongle->state_lock);
	if (!queue_dongle_request(dongle, queue, request))
		return (0);
	if (!wait_for_dongle(coder, dongle, queue, request))
	{
		scheduler_queue_remove(queue, request);
		pthread_mutex_unlock(&dongle->state_lock);
		pthread_cond_destroy(&request->request_cond);
		free(request);
		return (0);
	}
	request = scheduler_queue_pop_front(queue);
	dongle->state = TAKEN;
	pthread_mutex_unlock(&dongle->state_lock);
	pthread_cond_destroy(&request->request_cond);
	free(request);
	return (1);
}
