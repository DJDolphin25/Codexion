/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:09:33 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/18 17:52:44 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

static int	join_coders(t_global *global, int limit)
{
	int	i;
	int	error;

	i = 0;
	error = 1;
	while (i < limit)
	{
		if (pthread_join(global->coders[i].thread_id, NULL) != 0)
		{
			fprintf(stderr, "Failed to join coder %d thread\n",
				global->coders[i].id);
			error = 0;
		}
		i++;
	}
	return (error);
}

static int	create_coders(t_global *global, int *created)
{
	int	i;

	i = 0;
	while (i < global->number_of_coders)
	{
		if (pthread_create(&global->coders[i].thread_id, NULL,
				execute_thread, &global->coders[i]) != 0)
		{
			fprintf(stderr, "Failed to create coder %d thread\n",
				global->coders[i].id);
			*created = i;
			return (0);
		}
		i++;
	}
	*created = i;
	return (1);
}

static int	join_monitor(t_global *global)
{
	if (pthread_join(global->monitor_thread_id, NULL) != 0)
	{
		fprintf(stderr, "%s", "Failed to join monitor thread\n");
		return (0);
	}
	return (1);
}

int	thread_creation(t_global *global)
{
	int	created;

	if (pthread_create(&global->monitor_thread_id, NULL, monitor_thread,
			global) != 0)
	{
		fprintf(stderr, "%s", "Failed to create monitor thread\n");
		return (0);
	}
	if (!create_coders(global, &created))
	{
		simulation_set_finished(global);
		wake_all_requests(global);
		join_coders(global, created);
		if (!join_monitor(global))
			return (0);
		return (0);
	}
	if (!join_coders(global, global->number_of_coders))
	{
		simulation_set_finished(global);
		wake_all_requests(global);
		if (!join_monitor(global))
			return (0);
		return (0);
	}
	if (!join_monitor(global))
		return (0);
	return (1);
}
