/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 10:20:07 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/22 19:01:34 by zpiarova         ###   ########.fr       */
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
	t_philo	*philo;

	philo = (t_philo *)arg;
	sem_wait(philo->shared->stop_sem);
	sem_wait(philo->mutex_local_sem);
	philo->stop_simulation = true;
	sem_post(philo->mutex_local_sem);
	return (NULL);
}

void	*death_routine(void *arg)
{
	t_philo	*philo;
	bool	dead;
	bool	stopped;
	int		i;

	philo = (t_philo *)arg;
	while (1)
	{
		sem_wait(philo->mutex_local_sem);
		stopped = philo->stop_simulation;
		dead = get_time_in_micros() - philo->last_eaten > (philo->die * 1000);
		if (dead)
			philo->stop_simulation = true;
		sem_post(philo->mutex_local_sem);
		if (dead && !stopped)
		{
			i = -1;
			while (++i < philo->total)
				sem_post(philo->shared->stop_sem);
			log_msg(philo, DEATH);
		}
		if (stopped || dead)
		{
			i = -1;
			while (++i < philo->total)
				sem_post(philo->shared->fullness_sem);
			return (NULL);
		}
		usleep(1000);
	}
}

int	run_full_checker_thread(t_philo *philo)
{
	pthread_t	full_checker;
	
	if (pthread_create(&full_checker, NULL, &full_routine, philo) == ERROR)
		return (write(2, "Error creating full_check thread.\n", 34), ERROR);
	pthread_join(full_checker, NULL);
	return (SUCCESS);
}
// fullness checker waits n times until all philos are full
// when fullness semaphore waits n times, means sim has stopped
// so the thread posts n times to stop semaphore and returns
void	*full_routine(void *arg)
{
	t_philo *philo;
	int		i;

	philo = (t_philo *)arg;
	i = -1;
	while (++i < philo->total)
		sem_wait(philo->shared->fullness_sem);
	i = -1;
	while (++i < philo->total) 
		sem_post(philo->shared->stop_sem);
	return (NULL);
}
