/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kel-hadd <kel-hadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 00:21:05 by kel-hadd          #+#    #+#             */
/*   Updated: 2026/09/15 01:21:00 by kel-hadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_sync(t_data *data)
{
	if (pthread_mutex_init(&data->stop_lock, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&data->log_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&data->stop_lock);
		return (0);
	}
	if (pthread_mutex_init(&data->alloc_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&data->log_lock);
		pthread_mutex_destroy(&data->stop_lock);
		return (0);
	}
	if (pthread_cond_init(&data->alloc_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&data->alloc_lock);
		pthread_mutex_destroy(&data->log_lock);
		pthread_mutex_destroy(&data->stop_lock);
		return (0);
	}
	return (1);
}

static int	init_coders(t_data *data)
{
	int	i;

	data->coders = malloc(sizeof(t_coder) * data->cfg.num_coders);
	if (!data->coders)
		return (0);
	i = 0;
	while (i < data->cfg.num_coders)
	{
		data->coders[i].id = i + 1;
		data->coders[i].compiles_done = 0;
		data->coders[i].deadline = data->start_time
			+ data->cfg.time_to_burnout;
		data->coders[i].data = data;
		if (pthread_mutex_init(&data->coders[i].lock, NULL) != 0)
		{
			while (i-- > 0)
				pthread_mutex_destroy(&data->coders[i].lock);
			return (free(data->coders), data->coders = NULL, 0);
		}
		i++;
	}
	return (1);
}

static int	init_arrays(t_data *data)
{
	int	n;

	n = data->cfg.num_coders;
	data->dongles = malloc(sizeof(t_dongle) * n);
	data->waits = malloc(sizeof(t_wait) * n);
	data->heap.items = malloc(sizeof(t_wait *) * n);
	if (!data->dongles || !data->waits || !data->heap.items)
	{
		free(data->dongles);
		free(data->waits);
		free(data->heap.items);
		data->dongles = NULL;
		data->waits = NULL;
		data->heap.items = NULL;
		return (0);
	}
	memset(data->dongles, 0, sizeof(t_dongle) * n);
	memset(data->waits, 0, sizeof(t_wait) * n);
	data->heap.size = 0;
	return (1);
}

int	init_data(t_data *data, t_config *cfg)
{
	memset(data, 0, sizeof(*data));
	data->cfg = *cfg;
	if (!init_sync(data))
		return (0);
	data->start_time = get_time_ms();
	if (!init_coders(data) || !init_arrays(data))
	{
		free_data(data);
		return (0);
	}
	return (1);
}

void	free_data(t_data *data)
{
	int	i;

	i = 0;
	while (data->coders && i < data->cfg.num_coders)
		pthread_mutex_destroy(&data->coders[i++].lock);
	free(data->coders);
	free(data->dongles);
	free(data->waits);
	free(data->heap.items);
	pthread_cond_destroy(&data->alloc_cond);
	pthread_mutex_destroy(&data->alloc_lock);
	pthread_mutex_destroy(&data->log_lock);
	pthread_mutex_destroy(&data->stop_lock);
}
