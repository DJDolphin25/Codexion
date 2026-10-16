/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_burnout.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 21:10:39 by theoppon          #+#    #+#             */
/*   Updated: 2026/10/15 21:10:40 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

int	coder_burned_out(t_global *global, t_coder *coder,
			struct timespec *current)
{
	struct timespec	deadline;

	clock_gettime(CLOCK_REALTIME, current);
	pthread_mutex_lock(&global->finished_mutex);
	deadline = coder->last_compile_start;
	pthread_mutex_unlock(&global->finished_mutex);
	add_milliseconds(&deadline, global->time_to_burnout);
	if (current->tv_sec < deadline.tv_sec)
		return (0);
	if (current->tv_sec > deadline.tv_sec)
		return (1);
	if (current->tv_nsec < deadline.tv_nsec)
		return (0);
	return (1);
}

void	wake_all_requests(t_global *global)
{
	int	index;

	index = 0;
	while (index < global->number_of_coders)
	{
		pthread_mutex_lock(&global->dongles[index].state_lock);
		scheduler_queue_signal_all(&global->scheduler_queue[index]);
		pthread_mutex_unlock(&global->dongles[index].state_lock);
		index++;
	}
}
