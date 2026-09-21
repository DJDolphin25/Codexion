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

int	destroy_cond(t_global *global, int limit)
{
	int	i;
	int	error;

	i = 0;
	error = 1;
	while (i < limit)
	{
		if (pthread_cond_destroy(&global->dongles[i].available_cond) != 0)
		{
			fprintf(stderr, "Failed to destroy cond %d\n", i);
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
	if (pthread_mutex_init(&global->log_mutex, NULL) != 0)
	{
		fprintf(stderr, "%s", "Failed to initialize log mutex\n");
		return (0);
	}
	return (1);
}

int	destroy_global(t_global *global)
{
	if (global == NULL)
		return (0);
	if (pthread_mutex_destroy(&global->log_mutex) != 0)
	{
		fprintf(stderr, "%s", "Failed to destroy log mutex\n");
		return (0);
	}
	return (1);
}
