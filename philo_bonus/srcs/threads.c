/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 10:20:07 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/21 21:10:09 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// this thread can communicate with the others
// only waits to receive "signal" from stop_sem
// stop_sem is a semaphore signalling that the simulation is over
// stop_sem is triggered by death or by all being full
// then it sets stop_simulation for this process to true and returns
void	*stop_routine(void	*arg)
{
	t_resources	*resources;

	resources = (t_resources *)arg;
	sem_wait(resources->shared->stop_sem);
	log_msg(resources->philo, RECEIVED);
	sem_wait(resources->philo->mutex_local_sem);
	resources->philo->stop_simulation = true;
	sem_post(resources->philo->mutex_local_sem);
	return (NULL);
}

// runs for each philo constantly until he dies
// if he dies, prints DEATH msg and sends signal to all processes (incl. this one) which are caught by stop_routine thread
// can finish if philo dies or if it receives stop_simulation == true
void	*death_routine(void *arg)
{
	t_philo	*philo;
	int		i;
	bool	dead = false;
	bool	stopped;

	philo = (t_philo *)arg;
	while (1)
	{
		sem_wait(philo->mutex_local_sem);
		stopped = philo->stop_simulation;
		sem_post(philo->mutex_local_sem);
		if (get_time_in_micros() - philo->last_eaten > (philo->die * 1000) && stopped == false)
		{
			sem_post(philo->shared->fork_sem);
			sem_wait(philo->mutex_local_sem);
			philo->stop_simulation = true;
			sem_post(philo->mutex_local_sem);
			dead = true;
			i = -1;
			while (++i < philo->total)
				sem_post(philo->shared->stop_sem);
			log_msg(philo, DEATH);
		}
		sem_wait(philo->mutex_local_sem);
		stopped = philo->stop_simulation;
		sem_post(philo->mutex_local_sem);
		if (stopped)
		{
            // // post to monitoring_semaphore so the main process can continue
			i = -1;
			while (++i < philo->total)
				sem_post(philo->shared->fullness_sem);
			return (NULL);
		}
		usleep(100);
	}
}

void	*fullness_checker_routine(void *arg)
{
	t_resources *resources;
	int			i;

	resources = (t_resources *)arg;
	// fullness checker waits n times until all philos are full
	while (++i < resources->philos[0].total)
		sem_wait(resources->philos[0].shared->fullness_sem);
	// when the monitoring semaphore finished waiting and reaches here, it means simulation should stop
	i = -1;
	while (++i < resources->philos[0].total) 
		sem_post(resources->philos[0].shared->stop_sem);
	return (NULL);
}
