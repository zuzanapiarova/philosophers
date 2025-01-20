/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:05:45 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/20 13:42:13 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// after each action, check if stop_simulation is true
// if sop_sim is true, do not do anything else, just exit
// after each meal check if full, if yes, post to fullness monitoring semaphore
int	routine(t_philo *philo)
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
		log_msg(philo, FULL);
		sem_post(philo->shared->monitoring_sem);
	}
	if (p_sleep(philo) == ERROR)
		return (ERROR);
	if (p_think(philo) == ERROR)
		return (ERROR);
	return (SUCCESS);
}

// runs for each child with input of one philo struct
// each philo has 2 monitoring threads - one for its death, other for stop_sim
int	child_process(t_philo *philo)
{
	pthread_mutex_init(&philo->lock, NULL);
	pthread_create(&philo->stop_sim_checker, NULL, &stop_routine, philo);
	// pthread_create(&philo->full_checker, NULL, &full_routine, philo);
	pthread_create(&philo->death_checker, NULL, &death_routine, philo);
	while (1)
	{
		if (routine(philo) == ERROR)
		{
			pthread_join(philo->stop_sim_checker, NULL);
			// pthread_join(philo->full_checker, NULL);
			pthread_join(philo->death_checker, NULL);
			pthread_mutex_destroy(&philo->lock);
			return (ERROR);
		}
	}
	pthread_join(philo->stop_sim_checker, NULL);
	// pthread_join(philo->full_checker, NULL);
	pthread_join(philo->death_checker, NULL);
	pthread_mutex_destroy(&philo->lock);
	return (SUCCESS);
}

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
			sem_close(shared.full_sem);
			sem_close(shared.monitoring_sem);
			printf("Resources destroyed. Child %d exiting.\n", i);
			exit(exit_status);
		}
	}
	// monitoring process ??? ------------------------------------------------------------------------------------------------------------------------------------------------------------------
	i = 0;
	while (i < philos[0].total) // & -------------------------------- wait for collecting signals from all philos for being full --- OR --- signal from one of them that he died ---------------
	{
		sem_wait(philos[0].shared->monitoring_sem);
		printf("waited for p%d\n", i + 1);
		i++;
	}
	log_msg(&philos[0],FINISH);
	 // when the monitoring semaphore finished waiting and reaches here, it means simulation should stop
	i = 0;
	while (i < philos[0].total) // & -------------------------------- post n times to semaphore so all processes get the memo that simulation is over ------------------------------------------
	{
		sem_post(philos[0].shared->stop_sem);
		i++;
	}
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
