/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: theoppon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 16:41:19 by theoppon          #+#    #+#             */
/*   Updated: 2026/09/18 17:43:53 by theoppon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

int	take_dongle(t_dongle *dongle)
{
	while (dongle->state != FREE)
	{
		pthread_mutex_lock(&dongle->state_lock);
		if (dongle->state == TAKEN)
			pthread_cond_wait(&dongle->available_cond, &dongle->state_lock);
		else if (dongle->state == COOLDOWN)
		{
			clock_gettime(CLOCK_REALTIME, &dongle->now);
			dongle->target_time = dongle->last_release;
			dongle->target_time.tv_nsec += dongle->global->dongle_cooldown * 1000000;
			{
				if (dongle->target_time.tv_nsec >= 1000000000)
				{
					dongle->target_time.tv_sec += dongle->target_time.tv_nsec / 1000000000;
					dongle->target_time.tv_nsec %= 1000000000
					;
				}
				if (dongle->now.tv_sec < dongle->target_time.tv_sec ||
					(dongle->now.tv_sec == dongle->target_time.tv_sec &&
					dongle->now.tv_nsec < dongle->target_time.tv_nsec ))
					pthread_cond_timedwait(&dongle->available_cond, &dongle->state_lock,
					&dongle->target_time);
				else
					dongle->state = FREE;
			}
		}
	}
	dongle->state = TAKEN;
	pthread_mutex_unlock(&dongle->state_lock);
	return (1);
}

int	drop_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->state_lock);
	dongle->state = COOLDOWN;
	clock_gettime(CLOCK_REALTIME, &dongle->last_release);
	pthread_cond_signal(&dongle->available_cond);
	pthread_mutex_unlock(&dongle->state_lock);
	return (1);
}

void	acquire_dongles(t_coder *coder)
{
	t_global *global;

    global = coder->left_dongle->global;
	if (coder->id % 2 == 0)
	{
		pthread_mutex_lock(&global->log_mutex);
        fprintf(stderr, "[Coder %d] Esperando dongle izq #%td\n",
            coder->id, coder->left_dongle - global->dongles);
        pthread_mutex_unlock(&global->log_mutex);
		
		take_dongle(coder->left_dongle);

		pthread_mutex_lock(&global->log_mutex);
        fprintf(stderr, "[Coder %d] Tomado izq #%td. Esperando der #%td\n",
            coder->id, coder->left_dongle - global->dongles, coder->right_dongle - global->dongles);
        pthread_mutex_unlock(&global->log_mutex);
		
		take_dongle(coder->right_dongle);
	}
	else
	{
		pthread_mutex_lock(&global->log_mutex);
        fprintf(stderr, "[Coder %d] Esperando dongle der #%td\n",
            coder->id, coder->right_dongle - global->dongles);
        pthread_mutex_unlock(&global->log_mutex);
		
		take_dongle(coder->right_dongle);
		
		pthread_mutex_lock(&global->log_mutex);
        fprintf(stderr, "[Coder %d] Tomado der #%td. Esperando izq #%td\n",
            coder->id, coder->right_dongle - global->dongles, coder->left_dongle - global->dongles);
        pthread_mutex_unlock(&global->log_mutex);
		
		take_dongle(coder->left_dongle);
	}
	pthread_mutex_lock(&global->log_mutex);
    fprintf(stderr, "[Coder %d] AMBOS DONGLES CONSEGUIDOS (#%td y #%td)\n",
        coder->id, coder->left_dongle - global->dongles, coder->right_dongle - global->dongles);
    pthread_mutex_unlock(&global->log_mutex);
}

int	init_dongles(t_global *global)
{
	int		i;
	enum 	e_dongle_state condition;

	global->dongles = malloc(sizeof(t_dongle) * global->number_of_coders);
	if (!global->dongles)
		return (0);
	i = 0;
	condition = FREE;
	while (i < global->number_of_coders)
	{
		if (pthread_mutex_init(&global->dongles[i].state_lock, NULL) != 0)
		{
			fprintf(stderr, "Failed to mutex dongle %d\n", i);
			destroy_mutex(global, i);
			return (0);
		}
		if (pthread_cond_init(&global->dongles[i].available_cond, NULL) != 0)
		{
			fprintf(stderr, "Failed to cond dongle %d\n", i);
			destroy_mutex(global, i);
			destroy_cond(global, i);
			free(global->dongles);
			global->dongles = NULL;
			return (0);
		}
		global->dongles[i].global = global;
		global->dongles[i].state = condition;
		global->dongles[i].last_release.tv_sec = 0;
		global->dongles[i].last_release.tv_nsec = 0;
		i++;
	}
	return (1);
}
