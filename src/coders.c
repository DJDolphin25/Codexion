/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 15:52:34 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/01 15:52:35 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>


void	*thread_creation()
{
	int 	i;
	t_coder	*coders;

	i = 0;
	while(i < coders->global->number_of_coders)
	{
		if (pthread_create(coders[i]->thread_id, NULL, funcion, coders[i]) != 0)
			fprintf(stderr, "Failed to create thread %ld", coders[i]->thread_id)
		i++;
	}
	i = 0;
	while(i < coders->global->number_of_coders)
	{
		if (pthread_join(&coders[i]->thread_id, NULL))
			fprintf(stderr, "Failed to create thread %ld", &coders[i]->thread_id)
		i++;
	}
}

int    init_coders(t_global *global)
{
		int			i;

    global->coders = malloc(sizeof(t_coder) * global->number_of_coders);
		if (!global->coders)
			return (0);
		i = 0;
		while(i < global->number_of_coders)
		{
			global->coders[i].id = i + 1;
			i++;
		}
    return (1);
}
