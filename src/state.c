/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   state.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kel-hadd <kel-hadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 00:21:25 by kel-hadd          #+#    #+#             */
/*   Updated: 2026/09/15 01:21:00 by kel-hadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_stopped(t_data *data)
{
	int	value;

	pthread_mutex_lock(&data->stop_lock);
	value = data->stop;
	pthread_mutex_unlock(&data->stop_lock);
	return (value);
}

void	set_stop(t_data *data)
{
	pthread_mutex_lock(&data->stop_lock);
	data->stop = 1;
	pthread_mutex_unlock(&data->stop_lock);
}

long	get_deadline(t_coder *coder)
{
	long	value;

	pthread_mutex_lock(&coder->lock);
	value = coder->deadline;
	pthread_mutex_unlock(&coder->lock);
	return (value);
}

void	set_deadline(t_coder *coder, long value)
{
	pthread_mutex_lock(&coder->lock);
	coder->deadline = value;
	pthread_mutex_unlock(&coder->lock);
}

void	inc_compiles(t_coder *coder)
{
	pthread_mutex_lock(&coder->lock);
	coder->compiles_done++;
	pthread_mutex_unlock(&coder->lock);
}
