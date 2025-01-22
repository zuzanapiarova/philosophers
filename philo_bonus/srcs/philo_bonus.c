/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:05:45 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/22 19:09:21 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// runs for each child with input of one philo struct
// each philo has 2 monitoring threads - one for its death, other for stop_sim
void	child_routine(t_philo *philo)
{
	while (1)
	{
		if (philo->id % 2 == 0)
			usleep(10);
		if (check_stop_sim(philo))
			break ;
		if (philo->times_to_eat == 0)
			break ;
		if (take_forks(philo) == ERROR)
			break ;
		if (p_eat(philo) == ERROR)
			break ;
		leave_forks(philo);
		if ((int)philo->times_eaten == philo->times_to_eat)
		{
			sem_post(philo->shared->fullness_sem);
			usleep(500);
		}
		if (check_stop_sim(philo) == true)
			break ;
		if (p_sleep(philo) == ERROR)
			break ;
		if (p_think(philo) == ERROR)
			break ;
	}
}

int	start_simulation(t_philo *philos, t_shared *shared, pid_t *pids, int total)
{
	int			i;
	long long	start_time;

	i = -1;
	start_time = get_time_in_micros();
	while (++i < total)
	{
		pids[i] = fork();
		if (pids[i] < 0)
			return (handle_fork_error(pids, i));
		if (pids[i] == 0)
		{
			philos[i].start_time = start_time;
			if (init_local_resources(&philos[i]) == ERROR)
				return (ERROR);
			child_routine(&philos[i]);
			destroy_local_resources(&philos[i]);
			destroy_global_resources_child(philos, shared, pids);
			exit(SUCCESS);
		}
	}
	if (run_full_checker_thread(&philos[0]) == ERROR)
		return (ERROR);
	return (SUCCESS);
}

int	main(int argc, char **argv)
{
	int			total;
	pid_t		*pids;
	t_philo		*philos;
	t_shared	shared;
	
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
	destroy_global_resources_parent(philos, &shared, pids);
	return (SUCCESS);
}
