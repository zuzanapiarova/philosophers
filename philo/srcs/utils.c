/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 08:40:37 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/07 15:57:44 by zpiarova         ###   ########.fr       */
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

long long	get_time_in_micros(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == 0)
		return ((tv.tv_sec * 1000000) + (tv.tv_usec) + TIMEZONE_MS);
	return (ERROR);
}

// use mutex to lock access to terminal while one log msg is being printed out from another thread wanting to print a msg
int	log_msg(t_philo *philo, t_action action)
{
	char	*msg;
	
	// *if (check_stop_sim(philo) && action != DEATH)
	// *return (ERROR);
	if (action == FORK_L)
		msg = "has taken left fork\n";
	else if (action == FORK_R)
		msg = "has taken right fork\n";
	else if (action == EATS)
		msg = "is eating for ";
	else if (action == THINKS)
		msg = "is thinking\n";
	else if (action == SLEEPS)
		msg = "is sleeping\n";
	else if (action == DEATH)
		msg = "died\n";
	else if (action == FINISH)
		msg = "finished last meal\n";
	pthread_mutex_lock(philo->msg_lock);
	ft_putnbr(get_time_in_ms());
	write(1, " ", 1);
	ft_putnbr(philo->id);
	write(1, " ", 1);
	write(1, msg, ft_strlen(msg));
	if (action == EATS)
	{
		ft_putnbr(philo->times_eaten);
		write(1, ". time\n", 7);
	}
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