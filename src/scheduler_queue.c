#include <codexion.h>

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
		i++;
	}
	return (1);
}

void	destroy_scheduler_queues(t_global *global)
{
	int						i;

	if (global == NULL || global->scheduler_queue == NULL)
		return ;
	i = 0;
	while (i < global->number_of_coders)
	{
		scheduler_queue_reset(&global->scheduler_queue[i]);
		free(global->scheduler_queue[i].items);
		i++;
	}
	free(global->scheduler_queue);
	global->scheduler_queue = NULL;
}
