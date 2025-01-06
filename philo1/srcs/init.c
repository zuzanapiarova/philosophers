/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 18:43:21 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/04 20:58:29 by zuzanapiaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int init_philo(t_philo *philo, int i, char **argv, pthread_mutex_t **forks, pthread_mutex_t *msg_lock, pthread_mutex_t *stop_lock, bool *stop_simulation)
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
	philo->forks = *forks;
	philo->times_eaten = 0;
	philo->last_eaten = get_time_in_ms();
	pthread_mutex_init(&philo->lock, NULL);
	philo->msg_lock = msg_lock;
	philo->stop_lock = stop_lock;
	philo->stop_simulation = stop_simulation; // !!!
	if (pthread_create(&philo->thread, NULL, &routine, (void *)philo) != 0)
		return (ERROR);
	return (SUCCESS);
}
