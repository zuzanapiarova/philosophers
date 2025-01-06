/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routines.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 10:00:49 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/06 11:26:23 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

void    *routine(void *arg)
{
    t_philo *philo;

    philo = (t_philo *)arg;
    while (1)
    {
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
            *(philo->stop_simulation) = true;
            break ;
        }
        if (p_sleep(philo) == ERROR)
            break ;
        if (p_think(philo) == ERROR)
            break ;
    }
    printf("finished loop in thread %d\n", philo->id);
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
            if (get_time_in_ms() - philos[i].last_eaten > philos[i].die)
            {
                log_msg(&philos[i], DEATH);
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