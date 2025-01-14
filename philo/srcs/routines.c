/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routines.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 10:00:49 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/14 13:08:46 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// performs actions in order - take forks - eat - sleep - think - repeat
// after each action checks if the philo did not die while performing it 
// if yes, returns error and breaks out of the loop
// at beginning checks if the times_to_eat is not 0 or smaller than times_eaten
int	perform_actions(t_philo *p)
{
	if (((int)p->times_eaten >= p->times_to_eat && p->times_to_eat != -1))
	{
		p->finished = true;
		return (ERROR);
	}
	if (take_forks(p) == ERROR)
		return (ERROR);
	if (p_eat(p) == ERROR)
	{
		leave_forks(p);
		return (ERROR);
	}
	if (leave_forks(p) == ERROR)
		return (ERROR);
	if (p->times_to_eat != -1 && (int)p->times_eaten >= p->times_to_eat)
	{
		p->finished = true;
		log_msg(p, FINISH);
		return (ERROR);
	}
	if (p_sleep(p) == ERROR)
		return (ERROR);
	if (p_think(p) == ERROR)
		return (ERROR);
	return (SUCCESS);
}

// runs for each thread
// first condition in while loop checks if the number of times to eat is not 0
// or if it is not bigger than times_to_eat
void	*routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->id % 2 == 0)
		usleep(10);
	while (1)
	{
		if (perform_actions(philo) == ERROR)
			break ;
	}
	return (NULL);
}

// runs in a separate monitoring thread to check for deaths
// every 5ms loops through all philos
// and checks if time since last meal did not surpass time_to_die
// if it finds death, returns
// does not check anymore for philos that finished eating
void	*monitoring(void *arg)
{
	int		i;
	t_philo	*philos;

	philos = (t_philo *)arg;
	while (1)
	{
		i = -1;
		while (++i < philos[0].total)
		{
			pthread_mutex_lock(philos[i].stop_lock);
			if (get_time_in_micros() - philos[i].last_eaten
				> (philos[i].die * 1000) && !philos->finished)
				*(philos[i].stop_simulation) = true;
			if (*(philos[i].stop_simulation) == true)
			{
				if (!philos[i].finished)
					log_msg(&philos[i], DEATH);
				pthread_mutex_unlock(philos[i].stop_lock);
				return (NULL);
			}
			pthread_mutex_unlock(philos[i].stop_lock);
		}
		usleep(5000);
	}
}
