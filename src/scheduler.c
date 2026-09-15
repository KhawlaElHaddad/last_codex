/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kel-hadd <kel-hadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 00:21:22 by kel-hadd          #+#    #+#             */
/*   Updated: 2026/09/15 01:21:00 by kel-hadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	fill_heap(t_data *data)
{
	int	i;

	data->heap.size = 0;
	i = 0;
	while (i < data->cfg.num_coders)
	{
		if (data->waits[i].state == WAITING)
			heap_push(&data->heap, &data->waits[i], data->cfg.scheduler);
		i++;
	}
}

static int	can_grant(t_data *data, t_wait *wait)
{
	long	now;

	now = get_time_ms();
	if (data->dongles[wait->dongle_a].free_at > now)
		return (0);
	if (wait->dongle_b >= 0
		&& data->dongles[wait->dongle_b].free_at > now)
		return (0);
	return (1);
}

static void	grant(t_data *data, t_wait *wait)
{
	data->dongles[wait->dongle_a].free_at = LONG_MAX;
	if (wait->dongle_b >= 0)
		data->dongles[wait->dongle_b].free_at = LONG_MAX;
	wait->state = GRANTED;
}

void	scheduler_try_grant(t_data *data)
{
	t_wait	*wait;

	fill_heap(data);
	while (data->heap.size > 0)
	{
		wait = heap_pop(&data->heap, data->cfg.scheduler);
		if (wait->state == WAITING && can_grant(data, wait))
			grant(data, wait);
	}
}
