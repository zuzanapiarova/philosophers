/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 09:28:58 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/13 19:43:41 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int	right(t_philo *philo, pthread_mutex_t *right_fork, pthread_mutex_t *left_fork)
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
	return (SUCCESS);
}

int	left(t_philo *philo, pthread_mutex_t *left_fork, pthread_mutex_t *right_fork)
{
	pthread_mutex_lock(left_fork);
	if (check_stop_sim(philo))
	{
		pthread_mutex_unlock(left_fork);
		return (ERROR);
	}
	log_msg(philo, FORK_L);
	if (philo->total == 1)
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
	return (SUCCESS);
}

// every philo takes fork on its id and the one before it
// last one takes the fork on id - 1 and on forks[0]
// for even philos, first lock R, then l fork
// for odd philos, lock first L, then R fork
// for last philo, take fork[id] as left and fork[0] as right fork
int	take_forks(t_philo *philo)
{
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;

	left_fork = &philo->forks[philo->id - 1];
	if (philo->id == philo->total)
		right_fork = &philo->forks[0];
	else
		right_fork = &philo->forks[philo->id];
	if (check_stop_sim(philo))
		return (ERROR);
	if (philo->id % 2 == 0)
	{
		if (right(philo, right_fork, left_fork) == ERROR)
		return (ERROR);
	} 
	else
	{
		if (left(philo, left_fork, right_fork) == ERROR)
		return (ERROR);
	}
	return (SUCCESS);
}

int	p_eat(t_philo *philo)
{
	if (check_stop_sim(philo))
		return (ERROR);
	pthread_mutex_lock(&philo->lock);
	if ((int)philo->times_eaten > -1)
		philo->times_eaten++;
	philo->last_eaten = get_time_in_micros();
	log_msg(philo, EATS);
	usleep(philo->eat * 1000);
	pthread_mutex_unlock(&philo->lock);
	return (0);
}

int	leave_forks(t_philo *philo)
{
	pthread_mutex_unlock(&philo->forks[philo->id - 1]);
	if (philo->id == philo->total)
		pthread_mutex_unlock(&philo->forks[0]);
	else
		pthread_mutex_unlock(&philo->forks[philo->id]);
	if (check_stop_sim(philo))
		return (ERROR);
	else
		return (0);
}

int	p_sleep(t_philo *philo)
{
	if (check_stop_sim(philo))
		return (ERROR);
	pthread_mutex_lock(&philo->lock);
	log_msg(philo, SLEEPS);
	if (philo->sleep != 0)
		usleep(philo->sleep * 1000);
	else
		usleep(2000);
	pthread_mutex_unlock(&philo->lock);
	return (0);
}

int	p_think(t_philo *philo)
{
	long long	time_passed;
	long long	time_left;
	long long	time_for_thinking;

	if (check_stop_sim(philo))
		return (ERROR);
	pthread_mutex_lock(&philo->lock);
	log_msg(philo, THINKS);
	time_passed = get_time_in_micros() - philo->last_eaten;
	time_left = (philo->die * 1000) - time_passed;
	time_for_thinking = (philo->die - philo->eat - philo->sleep) * 1000;
	if (time_left >= time_for_thinking / 2)
		usleep(time_left / 4);
	else if (time_left > 100)
		usleep(10);
	pthread_mutex_unlock(&philo->lock);
	return (0);
}
