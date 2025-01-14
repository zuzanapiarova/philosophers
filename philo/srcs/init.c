/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 11:25:49 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/14 13:04:41 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// initializes the shared resources into a structure 
// shared resources are: stop_simulation, forks, msg_lock, stop_lock
int	init_shared_resources(t_shared *shared, int total)
{
	int	i;

	i = -1;
	shared->stop_simulation = false;
	pthread_mutex_init(&shared->msg_lock, NULL);
	pthread_mutex_init(&shared->stop_lock, NULL);
	shared->forks = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * total);
	if (!shared->forks)
		return (write(2, "Memory allocation problem. Exiting.\n", 36), ERROR);
	while (++i < total)
		pthread_mutex_init(&shared->forks[i], NULL);
	return (SUCCESS);
}

// initialzes each philo separately from argv and shared resources
// each philo is a separate entity and cannot see the data of other philos
// appart from shared resources: stop_simulation, forks, msg_lock, stop_lock
int	init_philo_data(t_philo *p, int i, char **argv, t_shared *shared)
{
	p->id = i + 1;
	p->total = ft_atou(argv[1]);
	p->die = ft_atou(argv[2]);
	p->eat = ft_atou(argv[3]);
	p->sleep = ft_atou(argv[4]);
	if (argv[5])
		p->times_to_eat = ft_atou(argv[5]);
	else
		p->times_to_eat = -1;
	p->forks = shared->forks;
	p->times_eaten = 0;
	p->finished = false;
	if (pthread_mutex_init(&p->lock, NULL) != 0)
		return (ERROR);
	p->msg_lock = &shared->msg_lock;
	p->stop_lock = &shared->stop_lock;
	p->stop_simulation = &shared->stop_simulation;
	p->last_eaten = get_time_in_micros();
	if (pthread_create(&p->thread, NULL, &routine, (void *)p) != 0)
		return (ERROR);
	return (SUCCESS);
}

// initializes philo array with philo structures 
int	allocate_philo_array(t_philo **philos, t_shared *shared, int total)
{
	int	i;

	i = -1;
	*philos = (t_philo *)malloc(sizeof(t_philo) * total);
	if (!philos)
	{
		while (++i < total)
			pthread_mutex_destroy(&shared->forks[i]);
		pthread_mutex_destroy(&shared->msg_lock);
		pthread_mutex_destroy(&shared->stop_lock);
		free(shared->forks);
		return (write(2, "Memory allocation problem. Exiting.\n", 36), ERROR);
	}
	return (SUCCESS);
}

int	fill_philo_array(t_philo *philos, t_shared *shared, char **argv, int total)
{
	int	i;

	i = -1;
	while (++i < total)
	{
		if (init_philo_data(&philos[i], i, argv, shared) != SUCCESS)
		{
			i--;
			while (i > 0)
			{
				pthread_mutex_destroy(&philos[i].lock);
				pthread_join(philos[i].thread, NULL);
				i--;
			}
			i = -1;
			while (++i < total)
				pthread_mutex_destroy(&shared->forks[i]);
			free(shared->forks);
			pthread_mutex_destroy(&shared->msg_lock);
			pthread_mutex_destroy(&shared->stop_lock);
			free(philos);
			return (ERROR);
		}
	}
	return (SUCCESS);
}

int	init_monitor(pthread_t *monitor, t_philo *philos, t_shared *shared, int t)
{
	int	i;

	i = -1;
	if (pthread_create(monitor, NULL, &monitoring, (void *)philos) != 0)
	{
		while (++i < t)
		{
			pthread_join(philos[i].thread, NULL);
			pthread_mutex_destroy(&shared->forks[i]);
			pthread_mutex_destroy(&philos[i].lock);
		}
		free(shared->forks);
		pthread_mutex_destroy(&shared->msg_lock);
		pthread_mutex_destroy(&shared->stop_lock);
		free(philos);
		return (ERROR);
	}
	return (SUCCESS);
}
