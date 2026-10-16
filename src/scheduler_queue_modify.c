/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_queue_modify.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 21:10:00 by theoppon          #+#    #+#             */
/*   Updated: 2026/10/15 21:10:01 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

void	scheduler_queue_push(void *queue_ptr, t_fifo_request *request)
{
	int				index;
	t_fifo_queue	*queue;

	queue = (t_fifo_queue *)queue_ptr;
	request->order = queue->next_order++;
	index = queue->size;
	queue->items[index] = request;
	queue->size++;
	bubble_up(queue, index);
}

static int	find_request(t_fifo_queue *queue, t_fifo_request *request)
{
	int	index;

	index = 0;
	while (index < queue->size && queue->items[index] != request)
		index++;
	return (index);
}

int	scheduler_queue_remove(void *queue_ptr, t_fifo_request *request)
{
	int				index;
	int				last_index;
	t_fifo_queue	*queue;

	queue = (t_fifo_queue *)queue_ptr;
	index = find_request(queue, request);
	if (index == queue->size)
		return (0);
	last_index = queue->size - 1;
	queue->size--;
	if (index == last_index)
	{
		queue->items[last_index] = NULL;
		return (1);
	}
	queue->items[index] = queue->items[last_index];
	queue->items[last_index] = NULL;
	if (index > 0
		&& !request_is_before(queue, queue->items[(index - 1) / 2],
			queue->items[index]))
		bubble_up(queue, index);
	else
		bubble_down(queue, index);
	return (1);
}

t_fifo_request	*scheduler_queue_pop_front(void *queue_ptr)
{
	int				last_index;
	t_fifo_queue	*queue;
	t_fifo_request	*top;

	queue = (t_fifo_queue *)queue_ptr;
	if (queue->size == 0)
		return (NULL);
	top = queue->items[0];
	last_index = queue->size - 1;
	queue->items[0] = queue->items[last_index];
	queue->items[last_index] = NULL;
	queue->size--;
	if (queue->size > 0)
		bubble_down(queue, 0);
	return (top);
}
