/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kel-hadd <kel-hadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 00:21:15 by kel-hadd          #+#    #+#             */
/*   Updated: 2026/09/15 01:21:00 by kel-hadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	inspect_coder(t_coder *coder, int required, long now, int *done)
{
	int	burned;

	pthread_mutex_lock(&coder->lock);
	burned = (now >= coder->deadline);
	if (coder->compiles_done < required)
		*done = 0;
	pthread_mutex_unlock(&coder->lock);
	return (burned);
}

static int	check_state(t_data *data)
{
	int		i;
	int		all_done;
	long	now;

	i = 0;
	all_done = 1;
	now = get_time_ms();
	while (i < data->cfg.num_coders)
	{
		if (inspect_coder(&data->coders[i], data->cfg.compiles_required,
				now, &all_done))
		{
			log_burnout(data, data->coders[i].id, now);
			return (1);
		}
		i++;
	}
	if (!all_done)
		return (0);
	set_stop(data);
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_data	*data;

	data = (t_data *)arg;
	while (!is_stopped(data) && !check_state(data))
		usleep(MONITOR_TICK_US);
	pthread_mutex_lock(&data->alloc_lock);
	pthread_cond_broadcast(&data->alloc_cond);
	pthread_mutex_unlock(&data->alloc_lock);
	return (NULL);
}
