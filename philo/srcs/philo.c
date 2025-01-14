/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 08:40:25 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/14 13:01:20 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// inits forks, mutexes, and threads
// handles possible errors 
// cleans up roperly after the program is finished 
int	start_simulation(int argc, char **argv, int total)
{
	t_philo		*philos;
	t_shared	shared;
	pthread_t	monitor;

	(void)argc;
	philos = NULL;
	if (init_shared_resources(&shared, total) == ERROR)
		return (ERROR);
	if (allocate_philo_array(&philos, &shared, total) == ERROR)
		return (ERROR);
	if (fill_philo_array(philos, &shared, argv, total) == ERROR)
		return (ERROR);
	if (init_monitor(&monitor, philos, &shared, total) == ERROR)
		return (ERROR);
	if (cleanup(philos, &shared, &monitor, total) == ERROR)
		return (ERROR);
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
