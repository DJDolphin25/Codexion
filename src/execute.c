/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 14:17:18 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/18 14:17:20 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

static void		compile_thread(t_coder *coder)
{
	int		compile_time;

	compile_time = coder->global->time_to_compile * 1000;
	acquire_dongles(coder);
	pthread_mutex_lock(&coder->global->log_mutex);
	printf("%d is compiling\n", coder->id);
	pthread_mutex_unlock(&coder->global->log_mutex);
	usleep(compile_time);
	drop_dongle(coder->left_dongle);
	drop_dongle(coder->right_dongle);
}

static void		debug_thread(t_coder *coder)
{
	int		debug_time;

	debug_time = coder->global->time_to_debug * 1000;
	pthread_mutex_lock(&coder->global->log_mutex);
	printf("%d is debugging\n", coder->id);
	pthread_mutex_unlock(&coder->global->log_mutex);
	usleep(debug_time);
}

static void		refactor_thread(t_coder *coder)
{
	int		refactoring_time;

	refactoring_time = coder->global->time_to_refactor * 1000;
	pthread_mutex_lock(&coder->global->log_mutex);
	printf("%d is refactoring\n", coder->id);
	pthread_mutex_unlock(&coder->global->log_mutex);
	usleep(refactoring_time);
}

void	*execute_thread(void *data)
{
	t_coder	*coder;

	coder = (t_coder *)data;
	while(1)
	{
		compile_thread(coder);
		debug_thread(coder);
		refactor_thread(coder);
	}
	return (NULL);
}