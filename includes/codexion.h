/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 14:57:42 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/01 15:06:08 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <unistd.h>

//Structure for the dongle
typedef struct s_dongle
{
	int				state;

	pthread_mutex_t	mutex;

	struct timespec	last_release;
}	t_dongle;

//Structure for the coder
typedef struct s_coder
{
	int				id;
	int				compile_done;

	pthread_t		thread_id;

	struct timespec	last_compile_start;
	struct s_global	*global;

	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
}	t_coder;

//Structure for the arguments
typedef struct s_args
{
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;

	char			*scheduler;
}	t_args;

//Structure for the global variables
typedef struct s_global
{
	int				finished;
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;

	char			*scheduler;

	pthread_mutex_t	finished_mutex;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	scheduler_mutex;
	pthread_cond_t	scheduler_cond;

	struct timespec	start_time;

	t_dongle		*dongles;
	t_coder			*coders;

	void			*scheduler_queue;
}	t_global;

//Parser funtcion
int	parse_args(int ac, char **av, t_args *args);

int	thread_creation(t_global *global);
int	init_coders(t_global *global);

int	transfer_to_global(t_global *global, t_args *args);

#endif
