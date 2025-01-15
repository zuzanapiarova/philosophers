/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 09:28:58 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/14 20:07:45 by zpiarova         ###   ########.fr       */
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
	return (0);
}

int	p_sleep(t_philo *philo)
{
	if (check_stop_sim(philo))
		return (ERROR);
	// pthread_mutex_lock(&philo->lock);
	// pthread_mutex_unlock(&philo->lock);
	log_msg(philo, SLEEPS);
	if (philo->sleep != 0)
		usleep(philo->sleep * 1000);
	else
		usleep(5000); // also change dynamically as in think time 
	return (0);
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
	else/*  if (time_left > 100) */
		usleep(10);
	return (0);
}
