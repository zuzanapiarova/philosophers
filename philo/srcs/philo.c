/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 08:40:25 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/14 10:18:36 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int	init_philo(t_philo *p, int i, char **argv, t_shared *shared)
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

int	start_simulation(int argc, char **argv, int total)
{
	int			i;
	t_philo		*philos;
	pthread_t	monitor;
	t_shared	shared;
	(void)argc;

	i = -1;
	shared.stop_simulation = false;
	// init forks and other mutexes
	shared.forks = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * total);
	if (!shared.forks)
		return (ERROR);
	while (++i < total)
		pthread_mutex_init(&shared.forks[i], NULL);
	pthread_mutex_init(&shared.msg_lock, NULL);
	pthread_mutex_init(&shared.stop_lock, NULL);
	// start philos threads
	i = -1;
	philos = (t_philo *)malloc(sizeof(t_philo) * total);
	if (!philos)
	{
		while (++i < total)
			pthread_mutex_destroy(&shared.forks[i]);
		pthread_mutex_destroy(&shared.msg_lock);
		pthread_mutex_destroy(&shared.stop_lock);
		free(shared.forks);
		return (ERROR);
	}
	while (++i  < total)
	{
		if (init_philo(&philos[i], i, argv, &shared) != SUCCESS)
		{
			// clean the current unsuccessful philosophers lock or thread - if here is error, one of them went wrong
			i--;
			while (i > 0)
			{
				pthread_mutex_destroy(&philos[i].lock);
				pthread_join(philos[i].thread, NULL);
				i--;
			}
			i = -1;
			while (++i < total)
				pthread_mutex_destroy(&shared.forks[i]);
			free(shared.forks);
			pthread_mutex_destroy(&shared.msg_lock);
			pthread_mutex_destroy(&shared.stop_lock);
			free(philos);
			return (ERROR);
		}
	}
	// start monitoring thread
	i = -1;
	if (pthread_create(&monitor, NULL, &monitoring, (void *)philos) != 0)
	{
		while (++i < total)
		{
			pthread_join(philos[i].thread, NULL);
			pthread_mutex_destroy(&shared.forks[i]);
			pthread_mutex_destroy(&philos[i].lock);
		}
		free(shared.forks);
		pthread_mutex_destroy(&shared.msg_lock);
		pthread_mutex_destroy(&shared.stop_lock);
		free(philos);
		return (ERROR);
	}
	// properly end program and free resources
	while (++i < total)
	{
		pthread_mutex_destroy(&philos[i].lock);
		pthread_join(philos[i].thread, NULL);
		pthread_mutex_destroy(&shared.forks[i]);
	}
	// now that all threads are joined we now all of them finished eating and we can set stop_simulation t true so monitoring thread can exit
	// printf("set stop simulation to true\n");
	// *(philos[0].stop_simulation) = true;
	shared.stop_simulation = true;
	pthread_mutex_lock(&shared.stop_lock); // maybe not needed ?
	pthread_mutex_unlock(&shared.stop_lock); // maybe not needed ?
	free(shared.forks);
	//printf("joined philo threads and destroyed forks\n");
	pthread_mutex_destroy(&shared.msg_lock);
	pthread_mutex_destroy(&shared.stop_lock);
	//printf("destroyed shared mutexes\n");
	pthread_join(monitor, NULL);
	//printf("joined monitor thread\n");
	free(philos);
	return (SUCCESS);
}

int main(int argc, char **argv)
{
	if (handle_error_input(argc, argv) == ERROR)
		return (ERROR);
	if (start_simulation(argc, argv, ft_atou(argv[1])) == ERROR)
		return (ERROR);
	return (0);
}
