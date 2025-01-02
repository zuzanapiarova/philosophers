/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 16:41:14 by zuzanapiaro       #+#    #+#             */
/*   Updated: 2025/01/02 20:28:29 by zuzanapiaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int p_eat(t_philo *philo)
{
	long long	time;

	time = get_time_in_ms();
	philo->last_eaten = time;
	log_msg(philo, EATS);
	write(1, "time of last meal: ", 19);
	ft_putnbr(time);
	write(1, "\n", 1);
	usleep(philo->eat * 1000); // TODO: check if *1000 is correct - what data type does usleep expect?
	if ((int)philo->times_eaten > -1)
		philo->times_eaten++;
	return (0);
}

int p_sleep(t_philo *philo)
{
	log_msg(philo, SLEEPS);
	usleep(philo->sleep * 1000);
	return (0);
}

int take_forks(t_philo *philo)
{
	if (philo->id % 2 == 0)
	{
		// lock fork on right
		pthread_mutex_lock(&philo->forks[philo->id]);
		log_msg(philo, FORK_R);
		// lock fork on left
		pthread_mutex_lock(&philo->forks[philo->id - 2]);
		log_msg(philo, FORK_L); // maybe implement FORK_LEFT and FORK_RIGHT ???
	}
	else
	{
		pthread_mutex_lock(&(philo->forks[philo->id - 2]));
		log_msg(philo, FORK_L); // maybe implement FORK_LEFT and FORK_RIGHT ???
		// lock fork on right
		pthread_mutex_lock(&philo->forks[philo->id]);
		log_msg(philo, FORK_R);
	}
	return (0);
}

int leave_forks(t_philo *philo)
{
	pthread_mutex_unlock(&philo->forks[philo->id - 2]);
	pthread_mutex_unlock(&philo->forks[philo->id]);
	return (0);
}
