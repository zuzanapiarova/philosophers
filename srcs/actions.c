/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 16:41:14 by zuzanapiaro       #+#    #+#             */
/*   Updated: 2025/01/04 21:01:09 by zuzanapiaro      ###   ########.fr       */
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
			// this below has to be changed because it exits as soon as first finished eating needed times, but the rest still may need to eat and may die in the meantime, but we exit out of this
			// if ((int)philos[i].times_eaten == philos[i].times_to_eat)
			// 	return (NULL);
			// TODO: maybe instead add that if stop_simulation  == true we return, and change the stop_simulation to true elswhere in code, eg when we reach times_to_eat with all of them ? 
			// --------------------------------------------------------
			if ((get_time_in_ms() - philos[i].last_eaten) > philos[i].die)
			{
				log_msg(&philos[i], DEATH);
				*(philos[i].stop_simulation) = true;
				pthread_mutex_unlock(&philos[i].lock);
				return (NULL); // Exit the monitor thread
			}
			pthread_mutex_unlock(&philos[i].lock);
			i++;
		}
		usleep(5000);
		// if (philos[i].die < 8000)
		// 	usleep(philos[i].die / 2);
		// else
		// 	usleep(8000); // Sleep for a short period to avoid excessive CPU usage
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

int p_think(t_philo *philo)
{
	log_msg(philo, THINKS);
	//usleep(philo->sleep * 1000);
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
