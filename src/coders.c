/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 16:40:25 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/18 17:41:59 by theoppon         ###   ########.fr       */
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
		global->coders[i].left_dongle = &global->dongles[(i - 1
				+ global->number_of_coders) % global->number_of_coders];
		global->coders[i].right_dongle = &global->dongles[i];
		i++;
	}
	return (1);
}
