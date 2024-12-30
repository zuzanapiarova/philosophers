/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 16:41:14 by zuzanapiaro       #+#    #+#             */
/*   Updated: 2024/12/30 01:07:33 by zuzanapiaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int p_eat(t_info *info, t_philo *philo)
{
	long long	time;

	time = get_time_in_ms();
	write(1, "time of last meal: ", 19);
	ft_putnbr(time);
	write(1, "\n", 1);
	philo->last_eaten = time;
	usleep(info->eat * 1000);
	if ((int)philo->times_eaten != -1)
		philo->times_eaten++;
	log_msg(info, philo, EATS);
	return (0);
}

int p_sleep(t_info *info, t_philo *philo)
{
	usleep(info->sleep * 1000);
	log_msg(info, philo, SLEEPS);
	return (0);
}

int take_fork(t_info *info, t_philo *philo)
{
	// some algorithm to decide when to take forks
	// lock fork on left
	pthread_mutex_lock(&info->forks[philo->id - 2]);
	log_msg(info, philo, FORK); // maybe implement FORK_LEFT and FORK_RIGHT ???
	// lock fork on right
	pthread_mutex_lock(&info->forks[philo->id]);
	log_msg(info, philo, FORK);
	return (0);
}

int leave_forks(t_info *info, t_philo *philo)
{
	pthread_mutex_unlock(&info->forks[philo->id - 2]);
	pthread_mutex_unlock(&info->forks[philo->id]);
	return (0);
}
