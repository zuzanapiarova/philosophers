/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 12:41:01 by zuzanapiaro       #+#    #+#             */
/*   Updated: 2025/01/04 20:49:41 by zuzanapiaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

long long	get_time_in_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == 0)
		return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000) + TIMEZONE_MS);
	return (ERROR);
}

// use mutex to lock access to terminal while one log msg is being printed out from another thread wanting to print a msg
int	log_msg(t_philo *philo, t_action action)
{
	pthread_mutex_lock(philo->msg_lock);
	ft_putnbr(get_time_in_ms());
	write(1, " ", 1);
	ft_putnbr(philo->id);
	write(1, " ", 1);
	if (action == FORK_L)
		write(1, " has taken left fork\n", 21);
	else if (action == FORK_R)
		write(1, " has taken right fork\n", 22);
	else if (action == EATS)
	{
		write(1, " is eating for ", 15);
		ft_putnbr(philo->times_eaten);
		write(1, ". time\n", 7);
	}
	else if (action == THINKS)
		write(1, " is thinking\n", 13);
	else if (action == SLEEPS)
		write(1, " is sleeping\n", 13);
	else if (action == DEATH)
		write(1, " died\n", 6);
	pthread_mutex_unlock(philo->msg_lock);
	return (0);
}

bool	check_stop_sim(t_philo *philo)
{
	pthread_mutex_lock(philo->stop_lock);
	if (*(philo->stop_simulation) == true)
	{
		pthread_mutex_unlock(philo->stop_lock);
		return (true);
	}
	pthread_mutex_unlock(philo->stop_lock);
	return (false);
}
