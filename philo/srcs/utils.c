/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 08:40:37 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/13 17:36:39 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// returns time in miliseconds
long long	get_time_in_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == 0)
		return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000) + TIMEZONE_MILI);
	return (ERROR);
}

// retusns time in microseconds
long long	get_time_in_micros(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == 0)
		return ((tv.tv_sec * 1000000) + (tv.tv_usec) + TIMEZONE_MICRO);
	return (ERROR);
}

// checks if stop_simulation is true meaning one of them died
// or checks if the specific philo finished eating
// if either is true, returns true and caller will stop the specific thread
bool	check_stop_sim(t_philo *philo)
{
	bool	stopped;

	pthread_mutex_lock(&philo->lock);
	stopped = *(philo->stop_simulation);
	pthread_mutex_unlock(&philo->lock);
	if (stopped || philo->finished)
		return (true);
	return (false);
}
