/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kel-hadd <kel-hadd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 00:21:03 by kel-hadd          #+#    #+#             */
/*   Updated: 2026/09/15 01:21:00 by kel-hadd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	higher(t_wait *a, t_wait *b, int scheduler)
{
	if (scheduler == CODEX_EDF && a->deadline != b->deadline)
		return (a->deadline < b->deadline);
	return (a->seq < b->seq);
}

static void	sift_up(t_heap *heap, int i, int scheduler)
{
	int		parent;
	t_wait	*tmp;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!higher(heap->items[i], heap->items[parent], scheduler))
			break ;
		tmp = heap->items[i];
		heap->items[i] = heap->items[parent];
		heap->items[parent] = tmp;
		i = parent;
	}
}

static void	sift_down(t_heap *heap, int i, int scheduler)
{
	int		left;
	int		best;
	t_wait	*tmp;

	while (1)
	{
		left = (2 * i) + 1;
		best = i;
		if (left < heap->size
			&& higher(heap->items[left], heap->items[best], scheduler))
			best = left;
		if (left + 1 < heap->size
			&& higher(heap->items[left + 1], heap->items[best], scheduler))
			best = left + 1;
		if (best == i)
			break ;
		tmp = heap->items[i];
		heap->items[i] = heap->items[best];
		heap->items[best] = tmp;
		i = best;
	}
}

void	heap_push(t_heap *heap, t_wait *wait, int scheduler)
{
	heap->items[heap->size] = wait;
	sift_up(heap, heap->size, scheduler);
	heap->size++;
}

t_wait	*heap_pop(t_heap *heap, int scheduler)
{
	t_wait	*top;

	top = heap->items[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		sift_down(heap, 0, scheduler);
	}
	return (top);
}
