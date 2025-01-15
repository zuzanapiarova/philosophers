/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:05:45 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/15 21:02:13 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// TODO: monitoring process
// TODO: do i need any algo to allow fair access to resources like sleepng for a bit in threads?
// TODO: log messages are sometimes mixed up - possibly implement a binary semaphore for access to stdout ? 
// TODO: implement the actions 
int	child_process(t_philo *philo, sem_t *semaphore)
{
	while (1)
	{
		// Wait (decrement semaphore)
		sem_wait(semaphore);
		log_msg(philo, FORK_L);
		sem_wait(semaphore);
		log_msg(philo, FORK_R);
		// Critical section
		p_eat(philo);
		sem_post(semaphore);
		sem_post(semaphore);
		if (philo->times_to_eat != -1 && (int)philo->times_eaten >= philo->times_to_eat)
		{
			philo->finished = true;
			log_msg(philo, FINISH);
			return (ERROR);
		}
		p_sleep(philo);
		p_think(philo);
		// TODO: change death checking, for now it is after each actions iteration
		if (get_time_in_micros() - philo->last_eaten > (philo->die * 1000) && !philo->finished)
		{
			//*(philos[i].stop_simulation) = true;
			log_msg(philo, DEATH);
			return (ERROR);
		}
	}
	printf("Resources destroyed in %d. Program exiting.\n", philo->id);
	// must clean all resources - mallocs and sems before exiting 
	return (SUCCESS); // Exit child process
}

int	monitor(t_philo **philos, sem_t *semaphore)
{
	return (SUCCESS);
}

int	init_shared_resources(t_shared *shared, int total)
{
	(void)total;
	shared->start_time = get_time_in_micros(); // or here set to 0 and in code move further 
	// ? shared->stop_simulation = false;
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
	p->finished = false;
	// shared (value, not in memory): 
	p->start_time = shared->start_time;
	// ? p->stop_simulation = &shared->stop_simulation;
	return (SUCCESS);
}

// there are no shared resources as the memory for each process is duplicated, not shared even for pointers
int	start_simulation(int argc, char **argv, int total)
{
	int			i;
	pid_t		*pids;
	t_philo		*philos;
	sem_t		*semaphore;
	t_shared	shared;

	i = -1;
	// create pids array -----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	pids = (pid_t *)malloc(sizeof(pid_t) * total);
	if (!pids)
		return (write(2, "Memory allocation problem. Exiting.\n", 36), ERROR);
	// create semaphore ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	semaphore = sem_open(SEM, O_CREAT | O_EXCL, 0644, total); // need to check the permissions later 
	if (semaphore == SEM_FAILED) 
	{
		free(pids);
		perror("error");
		return (write(2, "Error creating semaphore.\n", 26), ERROR);
	}
	// create philo structures - alloc ---------------------------------------------------------------------------------------------------------------------------------------------------------
	philos = (t_philo *)malloc(sizeof(t_philo) * total);
	if (!philos)
	{
		sem_close(semaphore);
        sem_unlink(SEM);
		free(pids);
		return (write(2, "Memory allocation problem. Exiting.\n", 36), ERROR);
	}
	// init shared struct, init philos structures, and pass them the shared struct -------------------------------------------------------------------------------------------------------------
	init_shared_resources(&shared, total);
	i = -1;
	while (++i < total)
		init_philo_data(&philos[i], i, argv, &shared);
	i = -1;
	// Create child processes ------------------------------------------------------------------------------------------------------------------------------------------------------------------
	while (++i < total)
	{
		pids[i] = fork();
		if (pids[i] < 0)
		{
			// TODO: exit all created processes
			// TODO: free all pids, free all philos
			sem_close(semaphore);
			sem_unlink(SEM);
			return (write(2, "Error forking processes.\n", 25), ERROR);
		}
		// Child process functionality ---------------------------------------------------------------------------------------------------------------------------------------------------------
		if (pids[i] == 0)
		{
			child_process(&philos[i], semaphore);
			// cleanup child process here because it does not have access ot entire philo array nor entire pids array - because philos should not know the state of other philos in my opinion 
			free(pids);
			free(philos);
			sem_close(semaphore);
			sem_unlink(SEM);
			exit(SUCCESS);
		}
	}
	// monitoring thread -----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	monitor(&philos, semaphore);
	// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Cleanup
	i = -1;
	// Wait for all child processes to finish 
	while (++i < total)
	{
		waitpid(pids[i], NULL, 0);
	}
	// Close and unlink the semaphore, free allocs
	sem_close(semaphore);
	sem_unlink(SEM);
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