/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 08:40:37 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/21 11:36:45 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/philosophers.h"

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

	(void)philo;
	log_msg(philo, STOP_STATUS);
	// TODO: maybe change it to return 0 if false, 1 if dead, 2 if full so we can distinguish
	stopped = *(philo->shared->stop_simulation);
	return (stopped);
}
