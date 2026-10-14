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

# ifndef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 200809L
# endif

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

typedef struct s_fifo_request
{
	pthread_cond_t		request_cond;
	unsigned long		order;
	struct timespec		deadline;
	pthread_cond_t		request_cond;
	unsigned long		order;
	struct timespec		deadline;
	struct s_fifo_request	*next;
}	t_fifo_request;

typedef struct s_fifo_queue
{
	t_fifo_request		**items;
	int				size;
	int				capacity;
	unsigned long		next_order;
	struct s_global		*global;

	t_fifo_request		*front;
	t_fifo_request		*back;
}	t_fifo_queue;

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
	pthread_t		monitor_thread_id;

	struct timespec	start_time;

	t_dongle		*dongles;
	t_coder			*coders;

	t_fifo_queue	*scheduler_queue;
}	t_global;

int		parse_args(int ac, char **av, t_args *args);
int		transfer_to_global(t_global *global, t_args *args);

int		thread_creation(t_global *global);
void	*execute_thread(void *data);

int		init_coders(t_global *global);

int		init_scheduler_queues(t_global *global);
void	destroy_scheduler_queues(t_global *global);
void	*scheduler_get_queue(t_dongle *dongle);
int		scheduler_queue_is_first(void *queue_ptr, t_fifo_request *request);

void	*scheduler_queue_top(void *queue_ptr);
void	scheduler_queue_push(void *queue_ptr, t_fifo_request *request);
int		scheduler_queue_remove(void *queue_ptr, t_fifo_request *request);
t_fifo_request	*scheduler_queue_pop_front(void *queue_ptr);
void	scheduler_queue_signal_all(void *queue_ptr);
void	scheduler_queue_reset(void *queue_ptr);

void	add_milliseconds(struct timespec *time, long milliseconds);
int		cooldown_expired(t_dongle *dongle, struct timespec *target);
int		request_is_before(t_fifo_queue *queue, t_fifo_request *first,
				 t_fifo_request *second);
void	swap_requests(t_fifo_request **first, t_fifo_request **second);
void	bubble_up(t_fifo_queue *queue, int index);
void	bubble_down(t_fifo_queue *queue, int index);
void	scheduler_queue_push(void *queue_ptr, t_fifo_request *request);


long	timestamp_ms(t_global *global);
void	log_state(t_coder *coder, const char *state);


int		simulation_is_finished(t_global *global);
void	simulation_set_finished(t_global *global);
int		all_coders_completed(t_global *global);
void	*monitor_thread(void *data);
int		coder_burned_out(t_global *global, t_coder *coder,
				struct timespec *current);
void	wake_all_requests(t_global *global);

int		acquire_dongles(t_coder *coder);
int		drop_dongle(t_dongle *dongle);
int		init_dongles(t_global *global);
int		take_dongle(t_coder *coder, t_dongle *dongle);

int		destroy_mutex(t_global *global, int limit);
int		destroy_cond(t_global *global, int limit);

int		init_global(t_global *global);
int		destroy_global(t_global *global);

#endif
