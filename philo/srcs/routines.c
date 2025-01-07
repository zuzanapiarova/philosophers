/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routines.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 10:00:49 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/07 21:13:00 by zuzanapiaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

void    *routine(void *arg)
{
    t_philo *philo;

    philo = (t_philo *)arg;
    if (philo->id % 2 == 0)
        usleep(1);
    while (1)
    {
		if ((int)philo->times_eaten >= philo->times_to_eat && philo->times_to_eat != -1) // so simulation does not start if times_to_eat is 0
		{
			philo->stopped = true;
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
        //pthread_mutex_lock(&philo->lock);
		// code and locks below maybe make the code not as efficient - there is at least 2ms difference betwen stopping eating and sleeping now !
        if (philo->times_to_eat != -1 && (int)philo->times_eaten >= philo->times_to_eat)
        {
            philo->stopped = true;
            log_msg(philo, FINISH);
            //pthread_mutex_unlock(&philo->lock);
            break ;
        }
       // pthread_mutex_unlock(&philo->lock);
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

    philos = (t_philo *)arg;
    total = philos[0].total;
    while (1)
    {
        i = 0;
        while (i < total)
        {
            pthread_mutex_lock(&philos[i].lock);
            if ((get_time_in_micros() - philos[i].last_eaten > (philos[i].die * 1000)) || *(philos[i].stop_simulation) == true)
            {
                if (!philos[i].stopped)
                    log_msg(&philos[i], DEATH);
                philos[i].stopped = true;
                *(philos[i].stop_simulation) = true;
                pthread_mutex_unlock(&philos[i].lock);
                return (NULL);
            }
            pthread_mutex_unlock(&philos[i].lock);
            i++;
        }
        usleep(5000);
    }
}
