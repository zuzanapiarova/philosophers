/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 08:40:37 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/09 10:49:55 by zuzanapiaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

long long	get_time_in_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == 0)
		return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000) + TIMEZONE_MILI);
	return (ERROR);
}

long long	get_time_in_micros(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == 0)
		return ((tv.tv_sec * 1000000) + (tv.tv_usec) + TIMEZONE_MICRO);
	return (ERROR);
}

// use mutex to lock access to terminal while one log msg is being printed out from another thread wanting to print a msg
int	log_msg(t_philo *philo, t_action action)
{
	char	*msg;

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
	char *str = ft_utoa((get_time_in_micros() / 1000));
	pthread_mutex_lock(philo->msg_lock);
	//ft_putnbr(get_time_in_micros() / 1000);
	write(1, str, ft_strlen(str));
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
	free(str);
	return (0);
}

bool	check_stop_sim(t_philo *philo)
{
    bool	stopped;

    pthread_mutex_lock(philo->stop_lock);
    stopped = *(philo->stop_simulation);
    pthread_mutex_unlock(philo->stop_lock);
	if (stopped == false)
		stopped = philo->stopped;
    return (stopped);
}
