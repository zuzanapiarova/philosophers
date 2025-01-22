/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 10:20:07 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/22 17:01:08 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// this thread can communicate with the others
// only waits to receive "signal" from stop_sem
// stop_sem is a semaphore signalling that the simulation is over
// stop_sem is triggered by death or by all being full
// then it sets stop_simulation for this process to true and returns
	//log_msg(philo, RECEIVED); // remove later 
	//log_msg(philo, CHANGE);
void	*stop_routine(void	*arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	sem_wait(philo->shared->stop_sem);
	sem_wait(philo->mutex_local_sem);
	philo->stop_simulation = true;
	log_msg(philo, CHANGE);
	sem_post(philo->mutex_local_sem);
	return (NULL);
}

// runs for each philo constantly until he dies
// if he dies, prints DEATH msg and sends signal to all processes (incl. this one) which are caught by stop_routine thread
// can finish if philo dies or if it receives stop_simulation == true
/* void	*death_routine(void *arg)
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
} */

void	*death_routine(void *arg)
{
	t_philo	*philo;
	bool	dead;
	bool	stopped;

	philo = (t_philo *)arg;
	while (1)
	{
		// Check stop_simulation once per loop iteration
		sem_wait(philo->mutex_local_sem);
		stopped = philo->stop_simulation;
		dead = get_time_in_micros() - philo->last_eaten > (philo->die * 1000);
		if (dead)
			philo->stop_simulation = true;
		sem_post(philo->mutex_local_sem);
		if (dead && !stopped)
		{
			// Signal other processes about simulation stop
			for (int i = 0; i < philo->total; i++)
				sem_post(philo->shared->stop_sem);
			log_msg(philo, DEATH);
		}
		if (stopped || dead)
		{
			// Signal main process to continue (fullness_sem)
			for (int i = 0; i < philo->total; i++)
				sem_post(philo->shared->fullness_sem);
			return (NULL);
		}
		usleep(100);
	}
}

// fullness checker waits n times until all philos are full
// when fullness semaphore waits n times, means sim has stopped
// so the thread posts n times to stop semaphore and returns
void	*full_routine(void *arg)
{
	t_philo *philo;
	int			i;

	i = 0;
	philo = (t_philo *)arg;
	while (++i < philo->total)
		sem_wait(philo->shared->fullness_sem);
	i = -1;
	while (++i < philo->total) 
		sem_post(philo->shared->stop_sem);
	return (NULL);
}
