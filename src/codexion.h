/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kel-hadd <kel-hadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 00:21:39 by kel-hadd          #+#    #+#             */
/*   Updated: 2026/09/15 01:51:46 by kel-hadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <time.h>
# include <unistd.h>

# define CODEX_FIFO 0
# define CODEX_EDF 1
# define WAITING 1
# define GRANTED 2
# define MONITOR_TICK_US 1000

typedef struct s_data	t_data;

typedef struct s_wait
{
	int		state;
	int		dongle_a;
	int		dongle_b;
	long	seq;
	long	deadline;
}				t_wait;

typedef struct s_heap
{
	t_wait	**items;
	int		size;
}				t_heap;

typedef struct s_dongle
{
	long	free_at;
}				t_dongle;

typedef struct s_coder
{
	int				id;
	int				compiles_done;
	long			deadline;
	pthread_mutex_t	lock;
	pthread_t		thread;
	t_data			*data;
}						t_coder;

typedef struct s_config
{
	int			num_coders;
	long		time_to_burnout;
	long		time_to_compile;
	long		time_to_debug;
	long		time_to_refactor;
	int			compiles_required;
	long		dongle_cooldown;
	int			scheduler;
}					t_config;

struct s_data
{
	t_config			cfg;
	long				start_time;
	t_coder				*coders;
	t_dongle			*dongles;
	t_wait				*waits;
	t_heap				heap;
	pthread_mutex_t		alloc_lock;
	pthread_cond_t		alloc_cond;
	int					stop;
	pthread_mutex_t		stop_lock;
	pthread_mutex_t		log_lock;
	long				seq_counter;
	pthread_t			monitor;
};

long		get_time_ms(void);
void		precise_sleep(long ms, t_data *data);

int			parse_args(int argc, char **argv, t_config *cfg);

void		heap_push(t_heap *heap, t_wait *wait, int scheduler);
t_wait		*heap_pop(t_heap *heap, int scheduler);

void		dongle_release_pair(t_data *data, int a, int b);
int			dongle_acquire_pair(int a, int b, t_coder *coder, t_data *data);
void		scheduler_try_grant(t_data *data);

void		*coder_routine(void *arg);
void		*monitor_routine(void *arg);

void		log_msg(t_data *data, int coder_id, const char *msg);
void		log_burnout(t_data *data, int coder_id, long now);

int			is_stopped(t_data *data);
void		set_stop(t_data *data);
long		get_deadline(t_coder *coder);
void		set_deadline(t_coder *coder, long value);
void		inc_compiles(t_coder *coder);

int			init_data(t_data *data, t_config *cfg);
void		free_data(t_data *data);
int			run_simulation(t_data *data);

#endif
