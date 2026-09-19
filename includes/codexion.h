/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 14:57:42 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/18 20:21:39 by theoppon         ###   ########.fr       */
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

enum	e_dongle_state
{
	COOLDOWN,
	FREE,
	TAKEN,
};

typedef struct s_dongle
{
	int				state;

	pthread_mutex_t	state_lock;
	pthread_cond_t	available_cond;

	struct s_global	*global;

	struct timespec	last_release;
	struct timespec	now;
	struct timespec target_time;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	int				compiles_done;

	pthread_t		thread_id;

	struct s_global	*global;

	struct timespec	last_compile_start;

	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
}	t_coder;

typedef struct s_args
{
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	long			dongle_cooldown;

	char			*scheduler;
}	t_args;

typedef struct s_global
{
	int				finished;
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	long			dongle_cooldown;

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

int		parse_args(int ac, char **av, t_args *args);
int		transfer_to_global(t_global *global, t_args *args);

int		thread_creation(t_global *global);
void	*execute_thread(void *data);

int		init_coders(t_global *global);

void	acquire_dongles(t_coder *coder);
int		drop_dongle(t_dongle *dongle);
int		init_dongles(t_global *global);
int		take_dongle(t_dongle *dongle);

int		destroy_mutex(t_global *global, int limit);
int		destroy_cond(t_global *global, int limit);

int		init_global(t_global *global);
int		destroy_global(t_global *global);

#endif
