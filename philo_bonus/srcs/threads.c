/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 10:20:07 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/21 20:06:56 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// this thread can communicate with the others
// only waits to receive "signal" from stop_sem
// stop_sem is a semaphore signalling that the simulation is over
// stop_sem is triggered by death or by all being full
// then it frees resources and exits
void	*stop_routine(void	*arg)
{
	t_resources	*resources;

	resources = (t_resources *)arg;
	sem_wait(resources->shared->stop_sem);
	log_msg(resources->philo, RECEIVED);
	sem_wait(resources->philo->mutex_local_sem);
	resources->philo->stop_simulation = true;
	sem_post(resources->philo->mutex_local_sem);
	
	// cleanup - semaphores, threads, mallocs
	// //pthread_detach(resources->philo->death_checker); // we detach this thread
	// pthread_join(resources->philo->death_checker, NULL); //  we wait for the death thread, which got teh memo to end throug stop_simulation variable 
	// close_semaphores(resources->shared);
	// sem_close(resources->philo->mutex_local_sem);
	// sem_unlink(resources->philo->mutex_sem_name);
	// free(resources->philo->mutex_sem_name);
	// free(resources->philos);
	// free(resources->pids);
	// exit(SUCCESS);
	return (NULL);
}

// this thread can communicate with the others
// only waits to receive "signal" from stop_sem
// stop_sem is a semaphore signalling that the simulation is over
// then it sets stop_simulation to true in THIS process
// further functions and threads will work with this variable
// void	*full_routine(void	*arg)
// {
// 	t_philo	*philo;
//     int     i;

// 	philo = (t_philo *)arg;
// 	i = -1;
// 	while (++i < philo->total)
// 		sem_wait(philo->shared->full_sem);
// 	sem_post(philo->shared->full_sem);
// 	return (NULL);
// }

// runs for each philo constantly until he dies
// if he dies, prints DEATH msg and sends signal to all processes (incl. this one) which are caught by stop_routine thread
// can finish if philo dies or if it receives stop_simulation == true
void	*death_routine(void *arg)
{
	t_philo	*philo;
	int		i;
	bool	dead = false;
	bool	stopped;

	// printf("started death_routine thread\n");
	philo = (t_philo *)arg;
	while (1)
	{
		sem_wait(philo->mutex_local_sem);
		if (get_time_in_micros() - philo->last_eaten > (philo->die * 1000))
		{
			philo->stop_simulation = true;
			sem_post(philo->shared->fork_sem);
			dead = true;
		}
		stopped = philo->stop_simulation;
		sem_post(philo->mutex_local_sem);
		if (dead || stopped)
		{
			if (dead)
			{
				log_msg(philo, DEATH);
				i = -1;
				while (++i < philo->total)
					sem_post(philo->shared->stop_sem);
			}
            // post to monitoring_semaphore so the main process can continue
			i = -1;
			while (++i < philo->total)
				sem_post(philo->shared->monitoring_sem);
			return (NULL);
		}
		usleep(100);
	}
}
