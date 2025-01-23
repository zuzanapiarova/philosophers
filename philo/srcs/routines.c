/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routines.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 10:00:49 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/23 15:58:39 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// performs actions in order - take forks - eat - sleep - think - repeat
// after each action checks if the philo did not die while performing it 
// if yes, returns error and breaks out of the loop
// first if() checks if times_to_eat is not 0 
// returns ERROR if loop should stop, SUCCESS to continue
// testing:		// log_msg(p, FULL);
int	perform_actions(t_philo *p)
{
	if (p->times_to_eat == 0)
		return (ERROR);
	if (take_forks(p) == ERROR)
		return (ERROR);
	if (p_eat(p) == ERROR)
	{
		leave_forks(p);
		return (ERROR);
	}
	if (leave_forks(p) == ERROR)
		return (ERROR);
	if ((int)p->times_eaten == p->times_to_eat)
	{
		pthread_mutex_lock(p->stop_lock);
		*(p->stop_simulation) += 1;
		pthread_mutex_unlock(p->stop_lock);
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
		usleep((philo->eat / 2) * 1000);
	while (1)
	{
		if (perform_actions(philo) == ERROR)
			break ;
	}
	return (NULL);
}

void	unlock_two_mutexes(pthread_mutex_t *one, pthread_mutex_t *two)
{
	pthread_mutex_unlock(one);
	pthread_mutex_unlock(two);
}

// testing: //	log_msg(philo, FINISH);
void	finish_simulation(t_philo *philo)
{
	unlock_two_mutexes(&philo->lock, philo->stop_lock);
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
			pthread_mutex_lock(&philos[i].lock);
			if (get_time_in_micros() - philos[i].last_eaten
				> (philos[i].die * 1000))
			{
				*(philos[i].stop_simulation) = philos[i].total;
				log_msg(&philos[i], DEATH);
			}
			if (*(philos[i].stop_simulation) >= philos[i].total)
				return (finish_simulation(&philos[i]), NULL);
			unlock_two_mutexes(&philos[i].lock, philos[i].stop_lock);
		}
		usleep(1000);
	}
}
