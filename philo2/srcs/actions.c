/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 09:28:58 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/06 11:59:39 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int p_eat(t_philo *philo)
{
	if (log_msg(philo, EATS))
        return (ERROR);
	philo->last_eaten = get_time_in_ms();
	if ((int)philo->times_eaten > -1)
		philo->times_eaten++;
	usleep(philo->eat * 1000);
	return (0);
}

int p_sleep(t_philo *philo)
{
    if (log_msg(philo, SLEEPS)) // maybe add this after each iteration
		return (ERROR);
	usleep(philo->sleep * 1000);
	return (0);
}

int p_think(t_philo *philo)
{
    if (log_msg(philo, THINKS)) // maybe add this after each iteration
		return (ERROR);
    //if ((get_time_in_ms() - philo->last_eaten) > ((philo->die - philo->eat - philo->sleep) / 2))
	usleep(((philo->die - philo->eat - philo->sleep) / 2)* 1000);
	return (0);
}

// every philo takes fork on its id and the one before it
// last one takes the fork on id - 1 and on forks[0]
// for even philos, first lock R, then l fork
// for odd philos, lock first L, then R fork
// for last philo,
int take_forks(t_philo *philo)
{
    if (check_stop_sim(philo)) // maybe add this after each iteration
		return (ERROR);
	if (philo->id % 2 == 0)
	{
		if (philo->id == philo->total)
			pthread_mutex_lock(&philo->forks[0]);
		else
			pthread_mutex_lock(&philo->forks[philo->id]);
		log_msg(philo, FORK_R);
		pthread_mutex_lock(&philo->forks[philo->id - 1]);
		log_msg(philo, FORK_L);
	}
	else
	{
		pthread_mutex_lock(&philo->forks[philo->id - 1]);
		log_msg(philo, FORK_L);
		if (philo->id == philo->total)
			pthread_mutex_lock(&philo->forks[0]);
		else
			pthread_mutex_lock(&philo->forks[philo->id]);
		log_msg(philo, FORK_R);
	}
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