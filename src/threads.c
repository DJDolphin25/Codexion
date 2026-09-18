/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:09:33 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/18 17:09:36 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

static int	join_threads(t_global *global, int limit)
{
	int	i;
	int error;

	i = 0;
	error = 1;
	while (i < limit)
	{
		if (pthread_join(global->coders[i].thread_id, NULL) != 0)
		{
			fprintf(stderr, "Failed to join thread %lu\n",
				global->coders[i].thread_id);
			error = 0;
		}
		i++;
	}
	return (error);
}

static int	create_threads(t_global *global)
{
	int	i;

	i = 0;
	while (i < global->number_of_coders)
	{
		if (pthread_create(&global->coders[i].thread_id, NULL,
				execute_thread, &global->coders[i]) != 0)
		{
			fprintf(stderr, "Failed to create thread %lu\n",
				global->coders[i].thread_id);
			join_threads(global, i);
			return (0);
		}
		i++;
	}
	return (1);
}

int	thread_creation(t_global *global)
{
	if (!create_threads(global))
		return (0);
	if (!join_threads(global, global->number_of_coders))
		return (0);
	return (1);
}
