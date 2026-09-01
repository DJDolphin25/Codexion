/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 15:22:17 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/01 15:25:22 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

static int	parse_scheduler(char *str, t_args *args)
{
	char	*s;

	s = str;
	while (*s)
	{
		if (!((*s >= 'A' && *s <= 'Z') || (*s >= 'a' && *s <= 'z')))
		{
			fprintf(stderr, "%s", "Last argument should only contain ");
			fprintf(stderr, "%s", "alphabetical characters\n");
			return (0);
		}
		else if (*s >= 'A' && *s <= 'Z')
			*s += 32;
		s++;
	}
	if (!(strcmp(str, "fifo") == 0 || strcmp(str, "edf") == 0))
	{
		fprintf(stderr, "%s", "Last argument should either be ");
		fprintf(stderr, "%s", "'fifo' or 'edf'\n");
		return (0);
	}
	args->scheduler = str;
	return (1);
}

static int	parse_numbers(int i, char *str)
{
	char	*s;

	s = str;
	while (*s)
	{
		if (!(*s >= '0' && *s <= '9'))
		{
			fprintf(stderr, "Argument %d '%s'", i, str);
			fprintf(stderr, "%s", " must be a number\n");
			return (0);
		}
		s++;
	}
	if ((strlen(str) == 10 && strcmp(str, "2147483647") > 0)
		|| strlen(str) > 10)
	{
		fprintf(stderr, "Argument %d '%s'", i, str);
		fprintf(stderr, "%s", " must be less than INT_MAX\n");
		return (0);
	}
	return (1);
}

static int	conversion(int i, char *str, t_args *args)
{
	int	value;

	value = atoi(str);
	if (value <= 0)
	{
		fprintf(stderr, "Argument %d '%s'", i, str);
		fprintf(stderr, "%s", " must be greater than 0\n");
		return (0);
	}
	if (i == 1)
		args->number_of_coders = value;
	else if (i == 2)
		args->time_to_burnout = value;
	else if (i == 3)
		args->time_to_compile = value;
	else if (i == 4)
		args->time_to_debug = value;
	else if (i == 5)
		args->time_to_refactor = value;
	else if (i == 6)
		args->number_of_compiles_required = value;
	else if (i == 7)
		args->dongle_cooldown = value;
	return (1);
}

void	transfer_to_global(t_global *global, t_args *args)
{
	if (global = NULL )
	global->finished = 0; // Sin empezar
	global->number_of_coders = args->number_of_coders;
	global->time_to_burnout = args->time_to_burnout;
	global->time_to_compile = args->time_to_compile;
	global->time_to_debug = args->time_to_debug;
	global->time_to_refactor = args->time_to_refactor;
	global->number_of_compiles_required = args->number_of_compiles_required;
	global->dongle_cooldown = args->dongle_cooldown;
	global->scheduler = args->scheduler;

	global->dongles = NULL;
}

int	parse_arg(int ac, char **av, t_args *args)
{
	int	i;

	memset(args, 0, sizeof(t_args));
	i = 1;
	while (i < ac)
	{
		if (av[i] == NULL || av[i][0] == '\0')
		{
			fprintf(stderr, "Argument %d can't be empty or NULL\n", i);
			return (0);
		}
		if (i == ac - 1)
			if (parse_scheduler(av[i], args) == 0)
				return (0);
		else
		{
			if (parse_numbers(i, av[i]) == 0)
				return (0);
			if (conversion(i, av[i], args) == 0)
				return (0);
		}
		i++;
	}
	return (1);
}
