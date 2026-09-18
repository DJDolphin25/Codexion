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

int	take_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->state == TAKEN)
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	dongle->state = TAKEN;
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}

int	drop_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->state = FREE;
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}

void	acquire_dongles(t_coder *coder)
{
	if (coder->id % 2 == 0)
	{
		while (!take_dongle(coder->left_dongle))
			usleep(2000);
		while (!take_dongle(coder->right_dongle))
			usleep(2000);
	}
	else
	{
		while (!take_dongle(coder->right_dongle))
			usleep(2000);
		while (!take_dongle(coder->left_dongle))
			usleep(2000);
	}
}

int	init_dongles(t_global *global)
{
	int		i;
	enum 	dongle_state condition;

	global->dongles = malloc(sizeof(t_dongle) * global->number_of_coders);
	if (!global->dongles)
		return (0);
	i = 0;
	condition = FREE;
	while (i < global->number_of_coders)
	{
		if (pthread_mutex_init(&global->dongles[i].mutex, NULL) != 0)
		{
			fprintf(stderr, "Failed to mutex dongle %d\n", i);
			destroy_mutex(global, i);
			return (0);
		}
		global->dongles[i].state = condition;
		global->dongles[i].last_release.tv_sec = 0;
		global->dongles[i].last_release.tv_nsec = 0;
		i++;
	}
	return (1);
}
