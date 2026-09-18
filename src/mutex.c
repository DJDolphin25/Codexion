/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 15:42:47 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/18 15:42:49 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

int destroy_mutex(t_global *global, int limit)
{
	int	i;
	int error;

	i = 0;
	error = 1;
	while (i < limit)
	{
		if (pthread_mutex_destroy(&global->dongles[i].mutex) != 0)
		{
			fprintf(stderr, "Failed to destry mutex %d\n", i);
			error = 0;
		}
		i++;
	}
	return (error);
}
