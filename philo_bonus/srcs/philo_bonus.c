/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:05:45 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/17 17:30:16 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// this thread can communicate with the others 
// only waits to receive "signal" from stop_sem
// stop_sem is a semaaphore signalling that the simulation is over
// then it sets stop_simulation to true in THIS process
// further functions and threads will work with this variable
void	*stop_routine(void	*arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	printf("started stop_checker thread\n");
	sem_wait(philo->shared->stop_sem);
	sem_post(philo->shared->stop_sem);
	pthread_mutex_lock(&philo->lock);
	*(philo->shared->stop_simulation) = true;
	pthread_mutex_unlock(&philo->lock);
	return (NULL);
}

// runs constantly until philo dies
// if he dies, prints DEATH msg and exits 
// if stop_simulation is true, exits
void	*death_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	printf("started death_checker thread\n");
	while (1)
	{
		pthread_mutex_lock(&philo->lock);
		if (get_time_in_micros() - philo->last_eaten 
			> (philo->die * 1000) || *(philo->shared->stop_simulation))
		{
			if (*(philo->shared->stop_simulation) != true)
				log_msg(philo, DEATH);
			pthread_mutex_unlock(&philo->lock);
			int i = 0;
			while (i < philo->total)
			{
				sem_post(philo->shared->monitoring_sem);
				i++;
			}
			return (NULL);
		}
		pthread_mutex_unlock(&philo->lock);
		usleep(1000);
	}
}

// runs for each child - derivative of a routine for philosopher thread
// receives one philo struct as parameter
int	child_process(t_philo *philo)
{
	pthread_mutex_init(&philo->lock, NULL);
	pthread_create(&philo->stop_sim_checker, NULL, &stop_routine, philo);
	pthread_create(&philo->death_checker, NULL, &death_routine, philo);
	while (1)
	{
		if (philo->times_to_eat == 0)
			break ;
		if (take_forks(philo) == ERROR)
			break ;
		if (p_eat(philo) == ERROR)
		{
			sem_post(philo->shared->fork_sem);
			sem_post(philo->shared->fork_sem);
			break ;
		}
		leave_forks(philo);
		if ((int)philo->times_eaten == philo->times_to_eat)
		{
			sem_post(philo->shared->monitoring_sem);
			log_msg(philo, FULL);
		}
		if (p_sleep(philo) == ERROR)
			break ;
		if (p_think(philo) == ERROR)
			break ;
	}
	pthread_join(philo->stop_sim_checker, NULL);
	pthread_join(philo->death_checker, NULL);
	pthread_mutex_destroy(&philo->lock);
	return (SUCCESS);
}

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
		return (write(2, "Error creating monitoring semaphore.\n", 37), ERROR);
	}
	shared->start_time = 0;
	*(shared->stop_simulation) = false;
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
	sem_close(shared->stop_sem);
	sem_unlink(STOP_SEM);
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

// there are no shared resources as the memory for each process is duplicated, not shared even for pointers
int	start_simulation(char **argv, int total)
{
	int			i;
	pid_t		*pids;
	t_philo		*philos;
	t_shared	shared;

	pids = NULL;
	philos = NULL;
	if (init_resources(&philos, &pids, &shared, argv) == ERROR)
		return (ERROR);
	shared.start_time = get_time_in_micros(); // or here set to 0 and in code move further 
	// Create child processes ------------------------------------------------------------------------------------------------------------------------------------------------------------------
	printf("forking processes\n");
	i = -1;
	while (++i < total)
	{
		pids[i] = fork();
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
			printf("forked child %d\n", i);
			int	exit_status = child_process(&philos[i]);
			// cleanup child process here because it does not have access ot entire philo array nor entire pids array - because philos should not know the state of other philos in my opinion 
			free(pids);
			free(philos);
			//destroy_semaphores(&shared);
			sem_close(shared.fork_sem);
			sem_close(shared.msg_sem);
			sem_close(shared.stop_sem);
			sem_close(shared.monitoring_sem);
			printf("Resources destroyed. Child %d exiting.\n", i);
			exit(exit_status);
		}
	}
	// monitoring process ??? ------------------------------------------------------------------------------------------------------------------------------------------------------------------
	i = 0;
	while (i < philos[0].total) // & -------------------------------- wait for collecting output from all philos for being full --- OR --- output from one of them that he died ----------------
	{
		sem_wait(philos[0].shared->monitoring_sem);
		printf("waited for p%d\n", i + 1);
		i++;
	}
	printf("all of them are");
	 // when the monitoring semaphore finished waiting and reaches here, it means simulation should stop
	sem_post(philos[0].shared->stop_sem);
	// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Cleanup
	i = -1;
	while (++i < total)
	{
		// TODO: gather exit codes of processes and if they are all 0, return 0, if any is error, return error
		// TODO: its ok because routine(child_process) cannot go wrong, only breaks if we catch death or full, so we return success
		waitpid(pids[i], NULL, 0);
	}
	destroy_semaphores(&shared);
	free(philos);
	free(pids);
	printf("Resources destroyed. Parent exiting.\n");
	return (SUCCESS);
}

int	main(int argc, char **argv)
{
	if (handle_error_input(argc, argv) == ERROR)
		return (ERROR);
	if (start_simulation(argv, ft_atou(argv[1])) == ERROR)
		return (ERROR);
	return (SUCCESS);
}
