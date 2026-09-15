/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kel-hadd <kel-hadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 00:21:11 by kel-hadd          #+#    #+#             */
/*   Updated: 2026/09/15 01:21:00 by kel-hadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_msg(t_data *data, int coder_id, const char *msg)
{
	long	now;

	pthread_mutex_lock(&data->log_lock);
	if (!is_stopped(data))
	{
		now = get_time_ms() - data->start_time;
		printf("%ld %d %s\n", now, coder_id, msg);
	}
	pthread_mutex_unlock(&data->log_lock);
}

void	log_burnout(t_data *data, int coder_id, long now)
{
	set_stop(data);
	pthread_mutex_lock(&data->log_lock);
	printf("%ld %d burned out\n", now - data->start_time, coder_id);
	pthread_mutex_unlock(&data->log_lock);
}
