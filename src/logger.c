/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 00:00:00 by theoppon          #+#    #+#             */
/*   Updated: 2026/10/09 00:00:00 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

long	timestamp_ms(t_global *global)
{
	struct timespec	current;
	long			seconds;
	long			nanoseconds;

	clock_gettime(CLOCK_REALTIME, &current);
	seconds = current.tv_sec - global->start_time.tv_sec;
	nanoseconds = current.tv_nsec - global->start_time.tv_nsec;
	return (seconds * 1000L + nanoseconds / 1000000L);
}

void	log_state(t_coder *coder, const char *state)
{
	t_global	*global;

	global = coder->global;
	pthread_mutex_lock(&global->log_mutex);
	printf("%ld %d %s\n", timestamp_ms(global), coder->id, state);
	pthread_mutex_unlock(&global->log_mutex);
}