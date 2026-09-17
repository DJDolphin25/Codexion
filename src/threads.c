/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 15:52:34 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/03 17:29:14 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

static void	*execute_thread(void *data)
{
	int		waiting_time;
	int		attempts;
	t_coder	*coder;

	attempts = 0;
	coder = (t_coder *)data;
	waiting_time = coder->global->time_to_compile * 1000;
	while (attempts < 1000)
	{
		if (take_dongle(coder->left_dongle) == 1)
		{
			printf("%d has taken a dongle\n", coder->id);
			usleep(waiting_time);
			drop_dongle(coder->left_dongle);
			printf("%d has released a dongle\n", coder->id);
			break ;
		}
		else
		{
			printf("%d waiting for dongle \n", coder->id);
			usleep(10000);
			attempts++;
		}
	}
	return (NULL);
}

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
