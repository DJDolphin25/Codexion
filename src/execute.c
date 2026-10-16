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
	int				status;
	struct timespec	deadline;

	clock_gettime(CLOCK_REALTIME, &deadline);
	deadline.tv_nsec += microseconds * 1000L;
	deadline.tv_sec += deadline.tv_nsec / 1000000000L;
	deadline.tv_nsec %= 1000000000L;
	pthread_mutex_lock(&global->finished_mutex);
	while (!global->finished)
	{
		status = pthread_cond_timedwait(&global->finish_cond,
				&global->finished_mutex, &deadline);
		if (status == ETIMEDOUT)
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

static void	release_dongles(t_coder *coder)
{
	drop_dongle(coder->left_dongle);
	if (coder->left_dongle != coder->right_dongle)
		drop_dongle(coder->right_dongle);
}

static int	compile_thread(t_coder *coder)
{
	long	compile_time;

	compile_time = (long)coder->global->time_to_compile * 1000L;
	if (!acquire_dongles(coder))
		return (0);
	if (simulation_is_finished(coder->global))
	{
		release_dongles(coder);
		return (0);
	}
	pthread_mutex_lock(&coder->global->finished_mutex);
	clock_gettime(CLOCK_REALTIME, &coder->last_compile_start);
	pthread_mutex_unlock(&coder->global->finished_mutex);
	log_state(coder, "is compiling");
	if (!sleep_microseconds(compile_time, coder->global))
	{
		release_dongles(coder);
		return (0);
	}
	release_dongles(coder);
	return (!simulation_is_finished(coder->global));
}

static int	run_phase(t_coder *coder, const char *state, int time_ms)
{
	if (simulation_is_finished(coder->global))
		return (0);
	log_state(coder, state);
	return (sleep_microseconds((long)time_ms * 1000L, coder->global));
}

void	*execute_thread(void *data)
{
	t_coder		*coder;
	t_global	*global;
	int			done;

	coder = (t_coder *)data;
	global = coder->global;
	pthread_mutex_lock(&global->finished_mutex);
	coder->compiles_done = 0;
	pthread_mutex_unlock(&global->finished_mutex);
	while (!simulation_is_finished(global))
	{
		pthread_mutex_lock(&global->finished_mutex);
		done = (coder->compiles_done
				>= global->number_of_compiles_required);
		pthread_mutex_unlock(&global->finished_mutex);
		if (done || !compile_thread(coder))
			break ;
		pthread_mutex_lock(&global->finished_mutex);
		coder->compiles_done++;
		pthread_mutex_unlock(&global->finished_mutex);
		if (!run_phase(coder, "is debugging", global->time_to_debug)
			|| !run_phase(coder, "is refactoring", global->time_to_refactor))
			break ;
	}
	return (NULL);
}
