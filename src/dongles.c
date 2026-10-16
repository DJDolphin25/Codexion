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
}

int	drop_dongle(t_dongle *dongle)
{
	t_fifo_request	*top;
	t_fifo_queue	*queue;

	pthread_mutex_lock(&dongle->state_lock);
	dongle->state = COOLDOWN;
	clock_gettime(CLOCK_REALTIME, &dongle->last_release);
	queue = scheduler_get_queue(dongle);
	top = scheduler_queue_top(queue);
	if (top != NULL)
		pthread_cond_signal(&top->request_cond);
	pthread_mutex_unlock(&dongle->state_lock);
	return (1);
}

static int	init_dongle_locks(t_global *global)
{
	int	i;

	i = 0;
	while (i < global->number_of_coders)
	{
		if (pthread_mutex_init(&global->dongles[i].state_lock, NULL) != 0)
		{
			fprintf(stderr, "Failed to init mutex of dongle %d\n", i);
			destroy_mutex(global, i);
			return (0);
		}
		global->dongles[i].global = global;
		global->dongles[i].state = FREE;
		global->dongles[i].last_release.tv_sec = 0;
		global->dongles[i].last_release.tv_nsec = 0;
		i++;
	}
	return (1);
}

int	init_dongles(t_global *global)
{
	global->dongles = malloc(sizeof(t_dongle) * global->number_of_coders);
	if (!global->dongles)
		return (0);
	if (!init_dongle_locks(global))
	{
		free(global->dongles);
		global->dongles = NULL;
		return (0);
	}
	if (!init_scheduler_queues(global))
	{
		destroy_mutex(global, global->number_of_coders);
		free(global->dongles);
		global->dongles = NULL;
		return (0);
	}
	return (1);
}
