/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 08:40:25 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/07 15:57:55 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int init_philo(t_philo *philo, int i, char **argv, pthread_mutex_t **forks, pthread_mutex_t *msg_lock, pthread_mutex_t *stop_lock, bool *stop_simulation)
{
    philo->id = i + 1;
	philo->total = ft_atou(argv[1]);
	philo->die = ft_atou(argv[2]);
	philo->eat = ft_atou(argv[3]);
	philo->sleep = ft_atou(argv[4]);
	if (argv[5])
		philo->times_to_eat = ft_atou(argv[5]);
	else
		philo->times_to_eat = -1;
    philo->forks = *forks;
	philo->times_eaten = 0;
	philo->last_eaten = get_time_in_micros();
    philo->stopped = false;
    pthread_mutex_init(&philo->lock, NULL);
	philo->msg_lock = msg_lock;
	philo->stop_lock = stop_lock;
    philo->stop_simulation = stop_simulation;
	if (pthread_create(&philo->thread, NULL, &routine, (void *)philo) != 0)
		return (ERROR);
	return (SUCCESS);
}

int start_simulation(int argc, char **argv, int total)
{
    int             i;
    t_philo         *philos;
    pthread_t       monitor;
    // shared resources - will be passed by address to function
    bool            stop_simulation;
    pthread_mutex_t *forks;
    pthread_mutex_t msg_lock;
    pthread_mutex_t stop_lock;
    (void)argc;

    i = 0;
    stop_simulation = false;
    // init forks and other mutexes
    forks = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * total);
    if (!forks)
        return (ERROR);
    while (i < total)
    {
        pthread_mutex_init(&forks[i], NULL);
        i++;
    }
    pthread_mutex_init(&msg_lock, NULL);
    pthread_mutex_init(&stop_lock, NULL);
    // start philos threads
    i = 0;
    philos = (t_philo *)malloc(sizeof(t_philo) * total);
    if (!philos)
    {
        while (i < total)
        {
            pthread_mutex_destroy(&forks[i]);
            i++;
        }
        pthread_mutex_destroy(&msg_lock);
        pthread_mutex_destroy(&stop_lock);
        free(forks);
        return (ERROR);
    }
    while (i  < total)
    {
        if (init_philo(&philos[i], i, argv, &forks, &msg_lock, &stop_lock, &stop_simulation) == ERROR)
        {
            i--;
            while (i > 0)
            {
                pthread_mutex_destroy(&philos[i].lock);
                pthread_join(philos[i].thread, NULL);
                i--;
            }
            i = 0;
            while (i < total)
            {
                pthread_mutex_destroy(&forks[i]);
                i++;
            }
            free(forks);
            pthread_mutex_destroy(&msg_lock);
            pthread_mutex_destroy(&stop_lock);
            free(philos);
            return (ERROR);
        }
        i++;
    }
    // start monitoring thread
    i = 0;
    if (pthread_create(&monitor, NULL, &monitoring, (void *)philos) != 0)
    {
        while (i < total)
        {
            pthread_join(philos[i].thread, NULL);
            pthread_mutex_destroy(&forks[i]);
            pthread_mutex_destroy(&philos[i].lock);
            i++;
        }
        free(forks);
        pthread_mutex_destroy(&msg_lock);
        pthread_mutex_destroy(&stop_lock);
        free(philos);
        return (ERROR);        
    }
    // properly end program and free resources
    i = 0;
	while (i < total)
	{
		pthread_mutex_destroy(&philos[i].lock);
		pthread_join(philos[i].thread, NULL);
		pthread_mutex_destroy(&forks[i]);
		i++;
	}
    // now that all threads are joined we now all of them finished eating and we can set stop_simulation t true so monitoring thread can exit 
    printf("set stop siulaiton as true\n");
    *(philos[0].stop_simulation) = true;
    free(forks);
	//printf("joined philo threads and destroyed forks\n");
	pthread_mutex_destroy(&msg_lock);
    pthread_mutex_destroy(&stop_lock);
	//printf("destroyed shared mutexes\n");
	pthread_join(monitor, NULL);
	//printf("joined monitor thread\n");
	free(philos);
	return (0);
}


int main(int argc, char **argv)
{
	if (handle_error_input(argc, argv) == ERROR)
		return (ERROR);
	if (start_simulation(argc, argv, ft_atou(argv[1])) == ERROR)
		return (ERROR);
	return (0);
}