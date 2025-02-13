/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 09:28:58 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/23 14:33:35 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int	take_forks(t_philo *philo)
{
	if (check_stop_sim(philo))
		return (ERROR);
	sem_wait(philo->shared->fork_sem);
	if (check_stop_sim(philo))
	{
		sem_post(philo->shared->fork_sem);
		return (ERROR);
	}
	log_msg(philo, FORK_L);
	if (philo->total == 1)
	{
		usleep(philo->die * 1000);
		sem_post(philo->shared->fork_sem);
		return (ERROR);
	}
	sem_wait(philo->shared->fork_sem);
	if (check_stop_sim(philo))
	{
		leave_forks(philo);
		return (ERROR);
	}
	log_msg(philo, FORK_R);
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
	{
		leave_forks(philo);
		return (ERROR);
	}
	if ((int)philo->times_eaten > -1)
		philo->times_eaten++;
	sem_wait(philo->mutex_local_sem);
	philo->last_eaten = get_time_in_micros();
	sem_post(philo->mutex_local_sem);
	log_msg(philo, EATS);
	usleep(philo->eat * 1000);
	if (check_stop_sim(philo))
	{
		leave_forks(philo);
		return (ERROR);
	}
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
	time_passed = get_time_in_micros() - philo->last_eaten;
	time_left = (philo->die * 1000) - time_passed;
	time_for_thinking = (philo->die - philo->eat - philo->sleep) * 1000;
	log_msg(philo, THINKS);
	if (time_left >= time_for_thinking / 2)
		usleep(time_left / 4);
	else
		usleep(10);
	return (SUCCESS);
}
