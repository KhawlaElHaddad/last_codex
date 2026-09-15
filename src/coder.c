/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kel-hadd <kel-hadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 00:20:32 by kel-hadd          #+#    #+#             */
/*   Updated: 2026/09/15 01:21:00 by kel-hadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	acquire_two(t_coder *coder, t_data *data)
{
	int	left;
	int	right;

	left = coder->id - 1;
	right = coder->id % data->cfg.num_coders;
	if (!dongle_acquire_pair(left, right, coder, data))
		return (0);
	log_msg(data, coder->id, "has taken a dongle");
	log_msg(data, coder->id, "has taken a dongle");
	return (1);
}

static void	release_two(t_coder *coder, t_data *data)
{
	dongle_release_pair(data, coder->id - 1,
		coder->id % data->cfg.num_coders);
}

static int	do_cycle(t_coder *coder, t_data *data)
{
	if (!acquire_two(coder, data))
		return (0);
	set_deadline(coder, get_time_ms() + data->cfg.time_to_burnout);
	log_msg(data, coder->id, "is compiling");
	precise_sleep(data->cfg.time_to_compile, data);
	release_two(coder, data);
	if (is_stopped(data))
		return (0);
	inc_compiles(coder);
	log_msg(data, coder->id, "is debugging");
	precise_sleep(data->cfg.time_to_debug, data);
	if (is_stopped(data))
		return (0);
	log_msg(data, coder->id, "is refactoring");
	precise_sleep(data->cfg.time_to_refactor, data);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_data	*data;
	int		taken;

	coder = (t_coder *)arg;
	data = coder->data;
	if (data->cfg.num_coders == 1)
	{
		taken = dongle_acquire_pair(0, -1, coder, data);
		if (taken)
			log_msg(data, coder->id, "has taken a dongle");
		while (!is_stopped(data))
			usleep(1000);
		if (taken)
			dongle_release_pair(data, 0, -1);
		return (NULL);
	}
	while (!is_stopped(data) && do_cycle(coder, data))
		;
	return (NULL);
}
