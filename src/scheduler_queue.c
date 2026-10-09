/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_queue.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 00:00:00 by theoppon          #+#    #+#             */
/*   Updated: 2026/10/09 00:00:00 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

void	*scheduler_get_queue(t_dongle *dongle)
{
	return (&dongle->global->scheduler_queue[dongle - dongle->global->dongles]);
}

int	scheduler_queue_is_first(void *queue_ptr, t_fifo_request *request)
{
	t_fifo_queue	*queue;

	queue = (t_fifo_queue *)queue_ptr;
	return (queue->front == request);
}

void	scheduler_queue_push(void *queue_ptr, t_fifo_request *request)
{
	t_fifo_queue	*queue;

	queue = (t_fifo_queue *)queue_ptr;
	request->next = NULL;
	if (queue->back != NULL)
		queue->back->next = request;
	else
		queue->front = request;
	queue->back = request;
}

void	scheduler_queue_pop_front(void *queue_ptr)
{
	t_fifo_queue	*queue;
	t_fifo_request	*front;

	queue = (t_fifo_queue *)queue_ptr;
	front = queue->front;
	if (front == NULL)
		return ;
	queue->front = front->next;
	if (queue->front == NULL)
		queue->back = NULL;
	free(front);
}

int	init_scheduler_queues(t_global *global)
{
	int	i;

	global->scheduler_queue = malloc(sizeof(t_fifo_queue)
		* global->number_of_coders);
	if (global->scheduler_queue == NULL)
		return (0);
	i = 0;
	while (i < global->number_of_coders)
	{
		global->scheduler_queue[i].front = NULL;
		global->scheduler_queue[i].back = NULL;
		i++;
	}
	return (1);
}

void	destroy_scheduler_queues(t_global *global)
{
	int						i;
	t_fifo_request	*next;
	t_fifo_request	*current;

	if (global == NULL || global->scheduler_queue == NULL)
		return ;
	i = 0;
	while (i < global->number_of_coders)
	{
		current = global->scheduler_queue[i].front;
		while (current != NULL)
		{
			next = current->next;
			free(current);
			current = next;
		}
		i++;
	}
	free(global->scheduler_queue);
	global->scheduler_queue = NULL;
}