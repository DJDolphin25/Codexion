/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 15:06:53 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/01 15:09:05 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

int	main(int ac, char **av)
{
	t_args		args;
	t_global	global;

	if (ac == 9)
	{
		if(parse_args(ac, av, &args) == 0)
			return (1);
		if (transfer_to_global(&global, &args) == 0)
			return (1);
		if (init_coders(&global) == 0)
			return (1);
		if (thread_creation(&global) == 0)
			return (1);
	}
	else
	{
		fprintf(stderr, "%s", "Number of arguments different from 9.\n");
		return (1);
	}
	return (0);
}
