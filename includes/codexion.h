#ifndef CODEXION_H
# define CODEXION_H

#include <pthread.h>
#include <time.h>

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	int				state;
	struct timespec	last_release;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		thread_id;
	struct timespec	last_compile_start;
	int				compiles_done;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	struct s_global	*global;
}	t_coder;

typedef struct s_global
{
	int				number_of_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	int				number_of_compiles_required;
	long			dongle_cooldown;
	int				scheduler;

	struct timespec	start_time;

	int				finished;
	pthread_mutex_t	finished_mutex;
	pthread_mutex_t	log_mutex;

	t_dongle		*dongles;

	void			*scheduler_queue;
	pthread_mutex_t	scheduler_mutex;
	pthread_cond_t	scheduler_cond;
}	t_global;

#endif

