/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_queue_heap.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 21:09:49 by theoppon          #+#    #+#             */
/*   Updated: 2026/10/15 21:09:51 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

int	request_is_before(t_fifo_queue *queue, t_fifo_request *first,
		t_fifo_request *second)
{
	if (first == NULL)
		return (0);
	if (second == NULL)
		return (1);
	if (strcmp(queue->global->scheduler, "edf") == 0)
	{
		if (first->deadline.tv_sec != second->deadline.tv_sec)
			return (first->deadline.tv_sec < second->deadline.tv_sec);
		if (first->deadline.tv_nsec != second->deadline.tv_nsec)
			return (first->deadline.tv_nsec < second->deadline.tv_nsec);
	}
	return (first->order < second->order);
}

void	swap_requests(t_fifo_request **first, t_fifo_request **second)
{
	t_fifo_request	*temp;

	temp = *first;
	*first = *second;
	*second = temp;
}

void	bubble_up(t_fifo_queue *queue, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (request_is_before(queue, queue->items[parent], queue->items[index]))
			break ;
		swap_requests(&queue->items[parent], &queue->items[index]);
		index = parent;
	}
}

void	bubble_down(t_fifo_queue *queue, int index)
{
	int	left;
	int	right;
	int	best;

	while (1)
	{
		left = index * 2 + 1;
		right = left + 1;
		best = index;
		if (left < queue->size
			&& request_is_before(queue, queue->items[left], queue->items[best]))
			best = left;
		if (right < queue->size
			&& request_is_before(queue, queue->items[right],
				queue->items[best]))
			best = right;
		if (best == index)
			break ;
		swap_requests(&queue->items[index], &queue->items[best]);
		index = best;
	}
}
