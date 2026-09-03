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
		i++;
	}
	return (1);
}
