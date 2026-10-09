/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 14:17:18 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/18 17:45:51 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

static void	sleep_microseconds(long microseconds)
{
	struct timespec	request;

	request.tv_sec = microseconds / 1000000L;
	request.tv_nsec = (microseconds % 1000000L) * 1000L;
	nanosleep(&request, NULL);
}

static void	compile_thread(t_coder *coder)
{
	int	compile_time;

	compile_time = coder->global->time_to_compile * 1000;
	acquire_dongles(coder);
	log_state(coder, "is compiling");
	sleep_microseconds(compile_time);
	drop_dongle(coder->left_dongle);
	drop_dongle(coder->right_dongle);
}

static void	debug_thread(t_coder *coder)
{
	int	debug_time;

	debug_time = coder->global->time_to_debug * 1000;
	log_state(coder, "is debugging");
	sleep_microseconds(debug_time);
}

static void	refactor_thread(t_coder *coder)
{
	int	refactoring_time;

	refactoring_time = coder->global->time_to_refactor * 1000;
	log_state(coder, "is refactoring");
	sleep_microseconds(refactoring_time);
}

void	*execute_thread(void *data)
{
	t_coder	*coder;

	coder = (t_coder *)data;
	coder->compiles_done = 0;
	while (coder->compiles_done < coder->global->number_of_compiles_required)
	{
		compile_thread(coder);
		coder->compiles_done++;
		debug_thread(coder);
		refactor_thread(coder);
	}
	return (NULL);
}
