/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 15:48:29 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/10 15:48:34 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

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
