/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 14:17:18 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/18 17:45:51 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

static int	sleep_microseconds(long microseconds, t_global *global)
{
	int	status;
	struct timespec	deadline;

	clock_gettime(CLOCK_REALTIME, &deadline);
	deadline.tv_nsec += microseconds * 1000L;
	deadline.tv_sec += deadline.tv_nsec / 1000000000L;
	deadline.tv_nsec %= 1000000000L;
	pthread_mutex_lock(&global->finished_mutex);
	while (!global->finished)
	{
		status = pthread_cond_timedwait(&global->scheduler_cond,
				&global->finished_mutex, &deadline);
		if (status == )
			break ;
		if (status != 0)
		{
			pthread_mutex_unlock(&global->finished_mutex);
			return (0);
		}
	}
	status = !global->finished;
	pthread_mutex_unlock(&global->finished_mutex);
	return (status);
}

static int	compile_thread(t_coder *coder)
{
	int	compile_time;

	compile_time = (long)coder->global->time_to_compile * 1000L;
	if (!acquire_dongles(coder))
		return (0);
	if (simulation_is_finished(coder->global))
	{
		drop_dongle(coder->left_dongle);
		drop_dongle(coder->right_dongle);
		return (0);
	}
	pthread_mutex_lock(&coder->global->finished_mutex);
	clock_gettime(CLOCK_REALTIME, &coder->last_compile_start);
	pthread_mutex_unlock(&coder->global->finished_mutex);
	log_state(coder, "is compiling");
	if (!sleep_microseconds(compile_time, coder->global))
	{
		drop_dongle(coder->left_dongle);
		drop_dongle(coder->right_dongle);
		return (0);
	}
	drop_dongle(coder->left_dongle);
	drop_dongle(coder->right_dongle);
	return (!simulation_is_finished(coder->global));
}

static int	debug_thread(t_coder *coder)
{
	int	debug_time;

	if (simulation_is_finished(coder->global))
		return (0);
	debug_time = (long)coder->global->time_to_debug * 1000L;
	log_state(coder, "is debugging");
	return (sleep_microseconds(debug_time, coder->global));
}

static int	refactor_thread(t_coder *coder)
{
	int	refactoring_time;

	if (simulation_is_finished(coder->global))
		return (0);
	refactoring_time = (long)coder->global->time_to_refactor * 1000L;
	log_state(coder, "is refactoring");
	return (sleep_microseconds(refactoring_time, coder->global));
}

void	*execute_thread(void *data)
{
	t_coder	*coder;

	coder = (t_coder *)data;
	pthread_mutex_lock(&coder->global->finished_mutex);
	coder->compiles_done = 0;
	pthread_mutex_unlock(&coder->global->finished_mutex);
	while (!simulation_is_finished(coder->global))
	{
		pthread_mutex_lock(&coder->global->finished_mutex);
		if (coder->compiles_done >= coder->global->number_of_compiles_required)
		{
			pthread_mutex_unlock(&coder->global->finished_mutex);
			break ;
		}
		pthread_mutex_unlock(&coder->global->finished_mutex);
		if (!compile_thread(coder))
			return (NULL);
		pthread_mutex_lock(&coder->global->finished_mutex);
		coder->compiles_done++;
		pthread_mutex_unlock(&coder->global->finished_mutex);
		if (simulation_is_finished(coder->global))
			return (NULL);
		if (!debug_thread(coder))
			return (NULL);
		if (!refactor_thread(coder))
			return (NULL);
	}
	return (NULL);
}
