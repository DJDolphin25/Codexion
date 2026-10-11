/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 16:41:19 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/18 17:43:53 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

<<<<<<< HEAD
static int	acquire_one_dongle(t_coder *coder, t_dongle *dongle)
{
	if (!take_dongle(coder, dongle))
		return (0);
	log_state(coder, "has taken a dongle");
	return (1);
}

int	acquire_dongles(t_coder *coder)
{
	if (coder->id % 2 == 0)
	{
		if (!acquire_one_dongle(coder, coder->left_dongle))
			return (0);
		if (!acquire_one_dongle(coder, coder->right_dongle))
		{
			drop_dongle(coder->left_dongle);
			return (0);
		}
	}
	else
	{
		if (!acquire_one_dongle(coder, coder->right_dongle))
			return (0);
		if (!acquire_one_dongle(coder, coder->left_dongle))
		{
			drop_dongle(coder->right_dongle);
			return (0);
		}
	}
	return (1);
=======
void	acquire_dongles(t_coder *coder)
{
	if (coder->id % 2 == 0)
	{
		take_dongle(coder->left_dongle);
		log_state(coder, "has taken a dongle");

		take_dongle(coder->right_dongle);
		log_state(coder, "has taken a dongle");
	}
	else
	{
		take_dongle(coder->right_dongle);
		log_state(coder, "has taken a dongle");

		take_dongle(coder->left_dongle);
		log_state(coder, "has taken a dongle");
	}
>>>>>>> 68f497b8d68ae8cbedd2c0b0e8af99a9d9a6761f
}

int	init_dongles(t_global *global)
{
	int		i;

	global->dongles = malloc(sizeof(t_dongle) * global->number_of_coders);
	if (!global->dongles)
		return (0);
	i = 0;
	while (i < global->number_of_coders)
	{
		if (pthread_mutex_init(&global->dongles[i].state_lock, NULL) != 0)
		{
			fprintf(stderr, "Failed to mutex dongle %d\n", i);
			destroy_mutex(global, i);
			free(global->dongles);
			global->dongles = NULL;
			return (0);
		}
		if (pthread_cond_init(&global->dongles[i].available_cond, NULL) != 0)
		{
			fprintf(stderr, "Failed to cond dongle %d\n", i);
			destroy_mutex(global, i);
			destroy_cond(global, i);
			free(global->dongles);
			global->dongles = NULL;
			return (0);
		}
		global->dongles[i].global = global;
		global->dongles[i].state = FREE;
		global->dongles[i].last_release.tv_sec = 0;
		global->dongles[i].last_release.tv_nsec = 0;
		i++;
	}
	if (!init_scheduler_queues(global))
	{
		destroy_mutex(global, global->number_of_coders);
		destroy_cond(global, global->number_of_coders);
		free(global->dongles);
		global->dongles = NULL;
		return (0);
	}
	return (1);
}
