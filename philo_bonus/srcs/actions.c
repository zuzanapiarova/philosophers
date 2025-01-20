/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 09:28:58 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/20 11:42:35 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int	take_forks(t_philo *philo)
{
	if (check_stop_sim(philo))
		return (ERROR);
	sem_wait(philo->shared->fork_sem);
	log_msg(philo, FORK_L);
	if (check_stop_sim(philo))
	{
		sem_post(philo->shared->fork_sem);
		return (ERROR);
	}
	sem_wait(philo->shared->fork_sem);
	log_msg(philo, FORK_R);
	if (check_stop_sim(philo))
	{
		leave_forks(philo);
		return (ERROR);
	}
	return (SUCCESS);
}

int	leave_forks(t_philo *philo)
{
	sem_post(philo->shared->fork_sem);
	sem_post(philo->shared->fork_sem);
	return (SUCCESS);
}

int	p_eat(t_philo *philo)
{
	if (check_stop_sim(philo))
		return (ERROR);
	if ((int)philo->times_eaten > -1)
		philo->times_eaten++;
	philo->last_eaten = get_time_in_micros();
	log_msg(philo, EATS);
	usleep(philo->eat * 1000);
	return (0);
}

int	p_sleep(t_philo *philo)
{
	if (check_stop_sim(philo))
		return (ERROR);
	log_msg(philo, SLEEPS);
	if (philo->sleep != 0)
		usleep(philo->sleep * 1000);
	else
		usleep(500);
	return (0);
}

int	p_think(t_philo *philo)
{
	long long	time_passed;
	long long	time_left;
	long long	time_for_thinking;

	if (check_stop_sim(philo))
		return (ERROR);
	time_passed = get_time_in_micros() - philo->last_eaten;
	time_left = (philo->die * 1000) - time_passed;
	time_for_thinking = (philo->die - philo->eat - philo->sleep) * 1000;
	log_msg(philo, THINKS);
	if (time_left >= time_for_thinking / 2)
		usleep(time_left / 4);
	else
		usleep(10);
	return (0);
}
