/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 10:20:07 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/20 22:04:18 by zuzanapiaro      ###   ########.fr       */
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
	t_philo	*philo;

	philo = (t_philo *)arg;
	sem_wait(philo->shared->stop_sem);
	log_msg(philo, RECEIVED);
	// TODO: clean resources - close semaphores, join threads
	//pthread_join(philo->death_checker, NULL);
	//pthread_join(philo->full_checker, NULL);
	pthread_detach(philo->death_checker);
	//pthread_detach(philo->death_checker);
	close_semaphores(philo->shared);
    log_msg(philo, STOP);
	exit(SUCCESS);
	// pthread_mutex_lock(&philo->lock);
	// /*(philo->shared->stop_simulation) = true;
	// pthread_mutex_unlock(&philo->lock);
	// /return (NULL);
}

// this thread can communicate with the others
// only waits to receive "signal" from stop_sem
// stop_sem is a semaphore signalling that the simulation is over
// then it sets stop_simulation to true in THIS process
// further functions and threads will work with this variable
void	*full_routine(void	*arg)
{
	t_philo	*philo;
    int     i;

	philo = (t_philo *)arg;
	i = -1;
	while (++i < philo->total)
		sem_wait(philo->shared->full_sem);
	sem_post(philo->shared->full_sem);
	return (NULL);
}

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
		if (get_time_in_micros() - philo->last_eaten > (philo->die * 1000)/*  || *(philo->shared->stop_simulation) */)
		{
			// post to forks so they can take the forks they are waiting for
			// i = -1;
			// while (++i < philo[0].total / 2)    // ?????????????????????
			// 	sem_post(philo[0].shared->fork_sem);
            // log death msg if we had death in this philo process
			//if (*(philo->shared->stop_simulation) != true) // only print death if the first condition is true (=philo died), stop_sim from this thread is not true
			log_msg(philo, DEATH);
            // /*(philo->shared->stop_simulation) = true;
            // & post n times to semaphore so all processes get the memo that simulation is over
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
			//return (NULL);
		}
		// pthread_mutex_unlock(&philo->lock);
		usleep(100);
	}
}
