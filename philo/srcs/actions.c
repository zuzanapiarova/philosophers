/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 09:28:58 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/23 13:02:05 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int	p_eat(t_philo *philo)
{
	if (check_stop_sim(philo))
		return (ERROR);
	pthread_mutex_lock(&philo->lock);
	if ((int)philo->times_eaten > -1)
		philo->times_eaten++;
	philo->last_eaten = get_time_in_micros();
	pthread_mutex_unlock(&philo->lock);
	log_msg(philo, EATS);
	usleep(philo->eat * 1000);
	return (SUCCESS);
}

int	p_sleep(t_philo *philo)
{
	long long	time_passed;
	long long	time_left;

	if (check_stop_sim(philo))
		return (ERROR);
	log_msg(philo, SLEEPS);
	usleep(philo->sleep * 1000);
	if (philo->sleep < 80)
	{
		time_passed = get_time_in_micros() - philo->last_eaten;
		time_left = (philo->die * 1000) - time_passed;
		usleep(time_left / 2);
	}
	return (SUCCESS);
}

int	p_think(t_philo *philo)
{
	long long	time_passed;
	long long	time_left;
	long long	time_for_thinking;

	if (check_stop_sim(philo))
		return (ERROR);
	pthread_mutex_lock(&philo->lock);
	time_passed = get_time_in_micros() - philo->last_eaten;
	time_left = (philo->die * 1000) - time_passed;
	time_for_thinking = (philo->die - philo->eat - philo->sleep) * 1000;
	pthread_mutex_unlock(&philo->lock);
	log_msg(philo, THINKS);
	if (time_left >= time_for_thinking / 2)
		usleep(time_left / 4);
	else
		usleep(10);
	return (SUCCESS);
}
