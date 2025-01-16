/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:05:45 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/16 11:13:35 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// TODO: monitoring process - monitoring semaphore
// TODO: do i need any algo to allow fair access to resources like sleepng for a bit in threads?
// TODO: log messages are sometimes mixed up - possibly implement a binary semaphore for access to stdout ? 
// TODO: --> msg semaphore - binary
// TODO: change check_im_status function to accept a semaphore 
int	child_process(t_philo *philo)
{
	while (1)
	{
		printf("D\n");
		if (((int)philo->times_eaten >= philo->times_to_eat && philo->times_to_eat != -1))
		{
			philo->finished = true;
			break ;
		}
		printf("E\n");
		// take forks - wait(decrement) semaphore
		sem_wait(philo->shared->fork_sem);
		printf("gor here\n");
		log_msg(philo, FORK_L);
		sem_wait(philo->shared->fork_sem);
		log_msg(philo, FORK_R);
		printf("F\n");
		// Eat
		if (p_eat(philo) == ERROR)
		{
			sem_post(philo->shared->fork_sem);
			sem_post(philo->shared->fork_sem);
			break ;
		}
		// leave forks - signal(return to) semaphore
		sem_post(philo->shared->fork_sem);
		sem_post(philo->shared->fork_sem);
		// check if times_eaten is not equal to times_to_reach thus philo is full(finished)
		if (philo->times_to_eat != -1 && (int)philo->times_eaten >= philo->times_to_eat)
		{
			philo->finished = true;
			log_msg(philo, FINISH);
			break ;
		}
		if (p_sleep(philo) == ERROR)
			break ;
		if (p_think(philo) == ERROR)
			break ;
		// TODO: change death checking, for now it is after each actions iteration
		if (get_time_in_micros() - philo->last_eaten > (philo->die * 1000) && !philo->finished)
		{
			// ? *(philos[i].stop_simulation) = true;
			log_msg(philo, DEATH);
			break ;
		}
	}
	printf("Resources destroyed in %d. Program exiting.\n", philo->id);
	// must clean all resources - mallocs and sems before exiting 
	return (SUCCESS); // Exit child process
}

int	monitor(t_philo **philos)
{
	return (SUCCESS);
}

int	init_shared_resources(t_shared *shared, int total)
{	
	// create semaphore representing forks ---------------------------------------------------------------------------------------------------------------------------------------------------------
	shared->fork_sem = sem_open(FORK_SEM, O_CREAT | O_EXCL, 0644, total); // need to check the permissions later 
	if (shared->fork_sem == SEM_FAILED) 
		return (write(2, "Error creating forks semaphore.\n", 32), ERROR);
	shared->msg_sem = sem_open(MSG_SEM, O_CREAT | O_EXCL, 0644, 1); // need to check the permissions later 
	//  create semaphore for locking msg output ----------------------------------------------------------------------------------------------------------------------------------------------------
	if (shared->msg_sem == SEM_FAILED)  
	{
		sem_close(shared->fork_sem);
		sem_unlink(FORK_SEM);
		return (write(2, "Error creating message semaphore.\n", 34), ERROR);
	}
	shared->monitoring_sem = sem_open(MONITORING_SEM, O_CREAT | O_EXCL, 0644, 1); // need to check the permissions later 
	// create semaphore for stop_simulation ---------------------------------------------------------------------------------------------------------------------------------------------------------
	if (shared->monitoring_sem == SEM_FAILED) 
	{
		sem_close(shared->fork_sem);
		sem_unlink(FORK_SEM);
		sem_close(shared->msg_sem);
		sem_unlink(MSG_SEM);
		return (write(2, "Error creating monitoring semaphore.\n", 37), ERROR);
	}
	shared->start_time = get_time_in_micros(); // or here set to 0 and in code move further 
	// ? shared->stop_simulation = false;
	return (SUCCESS);
}

int	destroy_semaphores(t_shared *shared)
{
	sem_close(shared->fork_sem);
	sem_unlink(FORK_SEM);
	sem_close(shared->msg_sem);
	sem_unlink(MSG_SEM);
	sem_close(shared->monitoring_sem);
	sem_unlink(MONITORING_SEM);
	return (SUCCESS);
}

// initialzes each philo separately from argv and shared resources
// each philo is a separate entity and cannot see the data of other philos
// appart from shared resources: stop_simulation, forks, msg_lock, stop_lock
int	init_philo_data(t_philo *p, int i, char **argv, t_shared *shared)
{
	printf("A\n");
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
	p->finished = false;
	p->shared = shared;
	printf("B\n");
	// shared (each process creates its own copy though, only named semaphores are trully shared): 
	// TODO: probably add a state_sem semaphore for reading shared resources but idk if needed
	// ? p->stop_simulation = &shared->stop_simulation;
	printf("C\n");
	return (SUCCESS);
}

// there are no shared resources as the memory for each process is duplicated, not shared even for pointers
int	start_simulation(int argc, char **argv, int total)
{
	int			i;
	pid_t		*pids;
	t_philo		*philos;
	t_shared	shared;

	i = -1;
	// create pids array -----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	pids = (pid_t *)malloc(sizeof(pid_t) * total);
	if (!pids)
		return (write(2, "Memory allocation problem. Exiting.\n", 36), ERROR);
	// create philo structures - alloc ---------------------------------------------------------------------------------------------------------------------------------------------------------
	philos = (t_philo *)malloc(sizeof(t_philo) * total);
	if (!philos)
	{
		free(pids);
		return (write(2, "Memory allocation problem. Exiting.\n", 36), ERROR);
	}
	// init shared struct, init philos structures, and pass them the shared struct -------------------------------------------------------------------------------------------------------------
	// create semaphore representing forks ---------------------------------------------------------------------------------------------------------------------------------------------------------
	// printf("creating forks semaphore\n");
	// shared.fork_sem = sem_open(FORK_SEM, O_CREAT | O_EXCL, 0644, total); // need to check the permissions later 
	// if (shared.fork_sem == SEM_FAILED) 
	// {
	// 	free(pids);
	// 	free(philos);
	// 	return (write(2, "Error creating forks semaphore.\n", 32), ERROR);
	// }
	// printf("creating msg semaphore\n");
	// shared.msg_sem = sem_open(MSG_SEM, O_CREAT | O_EXCL, 0644, 1); // need to check the permissions later 
	// //  create semaphore for locking msg output ----------------------------------------------------------------------------------------------------------------------------------------------------
	// if (shared.msg_sem == SEM_FAILED)  
	// {
	// 	sem_close(shared.fork_sem);
	// 	sem_unlink(FORK_SEM);
	// 	free(pids);
	// 	free(philos);
	// 	return (write(2, "Error creating message semaphore.\n", 34), ERROR);
	// }
	// printf("creating monitoring semaphore\n");
	// shared.monitoring_sem = sem_open(MONITORING_SEM, O_CREAT | O_EXCL, 0644, 1); // need to check the permissions later 
	// // create semaphore for stop_simulation ---------------------------------------------------------------------------------------------------------------------------------------------------------
	// if (shared.monitoring_sem == SEM_FAILED) 
	// {
	// 	sem_close(shared.fork_sem);
	// 	sem_unlink(FORK_SEM);
	// 	sem_close(shared.msg_sem);
	// 	sem_unlink(MSG_SEM);
	// 	free(pids);
	// 	free(philos);
	// 	return (write(2, "Error creating monitoring semaphore.\n", 37), ERROR);
	// } 
	// printf("storing start time\n");
	// shared.start_time = get_time_in_micros(); // or here set to 0 and in code move further 
	if (init_shared_resources(&shared, total) == ERROR)
	{
		free(pids);
		free(philos);
		return (ERROR);
	}
	printf("initializing philo data \n");
	i = -1;
	while (++i < total)
		init_philo_data(&philos[i], i, argv, &shared);
	printf("forking processes\n");
	i = -1;
	// Create child processes ------------------------------------------------------------------------------------------------------------------------------------------------------------------
	while (++i < total)
	{
		pids[i] = fork();
		printf("forked new process %d\n", i);
		if (pids[i] < 0)
		{
			// TODO: exit all created processes and free pids and philos in them  - maybe use the monitoring semaphore for this
			// TODO: free all pids, free all philos
			destroy_semaphores(&shared);
			return (write(2, "Error forking processes.\n", 25), ERROR);
		}
		// Child process functionality ---------------------------------------------------------------------------------------------------------------------------------------------------------
		if (pids[i] == 0)
		{
			printf("in child process\n");
			int	exit_status = child_process(&philos[i]);
			// cleanup child process here because it does not have access ot entire philo array nor entire pids array - because philos should not know the state of other philos in my opinion 
			free(pids);
			free(philos);
			destroy_semaphores(&shared);
			exit(exit_status);
		}
	}
	// monitoring thread -----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	monitor(&philos);
	// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Cleanup
	i = -1;
	// Wait for all child processes to finish 
	while (++i < total)
	{
		waitpid(pids[i], NULL, 0);
	}
	// Close and unlink the semaphore, free allocs
	destroy_semaphores(&shared);
	free(philos);
	free(pids);
	printf("Resources destroyed. Program exiting.\n");
	return (SUCCESS);
}

int	main(int argc, char **argv)
{
	if (handle_error_input(argc, argv) == ERROR)
		return (ERROR);
	if (start_simulation(argc, argv, ft_atou(argv[1])) == ERROR)
		return (ERROR);
	return (0);
}