/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 16:41:14 by zuzanapiaro       #+#    #+#             */
/*   Updated: 2025/01/03 20:24:26 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

void *monitoring(void *arg)
{
	int		i;
    t_philo *philos;

	philos = (t_philo *)arg;
    while (1)
	{
		i = 0;
        while (i < philos[i].total)
		{
            pthread_mutex_lock(&philos[i].lock); // Lock philosopher data
			if ((int)philos[i].times_eaten == philos[i].times_to_eat)
				return (NULL);
            long current_time = get_time_in_ms();
            if (current_time - philos[i].last_eaten > philos[i].die)
			{
                log_msg(&philos[i], DEATH);
				pthread_mutex_unlock(&philos[i].lock);
				return (NULL); // Exit the monitor thread
            }
            pthread_mutex_unlock(&philos[i].lock);
			if (philos[i].die < 8000)
				usleep(philos[i].die / 2);
			else
				usleep(8000); // Sleep for a short period to avoid excessive CPU usage
			i++;
        }
    }
}

int p_eat(t_philo *philo)
{
	philo->last_eaten = get_time_in_ms();
	if ((int)philo->times_eaten > -1)
		philo->times_eaten++;
	log_msg(philo, EATS);
	usleep(philo->eat * 1000);
	return (0);
}

int p_sleep(t_philo *philo)
{
	log_msg(philo, SLEEPS);
	usleep(philo->sleep * 1000);
	return (0);
}

// every philo takes fork on its id and the one before it
// last one takes the fork on id - 1 and on forks[0]
// for even philos, first lock R, then l fork
// for odd philos, lock first L, then R fork
// for last philo, 
int take_forks(t_philo *philo)
{
	if (philo->id % 2 == 0)
	{
		if (philo->id == philo->total)
			pthread_mutex_lock(&philo->forks[0]);
		else
			pthread_mutex_lock(&philo->forks[philo->id]);
		log_msg(philo, FORK_R);
		pthread_mutex_lock(&philo->forks[philo->id - 1]);
		log_msg(philo, FORK_L);
	}
	else
	{
		pthread_mutex_lock(&philo->forks[philo->id - 1]);
		log_msg(philo, FORK_L);
		if (philo->id == philo->total)
			pthread_mutex_lock(&philo->forks[0]);
		else
			pthread_mutex_lock(&philo->forks[philo->id]);
		log_msg(philo, FORK_R);
	}
	return (0);
}

int leave_forks(t_philo *philo)
{
	// unlock left fork
	pthread_mutex_unlock(&philo->forks[philo->id - 1]);
	// unlock right fork
	if (philo->id == philo->total)
			pthread_mutex_unlock(&philo->forks[0]);
		else
			pthread_mutex_unlock(&philo->forks[philo->id]);
	return (0);
}
