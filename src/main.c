/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kel-hadd <kel-hadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 00:21:13 by kel-hadd          #+#    #+#             */
/*   Updated: 2026/09/15 00:21:14 by kel-hadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	join_all(t_data *data, int monitor_ok, int created)
{
	int	i;

	if (monitor_ok)
		pthread_join(data->monitor, NULL);
	i = 0;
	while (i < created)
	{
		pthread_join(data->coders[i].thread, NULL);
		i = i + 1;
	}
}

int	run_simulation(t_data *data)
{
	int	created;
	int	monitor_ok;

	if (data->cfg.compiles_required == 0)
		return (1);
	monitor_ok = (pthread_create(&data->monitor, NULL, monitor_routine,
				data) == 0);
	created = 0;
	while (monitor_ok && created < data->cfg.num_coders)
	{
		if (pthread_create(&data->coders[created].thread, NULL, coder_routine,
				&data->coders[created]) != 0)
			break ;
		created++;
	}
	if (!monitor_ok || created < data->cfg.num_coders)
	{
		fprintf(stderr, "codexion: could not create all threads\n");
		set_stop(data);
	}
	join_all(data, monitor_ok, created);
	return (monitor_ok && created == data->cfg.num_coders);
}

int	main(int argc, char **argv)
{
	t_config	cfg;
	t_data		data;
	int			ok;

	if (!parse_args(argc, argv, &cfg))
		return (1);
	if (!init_data(&data, &cfg))
		return (1);
	ok = run_simulation(&data);
	free_data(&data);
	if (!ok)
		return (1);
	return (0);
}
