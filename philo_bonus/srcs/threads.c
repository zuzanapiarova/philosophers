/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 10:20:07 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/21 11:34:30 by zpiarova         ###   ########.fr       */
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
	*(resources->philo->shared->stop_simulation) = true;
	printf("stop sim in %d: %d\n", resources->philo->id, *(resources->philo->shared->stop_simulation));
	// pthread_join(resources->philo->death_checker, NULL);
	// /clean resources - close semaphores, join threads
	// printf("joined death thread id %d\n", resources->philo->id);
	// close_semaphores(resources->shared);
	// free(resources->philos);
	// free(resources->pids);
	//exit(SUCCESS);
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
void	*death_routine(void *arg)
{
	t_philo	*philo;
	int		i;

	philo = (t_philo *)arg;
	while (1)
	{
		// pthread_mutex_lock(&philo->lock);
		if (get_time_in_micros() - philo->last_eaten > (philo->die * 1000) || *(philo->shared->stop_simulation))
		{
			if (*(philo->shared->stop_simulation) == false) // only log death if the time surpassed tiem to die, not when we received signal to change stop_sim to false
				log_msg(philo, DEATH);
            // & post n times to stop_semaphore so all processes get the memo that simulation is over
            i = -1;
            while (++i < philo->total)
                sem_post(philo->shared->stop_sem);
			log_msg(philo, LAST);
            // TODO: post to monitoring_semaphore so the main process can continue
			// i = 0;
			// while (i < philo->total)
			// {
			// 	sem_post(philo->shared->monitoring_sem);
			// 	i++;
			// }
			// pthread_mutex_unlock(&philo->lock);
			return (NULL);
		}
		// pthread_mutex_unlock(&philo->lock);
		usleep(100);
	}
}
