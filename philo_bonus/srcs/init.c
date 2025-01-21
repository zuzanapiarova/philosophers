/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 10:21:56 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/20 20:44:27 by zuzanapiaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"../includes/philosophers.h"

int	init_shared_resources(t_shared *shared, int total)
{
	// create semaphore representing forks ---------------------------------------------------------------------------------------------------------------------------------------------------------
	shared->fork_sem = sem_open(FORK_SEM, O_CREAT | O_EXCL, 0644, total); // need to check the permissions later
	if (shared->fork_sem == SEM_FAILED)
		return (write(2, "Error creating forks semaphore.\n", 32), ERROR);
	//  create semaphore for locking msg output ----------------------------------------------------------------------------------------------------------------------------------------------------
	shared->msg_sem = sem_open(MSG_SEM, O_CREAT | O_EXCL, 0644, 1); // need to check the permissions later
	if (shared->msg_sem == SEM_FAILED)
	{
		sem_close(shared->fork_sem);
		sem_unlink(FORK_SEM);
		return (write(2, "Error creating message semaphore.\n", 34), ERROR);
	}
	//  create semaphore for stop simulation signalling ---------------------------------------------------------------------------------------------------------------------------------------------
	shared->stop_sem = sem_open(STOP_SEM, O_CREAT | O_EXCL, 0644, 0); // need to check the permissions later
	if (shared->msg_sem == SEM_FAILED)
	{
		sem_close(shared->fork_sem);
		sem_unlink(FORK_SEM);
		sem_close(shared->msg_sem);
		sem_unlink(MSG_SEM);
		return (write(2, "Error creating message semaphore.\n", 34), ERROR);
	}
    shared->full_sem = sem_open(FULL_SEM, O_CREAT | O_EXCL, 0644, 0); // need to check the permissions later
	if (shared->msg_sem == SEM_FAILED)
	{
		sem_close(shared->fork_sem);
		sem_unlink(FORK_SEM);
		sem_close(shared->msg_sem);
		sem_unlink(MSG_SEM);
        sem_close(shared->stop_sem);
		sem_unlink(STOP_SEM);
		return (write(2, "Error creating message semaphore.\n", 34), ERROR);
	}
	// create semaphore for stop_simulation ---------------------------------------------------------------------------------------------------------------------------------------------------------
	shared->monitoring_sem = sem_open(MONITORING_SEM, O_CREAT | O_EXCL, 0644, 0); // need to check the permissions later
	if (shared->monitoring_sem == SEM_FAILED)
	{
		sem_close(shared->fork_sem);
		sem_unlink(FORK_SEM);
		sem_close(shared->msg_sem);
		sem_unlink(MSG_SEM);
		sem_close(shared->stop_sem);
		sem_unlink(STOP_SEM);
        sem_close(shared->full_sem);
		sem_unlink(FULL_SEM);
		return (write(2, "Error creating monitoring semaphore.\n", 37), ERROR);
	}
	// /*(shared->stop_simulation) = false;
	shared->start_time = get_time_in_micros(); // or here set to 0 and in code move further
    return (SUCCESS);
}

// initialzes each philo separately from argv and shared resources
// each philo is a separate entity and cannot see the data of other philos
// appart from shared resources: stop_simulation, forks, msg_lock, stop_lock
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
	// TODO: probably add a state_sem semaphore for reading shared resources but idk if needed
	return (SUCCESS);
}

// allocated pids array, philo array, shared resources and each philo structure
int	init_resources(t_philo **philos, pid_t **pids, t_shared *shared, char **argv)
{
	int	i;
	int	total;

	total = ft_atou(argv[1]);
	// create pids array -----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	*pids = (pid_t *)malloc(sizeof(pid_t) * total);
	if (!(*pids))
		return (write(2, "Memory allocation problem. Exiting.\n", 36), ERROR);
	// create philo structures - alloc ---------------------------------------------------------------------------------------------------------------------------------------------------------
	*philos = (t_philo *)malloc(sizeof(t_philo) * total);
	if (!(*philos))
	{
		free(*pids);
		return (write(2, "Memory allocation problem. Exiting.\n", 36), ERROR);
	}
	// init shared struct, init philos structures, and pass them the shared struct -------------------------------------------------------------------------------------------------------------
	if (init_shared_resources(shared, total) == ERROR)
	{
		free(*pids);
		free(*philos);
		return (ERROR);
	}
	i = -1;
	while (++i < total)
		init_philo_data(&(*philos)[i], i, argv, shared);
	return (SUCCESS);
}
