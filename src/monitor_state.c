#include <codexion.h>

int	simulation_is_finished(t_global *global)
{
	int	finished;

	pthread_mutex_lock(&global->finished_mutex);
	finished = global->finished;
	pthread_mutex_unlock(&global->finished_mutex);
	return (finished);
}

void	simulation_set_finished(t_global *global)
{
	pthread_mutex_lock(&global->finished_mutex);
	global->finished = 1;
	pthread_cond_broadcast(&global->scheduler_cond);
	pthread_mutex_unlock(&global->finished_mutex);
}

int	all_coders_completed(t_global *global)
{
	int	index;
	int	done;

	done = 1;
	pthread_mutex_lock(&global->finished_mutex);
	index = 0;
	while (index < global->number_of_coders)
	{
		if (global->coders[index].compiles_done
			< global->number_of_compiles_required)
		{
			done = 0;
			break ;
		}
		index++;
	}
	pthread_mutex_unlock(&global->finished_mutex);
	return (done);
}
