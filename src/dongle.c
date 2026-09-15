/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kel-hadd <kel-hadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 00:20:53 by kel-hadd          #+#    #+#             */
/*   Updated: 2026/09/15 01:52:06 by kel-hadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long	wake_time(t_data *data, t_wait *wait)
{
	long	a;
	long	b;

	a = data->dongles[wait->dongle_a].free_at;
	if (wait->dongle_b < 0)
		return (a);
	b = data->dongles[wait->dongle_b].free_at;
	if (a == LONG_MAX || b == LONG_MAX)
		return (LONG_MAX);
	if (a > b)
		return (a);
	return (b);
}

static void	wait_for_grant(t_data *data, t_wait *wait)
{
	struct timespec	ts;
	long			wake;

	wake = wake_time(data, wait);
	if (wake == LONG_MAX)
		pthread_cond_wait(&data->alloc_cond, &data->alloc_lock);
	else
	{
		ts.tv_sec = wake / 1000;
		ts.tv_nsec = (wake % 1000) * 1000000;
		pthread_cond_timedwait(&data->alloc_cond, &data->alloc_lock, &ts);
	}
	if (wait->state == WAITING && !is_stopped(data))
	{
		scheduler_try_grant(data);
		pthread_cond_broadcast(&data->alloc_cond);
	}
}

void	dongle_release_pair(t_data *data, int a, int b)
{
	long	free_at;

	pthread_mutex_lock(&data->alloc_lock);
	free_at = get_time_ms() + data->cfg.dongle_cooldown;
	data->dongles[a].free_at = free_at;
	if (b >= 0)
		data->dongles[b].free_at = free_at;
	scheduler_try_grant(data);
	pthread_cond_broadcast(&data->alloc_cond);
	pthread_mutex_unlock(&data->alloc_lock);
}

int	dongle_acquire_pair(int a, int b, t_coder *coder, t_data *data)
{
	t_wait	*wait;
	int		granted;

	wait = &data->waits[coder->id - 1];
	pthread_mutex_lock(&data->alloc_lock);
	wait->state = WAITING;
	wait->dongle_a = a;
	wait->dongle_b = b;
	wait->seq = ++data->seq_counter;
	wait->deadline = get_deadline(coder);
	scheduler_try_grant(data);
	pthread_cond_broadcast(&data->alloc_cond);
	while (wait->state == WAITING && !is_stopped(data))
		wait_for_grant(data, wait);
	granted = (wait->state == GRANTED);
	wait->state = 0;
	pthread_mutex_unlock(&data->alloc_lock);
	return (granted);
}
