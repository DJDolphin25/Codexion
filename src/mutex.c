/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 15:42:47 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/18 17:47:46 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

int	destroy_mutex(t_global *global, int limit)
{
	int	i;
	int	error;

	i = 0;
	error = 1;
	while (i < limit)
	{
		if (pthread_mutex_destroy(&global->dongles[i].state_lock) != 0)
		{
			fprintf(stderr, "Failed to destry mutex %d\n", i);
			error = 0;
		}
		i++;
	}
	return (error);
}

int	init_global(t_global *global)
{
	if (global == NULL)
		return (0);
	global->finished = 0;
	pthread_cond_init(&global->finish_cond, NULL);
	if (pthread_mutex_init(&global->log_mutex, NULL) != 0)
	{
		fprintf(stderr, "%s", "Failed to initialize log mutex\\n");
		return (0);
	}
	if (pthread_mutex_init(&global->finished_mutex, NULL) != 0)
	{
		fprintf(stderr, "%s", "Failed to initialize finished mutex\\n");
		pthread_mutex_destroy(&global->log_mutex);
		return (0);
	}
	if (pthread_cond_init(&global->scheduler_cond, NULL) != 0)
	{
		fprintf(stderr, "%s", "Failed to initialize scheduler condition\\n");
		pthread_mutex_destroy(&global->finished_mutex);
		pthread_mutex_destroy(&global->log_mutex);
		return (0);
	}
	clock_gettime(CLOCK_REALTIME, &global->start_time);
	return (1);
}

static int	destroy_sync(t_global *global)
{
	int	error;

	error = 1;
	if (pthread_cond_destroy(&global->scheduler_cond) != 0)
	{
		fprintf(stderr, "%s", "Failed to destroy scheduler condition\n");
		error = 0;
	}
	if (pthread_mutex_destroy(&global->finished_mutex) != 0)
	{
		fprintf(stderr, "%s", "Failed to destroy finished mutex\n");
		error = 0;
	}
	if (pthread_mutex_destroy(&global->log_mutex) != 0)
	{
		fprintf(stderr, "%s", "Failed to destroy log mutex\n");
		error = 0;
	}
	pthread_cond_destroy(&global->finish_cond);
	return (error);
}

int	destroy_global(t_global *global)
{
	int	error;

	if (global == NULL)
		return (0);
	error = 1;
	destroy_scheduler_queues(global);
	if (global->dongles != NULL)
	{
		if (!destroy_mutex(global, global->number_of_coders))
			error = 0;
		free(global->dongles);
		global->dongles = NULL;
	}
	free(global->coders);
	global->coders = NULL;
	if (!destroy_sync(global))
		error = 0;
	return (error);
}
