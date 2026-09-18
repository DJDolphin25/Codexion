/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:36:54 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/18 17:49:15 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

int	init_global(t_global *global)
{
	if (global == NULL)
		return (0);
	if (pthread_mutex_init(&global->log_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&global->finished_mutex);
		return (0);
	}
	return (1);
}

void	destroy_global(t_global *global)
{
	if (global == NULL)
		return ;
	pthread_mutex_destroy(&global->log_mutex);
}
