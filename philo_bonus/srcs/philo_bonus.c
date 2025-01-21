/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:05:45 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/21 21:08:24 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// runs for each child with input of one philo struct
// each philo has 2 monitoring threads - one for its death, other for stop_sim
int	child_process(t_philo *philo)
{
	while (1)
	{
		if (check_stop_sim(philo))
			return (ERROR);
		if (philo->times_to_eat == 0)
			return (ERROR);
		if (take_forks(philo) == ERROR)
			return (ERROR);
		if (p_eat(philo) == ERROR)
		{
			leave_forks(philo);
			return (ERROR);
		}
		leave_forks(philo);
		if ((int)philo->times_eaten == philo->times_to_eat)
		{
			//log_msg(philo, FULL);
			sem_post(philo->shared->fullness_sem);
			if (check_stop_sim(philo) == true)
				return (ERROR);
		}
		if (p_sleep(philo) == ERROR)
			return (ERROR);
		if (p_think(philo) == ERROR)
			return (ERROR);
	}
	return (SUCCESS);
}

int	init_child(t_resources *resources, int i)
{
	int result;

	resources->philo = &resources->philos[i];
	resources->philo->mutex_sem_name = get_mutex_sem_name(resources->philo);
	resources->philo->mutex_local_sem = sem_open(resources->philo->mutex_sem_name, O_CREAT | O_EXCL, 0644, 1);
	if (resources->philo->mutex_local_sem == SEM_FAILED)
	{
		free(resources->philo->mutex_sem_name);
		return (write(2, "Error creating forks semaphore.\n", 32), ERROR);
	}
	pthread_create(&resources->philo->stop_sim_checker, NULL, &stop_routine, resources); // will free all resources regarding this process besided itself (obvi)
	pthread_create(&resources->philo->death_checker, NULL, &death_routine, resources->philo);
	if (child_process(resources->philo) == ERROR)
		result = ERROR;
	else	
		result = SUCCESS;
	// cleanup of resources for each process
	pthread_join(resources->philo->death_checker, NULL);
	pthread_join(resources->philo->stop_sim_checker, NULL);
	close_semaphores(resources->shared);
	sem_close(resources->philo->mutex_local_sem);
	sem_unlink(resources->philo->mutex_sem_name);
	free(resources->philo->mutex_sem_name);
	free(resources->philos);
	free(resources->pids);
	return (SUCCESS);
}

int	start_simulation(char **argv, int total)
{
	int			i;
	pid_t		*pids;
	t_philo		*philos;
	t_shared	shared;
	t_resources	resources;
	pthread_t	fullness_checker;

	pids = NULL;
	philos = NULL;
	
	if (init_resources(&philos, &pids, &shared, argv) == ERROR)
		return (ERROR);
	resources.pids = pids;
	resources.philos = philos;
	resources.philo = NULL;
	resources.shared = &shared;
	shared.start_time = get_time_in_micros();
	pthread_create(&fullness_checker, NULL, &fullness_checker_routine, &resources);
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
			int exit_status = init_child(&resources, i);
			exit(exit_status);
		}
	}
	pthread_join(fullness_checker, NULL);
	// Cleanup
	i = -1;
	while (++i < total)
	{
		// TODO: gather exit codes of processes and if they are all 0, return 0, if any is error, return error
		// TODO: its ok because routine(child_process) cannot go wrong, only breaks if we catch death or full, so we return success
		waitpid(pids[i], NULL, 0);
	}
	log_msg(&philos[0],FINISH);
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
