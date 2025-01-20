/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 10:20:07 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/20 13:43:10 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// this thread can communicate with the others 
// only waits to receive "signal" from stop_sem
// stop_sem is a semaaphore signalling that the simulation is over
// then it sets stop_simulation to true in THIS process
// further functions and threads will work with this variable
void	*stop_routine(void	*arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	printf("started stop_checker thread\n");
	sem_wait(philo->shared->stop_sem);
    log_msg(philo, STOP);
	// pthread_mutex_lock(&philo->lock);
	*(philo->shared->stop_simulation) = true;
	// pthread_mutex_unlock(&philo->lock);
	return (NULL);
}

// this thread can communicate with the others 
// only waits to receive "signal" from stop_sem
// stop_sem is a semaaphore signalling that the simulation is over
// then it sets stop_simulation to true in THIS process
// further functions and threads will work with this variable
void	*full_routine(void	*arg)
{
	t_philo	*philo;
    int     i;

	philo = (t_philo *)arg;
	printf("started stop_checker thread\n");
	i = 0;
	while (i < philo->total)
	{
		sem_wait(philo->shared->full_sem);
		printf("waited for p%d\n", i + 1);
		i++;
	}
	// pthread_mutex_lock(&philo->lock);
	*(philo->shared->stop_simulation) = true;
	// pthread_mutex_unlock(&philo->lock);
	return (NULL);
}

// runs constantly until philo dies
// if he dies, prints DEATH msg and exits 
// if stop_simulation is true, exits
void	*death_routine(void *arg)
{
	t_philo	*philo;
    int     i;

	philo = (t_philo *)arg;
	printf("started death_checker thread\n");
	while (1)
	{
		// pthread_mutex_lock(&philo->lock);
		if (get_time_in_micros() - philo->last_eaten 
			> (philo->die * 1000) || *(philo->shared->stop_simulation))
		{
            // log death msg if we had death in this philo process
			if (*(philo->shared->stop_simulation) != true) // only print death if the first condition is true (=philo died), stop_sim from this thread is not true
				log_msg(philo, DEATH);
            *(philo->shared->stop_simulation) = true;
            // post to stop_simulation semaphore so all threads can stop
            i = 0;
            while (i < philo[0].total) // & -------------------------------- post n times to semaphore so all processes get the memo that simulation is over ------------------------------------------
            {
                sem_post(philo[0].shared->stop_sem);
                i++;
            }
            // post to monitoring_semaphore so the main process can continue
			i = 0;
			while (i < philo->total)
			{
				sem_post(philo->shared->monitoring_sem);
				i++;
			}
			// pthread_mutex_unlock(&philo->lock);
			return (NULL);
		}
		// pthread_mutex_unlock(&philo->lock);
		usleep(1000);
	}
}
