/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routines.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 10:00:49 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/13 15:39:50 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

void    *routine(void *arg)
{
    t_philo *philo;

    philo = (t_philo *)arg;
    if (philo->id % 2 == 0)
        usleep(10); // maybe wait for time_to_die - time_to_eat - time_to_sleep / 2 ms 
    while (1)
    {
		if (((int)philo->times_eaten >= philo->times_to_eat && philo->times_to_eat != -1)) // so simulation does not start if times_to_eat is 0
		{
			philo->finished = true;
			break ;
		}
        if (take_forks(philo) == ERROR)
            break ;
        if (p_eat(philo) == ERROR)
        {
            leave_forks(philo);
            break ;
        }
        if (leave_forks(philo) == ERROR)
            break ;
        if (philo->times_to_eat != -1 && (int)philo->times_eaten >= philo->times_to_eat)
        {
            philo->finished = true;
            log_msg(philo, FINISH);
            break ;
        }
        if (p_sleep(philo) == ERROR)
            break ;
        if (p_think(philo) == ERROR)
            break ;
    }
    //printf("finished loop in thread %d\n", philo->id);
    return (NULL);
}

void    *monitoring(void *arg)
{
    int     i;
    int     total;
    t_philo *philos;
	bool	stop;

    philos = (t_philo *)arg;
    total = philos[0].total;
    while (1)
    {
        i = -1;
        while (++i < total)
        {
            pthread_mutex_lock(philos[i].stop_lock); // was stop_lock before // TODO: maybe change the stop lock everywhere to just lock so we avoid using too many mutexes ?? 
            if (get_time_in_micros() - philos[i].last_eaten > (philos[i].die * 1000))
                *(philos[i].stop_simulation) = true;
			stop = *(philos[i].stop_simulation);
            if (stop) // monitori exits only if simulation stopped because of death
            {
                if (!philos[i].finished)
                    log_msg(&philos[i], DEATH);
                pthread_mutex_unlock(philos[i].stop_lock);// was stop_lock before
                return (NULL);
            }
            pthread_mutex_unlock(philos[i].stop_lock);// was stop_lock before
        }
        usleep(5000);
    }
}

			// pthread_mutex_lock(philos[i].stop_lock);
			// stop = *(philos[i].stop_simulation);
			// pthread_mutex_unlock(philos[i].stop_lock);
            // if ((get_time_in_micros() - philos[i].last_eaten > (philos[i].die * 1000)) || stop)
            // {
            //     if (!philos[i].finished)
            //         log_msg(&philos[i], DEATH);
            //     philos[i].finished = true;
			// 	pthread_mutex_lock(philos[i].stop_lock);
            //     *(philos[i].stop_simulation) = true;
			// 	pthread_mutex_unlock(philos[i].stop_lock);
            //     return (NULL);
            // }
