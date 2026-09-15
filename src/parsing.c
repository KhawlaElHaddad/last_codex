/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kel-hadd <kel-hadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 00:21:21 by kel-hadd          #+#    #+#             */
/*   Updated: 2026/09/15 01:21:00 by kel-hadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	print_usage(void)
{
	fprintf(stderr, "Usage: ./codexion number_of_coders time_to_burnout ");
	fprintf(stderr, "time_to_compile time_to_debug time_to_refactor ");
	fprintf(stderr, "number_of_compiles_required dongle_cooldown scheduler\n");
	return (0);
}

static int	parse_number(const char *s, long *out, long min)
{
	long	n;
	int		digit;

	n = 0;
	if (!*s)
		return (0);
	while (*s >= '0' && *s <= '9')
	{
		digit = *s - '0';
		if (n > (INT_MAX - digit) / 10)
			return (0);
		n = (n * 10) + digit;
		s++;
	}
	if (*s || n < min)
		return (0);
	*out = n;
	return (1);
}

static void	fill_config(t_config *cfg, long *v)
{
	cfg->num_coders = (int)v[0];
	cfg->time_to_burnout = v[1];
	cfg->time_to_compile = v[2];
	cfg->time_to_debug = v[3];
	cfg->time_to_refactor = v[4];
	cfg->compiles_required = (int)v[5];
	cfg->dongle_cooldown = v[6];
}

int	parse_args(int argc, char **argv, t_config *cfg)
{
	long	v[7];
	int		i;

	if (argc != 9)
		return (print_usage());
	i = 0;
	while (i < 7)
	{
		if (!parse_number(argv[i + 1], &v[i], (i == 0)))
			return (print_usage());
		i++;
	}
	if (strcmp(argv[8], "fifo") == 0)
		cfg->scheduler = CODEX_FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		cfg->scheduler = CODEX_EDF;
	else
		return (print_usage());
	fill_config(cfg, v);
	return (1);
}
