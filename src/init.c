/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 17:46:06 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/03 17:46:22 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

static int destroy_mutex(t_global *global, int limit)
{
	int	i;

	i = 0;
	while (i < limit)
	{
		if (pthread_mutex_destroy(&global->dongles[i].mutex) != 0)
		{
			fprintf(stderr, "Failed to destry mutex %d\n", i);
			return (0);
		}
		i++;
	}
	return (1);
}

int	init_dongles(t_global *global)
{
	int		i;
	enum	dongle_state condition;

	global->dongles = malloc(sizeof(t_dongle) * global->number_of_coders);
	if (!global->dongles)
		return (0);
	i = 0;
	condition = FREE;
	while (i < global->number_of_coders)
	{
		global->dongles[i].state = condition;
		global->dongles[i].last_release.tv_sec = 0;
		global->dongles[i].last_release.tv_nsec = 0;
		if (pthread_mutex_init(&global->dongles[i].mutex, NULL) != 0)
		{
			fprintf(stderr, "Failed to mutex dongle %d\n", i);
			destroy_mutex(global, i);
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
    	global->coders[i].left_dongle = &global->dongles[i];
    	global->coders[i].right_dongle =
        	&global->dongles[(i + 1) % global->number_of_coders];
    	i++;
	}
	return (1);
}