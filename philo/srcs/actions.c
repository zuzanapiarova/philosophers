/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 09:28:58 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/06 17:42:50 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// every philo takes fork on its id and the one before it
// last one takes the fork on id - 1 and on forks[0]
// for even philos, first lock R, then l fork
// for odd philos, lock first L, then R fork
// for last philo,
int take_forks(t_philo *philo)
{
	pthread_mutex_t *left_fork;
	pthread_mutex_t *right_fork;
	
	left_fork = &philo->forks[philo->id - 1];
	if (philo->id == philo->total)
		right_fork = &philo->forks[0];
	else
		right_fork = &philo->forks[philo->id];
    if (check_stop_sim(philo))
		return (ERROR);
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(right_fork);
		if (check_stop_sim(philo))
		{
			pthread_mutex_unlock(right_fork);
			return (ERROR);
		}
		log_msg(philo, FORK_R);
		pthread_mutex_lock(left_fork);
		if (check_stop_sim(philo))
		{
			pthread_mutex_unlock(left_fork);
			pthread_mutex_unlock(right_fork);
			return (ERROR);
		}
		log_msg(philo, FORK_L);
	}
	else
	{
		pthread_mutex_lock(left_fork);
		if (check_stop_sim(philo))
		{
			pthread_mutex_unlock(left_fork);	
			return (ERROR);
		}
		log_msg(philo, FORK_L);
		if (philo->total == 1) // quick and dirty but do not know how else to do it since we cannot use other pthread functions 
		{
			usleep(philo->die * 1000);
			return (ERROR);
		}
		pthread_mutex_lock(right_fork);
		if (check_stop_sim(philo))
		{
			pthread_mutex_unlock(right_fork);
			pthread_mutex_unlock(left_fork);
			return (ERROR);
		}
		log_msg(philo, FORK_R);
	}
	return (0);
}

int p_eat(t_philo *philo)
{
 	if (check_stop_sim(philo))
		return (ERROR);
	log_msg(philo, EATS);
	philo->last_eaten = get_time_in_ms();
	if ((int)philo->times_eaten > -1)
		philo->times_eaten++;
	usleep(philo->eat * 1000);
	return (0);
}

int leave_forks(t_philo *philo)
{
	// unlock left fork
	pthread_mutex_unlock(&philo->forks[philo->id - 1]);
	// unlock right fork
	if (philo->id == philo->total)
		pthread_mutex_unlock(&philo->forks[0]);
	else
		pthread_mutex_unlock(&philo->forks[philo->id]);
    if (check_stop_sim(philo))
        return (ERROR);
    else
	    return (0);
}

int p_sleep(t_philo *philo)
{
	if (check_stop_sim(philo))
		return (ERROR);
	log_msg(philo, SLEEPS);
	usleep(philo->sleep * 1000);
	return (0);
}

int p_think(t_philo *philo)
{
	long long	time_passed;
	long long	time_left;
	long long	time_for_thinking;
	
	if (check_stop_sim(philo))
		return (ERROR);
	log_msg(philo, THINKS);
	time_passed = get_time_in_ms() - philo->last_eaten;
	time_left = philo->die - time_passed;
	time_for_thinking = philo->die - philo->eat - philo->sleep;
    // TODO: algo to make them think for a bit so others do not have to die - but how when we do not have access to other philo data ?
	// if (time_left / 10 >= time_for_thinking)
	// {
	// 	printf("true %d\n", philo->id);
	// 	usleep(time_left / 10 * 1000);
	// }
	usleep(10);
	return (0);
}