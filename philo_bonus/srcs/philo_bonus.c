/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:05:45 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/23 16:18:44 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// runs for each child with input of one philo struct
// each philo has 2 monitoring threads - one for its death, other for stop_sim
// sleep after it is full lets the last full thread have enough time to
// get memo that simulation ended
// no return DT because it can only exit when thread signals it death/fullness
void	child_routine(t_philo *philo)
{
	if (philo->id > philo->total / 2 && philo->total > 1)
		usleep((philo->eat / 2) * 1000);
	while (1)
	{
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
			if (init_local_res(&philos[i]) == ERROR)
				return (ERROR);
			child_routine(&philos[i]);
			destroy_local_res(&philos[i]);
			destroy_glob_res_child(philos, shared, pids);
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

	pids = NULL;
	philos = NULL;
	if (handle_error_input(argc, argv) == ERROR)
		return (ERROR);
	total = ft_atou(argv[1]);
	if (init_glob_res(&philos, &pids, &shared, argv) == ERROR)
		return (ERROR);
	if (start_simulation(philos, &shared, pids, total) == ERROR)
	{
		destroy_glob_res_parent(philos, &shared, pids);
		return (ERROR);
	}
	while (--total > -1)
		waitpid(pids[total], NULL, 0);
	destroy_glob_res_parent(philos, &shared, pids);
	return (SUCCESS);
}
