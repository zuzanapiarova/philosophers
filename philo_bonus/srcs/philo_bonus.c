/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:05:45 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/22 17:35:42 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// runs for each child with input of one philo struct
// each philo has 2 monitoring threads - one for its death, other for stop_sim
int	child_routine(t_philo *philo)
{
	if (philo->id % 2 == 0)
		usleep(100);
	while (1)
	{
		if (check_stop_sim(philo))
			return (ERROR);
		if (philo->times_to_eat == 0)
			return (ERROR);
		if (take_forks(philo) == ERROR)
			return (ERROR);
		if (p_eat(philo) == ERROR)
			return (ERROR);
		leave_forks(philo);
		if ((int)philo->times_eaten == philo->times_to_eat)
		{
			sem_post(philo->shared->fullness_sem);
			//usleep(500);
		}
		if (check_stop_sim(philo) == true)
			return (ERROR);
		if (p_sleep(philo) == ERROR)
			return (ERROR);
		if (p_think(philo) == ERROR)
			return (ERROR);
	}
	return (SUCCESS);
}

int	start_simulation(t_philo *philos, t_shared *shared, pid_t *pids, int total)
{
	int			i;
	long long	start_time;
	pthread_t	full_checker;

	start_time = get_time_in_micros();
	i = -1;
	while (++i < total)
	{
		pids[i] = fork();
		if (pids[i] < 0)
		{
			// TODO: exit all created processes and free pids and philos in them  - maybe use the monitoring semaphore for this
			// TODO: free all pids, free all philos
			return (write(2, "Error forking processes.\n", 25), ERROR);
		}
		if (pids[i] == 0)
		{
			philos[i].start_time = start_time;
			init_local_resources(&philos[i]);
			int exit_status = child_routine(&philos[i]);
			destroy_local_resources(&philos[i]);
			destroy_global_resources_child(philos, shared, pids);
			exit(exit_status);
		}
	}
	if (pthread_create(&full_checker, NULL, &full_routine, &philos[0]) == ERROR)
		return (write(2, "Error creating full_check thread.\n", 34), ERROR);
	pthread_join(full_checker, NULL);
	return (SUCCESS);
}

int	main(int argc, char **argv)
{
	pid_t		*pids;
	t_philo		*philos;
	t_shared	shared;
	int			exit_status;
	int			total;

	exit_status = 0;
	total = ft_atou(argv[1]);
	pids = NULL;
	philos = NULL;
	if (handle_error_input(argc, argv) == ERROR)
		return (ERROR);
	if (init_global_resources(&philos, &pids, &shared, argv) == ERROR)
		return (ERROR);
	if (start_simulation(philos, &shared, pids, total) == ERROR)
	{
		destroy_global_resources_parent(philos, &shared, pids);
		return (ERROR);
	}
	while (--total > -1)
		waitpid(pids[total], NULL, 0);
	// TODO: function to collect exit status from exited processes
	destroy_global_resources_parent(philos, &shared, pids);
	return (exit_status);
}
