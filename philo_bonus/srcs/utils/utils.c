/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 08:40:37 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/22 17:09:03 by zpiarova         ###   ########.fr       */
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

	sem_wait(philo->mutex_local_sem);
	stopped = philo->stop_simulation;
	sem_post(philo->mutex_local_sem);
	return (stopped);
}

// gets specific name for local philo mutex-representing semaphore
char	*get_mutex_sem_name(t_philo *philo)
{
	char	*name;
	char	*number;

	number = ft_utoa(philo->id);
	name = ft_strjoin("mutex_sem", number);
	free(number);
	return (name);
}
