#include <codexion.h>

void	add_milliseconds(struct timespec *time, long milliseconds)
{
	time->tv_nsec += milliseconds * 1000000L;
	if (time->tv_nsec >= 1000000000L)
	{
		time->tv_sec += time->tv_nsec / 1000000000L;
		time->tv_nsec %= 1000000000L;
	}
}

int	cooldown_expired(t_dongle *dongle, struct timespec *target)
{
	struct timespec	current;

	clock_gettime(CLOCK_REALTIME, &current);
	dongle->target_time = dongle->last_release;
	add_milliseconds(&dongle->target_time, dongle->global->dongle_cooldown);
	*target = dongle->target_time;
	if (current.tv_sec < target->tv_sec)
		return (0);
	if (current.tv_sec > target->tv_sec)
		return (1);
	if (current.tv_nsec < target->tv_nsec)
		return (0);
	return (1);
}
