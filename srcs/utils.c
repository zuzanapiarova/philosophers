/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 12:41:01 by zuzanapiaro       #+#    #+#             */
/*   Updated: 2024/12/30 14:52:26 by zuzanapiaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

long long get_time_in_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == 0)
		return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (ERROR);
}

// use mutex to lock access to terminal while one log msg is being printed out from another thread wanting to print a msg
int	log_msg(t_philo *philo, t_action action)
{
	ft_putnbr(get_time_in_ms());
	write(1, " ", 1);
	ft_putnbr(philo->id);
	write(1, " ", 1);
	if (action == FORK)
		write(1, " has taken a fork\n", 18);
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
	return (0);
}
