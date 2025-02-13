/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 11:14:19 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/23 13:04:51 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// cleans up resources as threads and mutexes at the end of the program
int	cleanup(t_philo *philos, t_shared *shared, pthread_t *monitor, int total)
{
	int	i;

	i = -1;
	while (++i < total)
		pthread_join(philos[i].thread, NULL);
	pthread_join(*monitor, NULL);
	i = -1;
	while (++i < total)
	{
		pthread_mutex_destroy(&philos[i].lock);
		pthread_mutex_destroy(&shared->forks[i]);
	}
	free(shared->forks);
	pthread_mutex_destroy(&shared->msg_lock);
	pthread_mutex_destroy(&shared->stop_lock);
	free(philos);
	return (SUCCESS);
}
