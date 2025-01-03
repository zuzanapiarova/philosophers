/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 18:43:21 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/03 19:22:02 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int init_philo(t_philo *philo, int i, char **argv, pthread_mutex_t **forks, pthread_mutex_t *msg_lock)
{
	philo->id = i + 1;
	philo->total = ft_atou(argv[1]);
	philo->die = ft_atou(argv[2]);
	philo->eat = ft_atou(argv[3]);
	philo->sleep = ft_atou(argv[4]);
	if (argv[5])//(argc == 6)
		philo->times_to_eat = ft_atou(argv[5]);
	else
		philo->times_to_eat = -1;
	philo->dead = false;
	philo->forks = *forks;
	philo->thinks = 0;
	philo->sleeps = 0;
	philo->eats = 0;
	philo->times_eaten = 0;
	philo->last_eaten = get_time_in_ms();
	pthread_mutex_init(&philo->lock, NULL); 
	philo->msg_lock = msg_lock;
	if (pthread_create(&philo->thread, NULL, &routine, (void *)philo) != 0)
		return (ERROR);
	return (SUCCESS);
}