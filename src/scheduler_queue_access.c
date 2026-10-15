#include <codexion.h>

void	*scheduler_get_queue(t_dongle *dongle)
{
	return (&dongle->global->scheduler_queue[dongle - dongle->global->dongles]);
}

int	scheduler_queue_is_first(void *queue_ptr, t_fifo_request *request)
{
	t_fifo_queue	*queue;

	queue = (t_fifo_queue *)queue_ptr;
	return (queue->size > 0 && queue->items[0] == request);
}

void	*scheduler_queue_top(void *queue_ptr)
{
	t_fifo_queue	*queue;

	queue = (t_fifo_queue *)queue_ptr;
	if (queue->size == 0)
		return (NULL);
	return (queue->items[0]);
}

void	scheduler_queue_signal_all(void *queue_ptr)
{
	int				index;
	t_fifo_queue	*queue;

	queue = (t_fifo_queue *)queue_ptr;
	index = 0;
	while (index < queue->size)
	{
		pthread_cond_signal(&queue->items[index]->request_cond);
		index++;
	}
}

void	scheduler_queue_reset(void *queue_ptr)
{
	t_fifo_queue	*queue;
	t_fifo_request	*top;

	queue = (t_fifo_queue *)queue_ptr;
	while (queue->size > 0)
	{
		top = scheduler_queue_pop_front(queue);
		pthread_cond_destroy(&top->request_cond);
		free(top);
	}
}
