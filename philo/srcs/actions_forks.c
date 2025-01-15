/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions_forks.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 10:15:48 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/15 14:39:19 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// runs for right handed philo - divisible by 2
// first takes right fork, then left
// checks for philo death between fork grabbing
int	fork_r(t_philo *philo, pthread_mutex_t *right_f, pthread_mutex_t *left_f)
{
	pthread_mutex_lock(right_f);
	if (check_stop_sim(philo))
	{
		pthread_mutex_unlock(right_f);
		return (ERROR);
	}
	log_msg(philo, FORK_R);
	pthread_mutex_lock(left_f);
	if (check_stop_sim(philo))
	{
		pthread_mutex_unlock(left_f);
		pthread_mutex_unlock(right_f);
		return (ERROR);
	}
	log_msg(philo, FORK_L);
	return (SUCCESS);
}

// runs for left handed philo - not divisible by 2
// first takes left fork, then right
// checks for philo death between fork grabbing
int	fork_l(t_philo *philo, pthread_mutex_t *left_f, pthread_mutex_t *right_f)
{
	pthread_mutex_lock(left_f);
	if (check_stop_sim(philo))
	{
		pthread_mutex_unlock(left_f);
		return (ERROR);
	}
	log_msg(philo, FORK_L);
	if (philo->total == 1)
	{
		usleep(philo->die * 1000);
		pthread_mutex_unlock(left_f);
		return (ERROR);
	}
	pthread_mutex_lock(right_f);
	if (check_stop_sim(philo))
	{
		pthread_mutex_unlock(right_f);
		pthread_mutex_unlock(left_f);
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
		if (fork_r(philo, right_fork, left_fork) == ERROR)
			return (ERROR);
	}
	else
	{
		if (fork_l(philo, left_fork, right_fork) == ERROR)
			return (ERROR);
	}
	return (SUCCESS);
}

// leaves both forks
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
