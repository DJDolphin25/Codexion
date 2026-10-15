/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 15:06:53 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/18 17:46:30 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

int	main(int ac, char **av)
{
	t_args		args;
	t_global	global;

	if (ac != 9)
	{
		fprintf(stderr, "%s", "Number of arguments different from 9.\\n");
		return (1);
	}
	if (!parse_args(ac, av, &args))
		return (1);
	if (!transfer_to_global(&global, &args))
		return (1);
	if (!init_global(&global))
		return (1);
	if (!init_dongles(&global))
	{
		destroy_global(&global);
		return (1);
	}
	if (!init_coders(&global))
	{
		destroy_global(&global);
		return (1);
	}
	if (!thread_creation(&global))
	{
		destroy_global(&global);
		return (1);
	}
	if (!destroy_global(&global))
		return (1);
	return (0);
}
