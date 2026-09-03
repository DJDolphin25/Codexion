/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 15:52:34 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/03 15:25:46 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

static void	*execute_thread(void *data)
{
	int		waiting_time;
	t_coder	*coder;

	coder = (t_coder *)data;
	waiting_time = coder->global->time_to_compile * 1000;
	printf("Coder id: %d is working\n", coder->id);
	usleep(waiting_time);
	return (NULL);
}

int	thread_creation(t_global *global)
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
			return (0);
		}
		i++;
	}
	i = 0;
	while (i < global->number_of_coders)
	{
		if (pthread_join(global->coders[i].thread_id, NULL) != 0)
		{
			fprintf(stderr, "Failed to join thread %lu\n",
				global->coders[i].thread_id);
			return (0);
		}
		i++;
	}
	return (1);
}

int	init_coders(t_global *global)
{
	int	i;

	global->coders = malloc(sizeof(t_coder) * global->number_of_coders);
	if (!global->coders)
		return (0);
	i = 0;
	while (i < global->number_of_coders)
	{
		global->coders[i].id = i + 1;
		global->coders[i].global = global;
		i++;
	}
	return (1);
}
