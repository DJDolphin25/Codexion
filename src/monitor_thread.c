/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_thread.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 21:06:42 by theoppon          #+#    #+#             */
/*   Updated: 2026/10/15 21:06:44 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

static int	check_burnout(t_global *global)
{
	int				index;
	int				compiles_done;
	struct timespec	current;

	index = 0;
	while (index < global->number_of_coders)
	{
		pthread_mutex_lock(&global->finished_mutex);
		compiles_done = global->coders[index].compiles_done;
		pthread_mutex_unlock(&global->finished_mutex);
		if (compiles_done < global->number_of_compiles_required
			&& coder_burned_out(global, &global->coders[index], &current))
		{
			simulation_set_finished(global);
			log_state(&global->coders[index], "burned out");
			wake_all_requests(global);
			return (1);
		}
		index++;
	}
	return (0);
}

static void	monitor_wait(t_global *global)
{
	int				status;
	struct timespec	deadline;

	clock_gettime(CLOCK_REALTIME, &deadline);
	deadline.tv_nsec += 1000000L;
	if (deadline.tv_nsec >= 1000000000L)
	{
		deadline.tv_sec++;
		deadline.tv_nsec -= 1000000000L;
	}
	pthread_mutex_lock(&global->finished_mutex);
	status = 0;
	while (!global->finished && status == 0)
		status = pthread_cond_timedwait(&global->scheduler_cond,
				&global->finished_mutex, &deadline);
	pthread_mutex_unlock(&global->finished_mutex);
}

void	*monitor_thread(void *data)
{
	t_global	*global;

	global = (t_global *)data;
	while (!simulation_is_finished(global))
	{
		if (all_coders_completed(global))
		{
			simulation_set_finished(global);
			break ;
		}
		if (check_burnout(global))
			return (NULL);
		monitor_wait(global);
	}
	return (NULL);
}
