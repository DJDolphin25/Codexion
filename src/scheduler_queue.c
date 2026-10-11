<<<<<<< HEAD
#include <codexion.h>

=======
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

>>>>>>> 68f497b8d68ae8cbedd2c0b0e8af99a9d9a6761f
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
<<<<<<< HEAD
		global->scheduler_queue[i].items = malloc(sizeof(t_fifo_request *)
			* global->number_of_coders);
		if (global->scheduler_queue[i].items == NULL)
		{
			while (--i >= 0)
				free(global->scheduler_queue[i].items);
			free(global->scheduler_queue);
			global->scheduler_queue = NULL;
			return (0);
		}
		global->scheduler_queue[i].size = 0;
		global->scheduler_queue[i].capacity = global->number_of_coders;
		global->scheduler_queue[i].next_order = 0;
		global->scheduler_queue[i].global = global;
=======
		global->scheduler_queue[i].front = NULL;
		global->scheduler_queue[i].back = NULL;
>>>>>>> 68f497b8d68ae8cbedd2c0b0e8af99a9d9a6761f
		i++;
	}
	return (1);
}

void	destroy_scheduler_queues(t_global *global)
{
	int						i;
<<<<<<< HEAD
=======
	t_fifo_request	*next;
	t_fifo_request	*current;
>>>>>>> 68f497b8d68ae8cbedd2c0b0e8af99a9d9a6761f

	if (global == NULL || global->scheduler_queue == NULL)
		return ;
	i = 0;
	while (i < global->number_of_coders)
	{
<<<<<<< HEAD
		scheduler_queue_reset(&global->scheduler_queue[i]);
		free(global->scheduler_queue[i].items);
=======
		current = global->scheduler_queue[i].front;
		while (current != NULL)
		{
			next = current->next;
			free(current);
			current = next;
		}
>>>>>>> 68f497b8d68ae8cbedd2c0b0e8af99a9d9a6761f
		i++;
	}
	free(global->scheduler_queue);
	global->scheduler_queue = NULL;
}