/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 10:21:56 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/22 19:21:37 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int	init_global_semaphores(t_shared *shared, int total)
{
	shared->fork_sem = sem_open(FORK_SEM, O_CREAT | O_EXCL, 0644, total);
	if (shared->fork_sem == SEM_FAILED)
		return (write(2, "Error creating forks semaphore.\n", 32), ERROR);
	shared->msg_sem = sem_open(MSG_SEM, O_CREAT | O_EXCL, 0644, 1);
	if (shared->msg_sem == SEM_FAILED)
	{
		sem_close(shared->fork_sem);
		sem_unlink(FORK_SEM);
		return (write(2, "Error creating message semaphore.\n", 34), ERROR);
	}
	shared->stop_sem = sem_open(STOP_SEM, O_CREAT | O_EXCL, 0644, 0);
	if (shared->msg_sem == SEM_FAILED)
	{
		sem_close(shared->fork_sem);
		sem_unlink(FORK_SEM);
		sem_close(shared->msg_sem);
		sem_unlink(MSG_SEM);
		return (write(2, "Error creating stop semaphore.\n", 31), ERROR);
	}
	shared->fullness_sem = sem_open(FULLNESS_SEM, O_CREAT | O_EXCL, 0644, 0);
	if (shared->fullness_sem == SEM_FAILED)
	{
		sem_close(shared->fork_sem);
		sem_unlink(FORK_SEM);
		sem_close(shared->msg_sem);
		sem_unlink(MSG_SEM);
		sem_close(shared->stop_sem);
		sem_unlink(STOP_SEM);
		return (write(2, "Error creating monitoring semaphore.\n", 37), ERROR);
	}
	return (SUCCESS);
}

// initialzes each philo separately
// each philo is a separate entity and cannot see the data of other philos
// data that will be intialized in each process separately so it doesnt consume
// memory of all processes: mutex_sem_name, mutex_local_sem, mutex_sem_name
int	init_philo_data(t_philo *p, int i, char **argv, t_shared *shared)
{
	p->id = i + 1;
	p->total = ft_atou(argv[1]);
	p->die = ft_atou(argv[2]);
	p->eat = ft_atou(argv[3]);
	p->sleep = ft_atou(argv[4]);
	if (argv[5])
		p->times_to_eat = ft_atou(argv[5]);
	else
		p->times_to_eat = -1;
	p->times_eaten = 0;
	p->last_eaten = get_time_in_micros();
	p->shared = shared;
	p->stop_simulation = false;
	p->exit_status = 0;
	p->start_time = 0;
	return (SUCCESS);
}

// allocated pids array, philo array, shared resources and each philo structure
int	init_global_resources(t_philo **ph, pid_t **pids, t_shared *sh, char **agv)
{
	int	i;
	int	total;

	total = ft_atou(agv[1]);
	*pids = (pid_t *)malloc(sizeof(pid_t) * total);
	if (!(*pids))
		return (write(2, "Memory allocation problem. Exiting.\n", 36), ERROR);
	*ph = (t_philo *)malloc(sizeof(t_philo) * total);
	if (!(*ph))
	{
		free(*pids);
		return (write(2, "Memory allocation problem. Exiting.\n", 36), ERROR);
	}
	if (init_global_semaphores(sh, total) == ERROR)
	{
		free(*pids);
		free(*ph);
		return (ERROR);
	}
	i = -1;
	while (++i < total)
		init_philo_data(&(*ph)[i], i, agv, sh);
	return (SUCCESS);
}

// initializes local resources - local threads, local semaphore
int	init_local_resources(t_philo *philo)
{
	philo->mutex_sem_name = get_mutex_sem_name(philo);
	philo->mutex_local_sem
		= sem_open(philo->mutex_sem_name, O_CREAT | O_EXCL, 0644, 1);
	if (philo->mutex_local_sem == SEM_FAILED)
	{
		free(philo->mutex_sem_name);
		return (write(2, "Error creating mutex semaphore.\n", 32), ERROR);
	}
	if (pthread_create(&philo->stop_sim_checker, NULL, &stop_routine, philo)
		!= SUCCESS)
		return (ERROR);
	if (pthread_create(&philo->death_checker, NULL, &death_routine, philo)
		!= SUCCESS)
		return (ERROR);
	return (SUCCESS);
}
