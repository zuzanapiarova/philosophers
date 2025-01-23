/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 08:40:37 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/23 13:04:12 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/philosophers.h"

// checks if stop_simulation is true meaning one of them died
// or checks if the specific philo finished eating
// if either is true, returns true and caller will stop the specific thread
bool	check_stop_sim(t_philo *philo)
{
	bool	stopped;

	sem_wait(philo->mutex_local_sem);
	stopped = philo->stop_simulation;
	sem_post(philo->mutex_local_sem);
	return (stopped);
}

// gets specific name for local philo mutex-representing semaphore
char	*get_mutex_sem_name(t_philo *philo)
{
	char	*name;
	char	*number;

	number = ft_utoa(philo->id);
	name = ft_strjoin("mutex_sem", number);
	free(number);
	return (name);
}

// closes semaphores from arguments, if exist
int	sem_close_helper(sem_t *sem1, sem_t *sem2, sem_t *sem3)
{
	if (sem1)
		sem_close(sem1);
	if (sem2)
		sem_close(sem2);
	if (sem3)
		sem_close(sem3);
	return (SUCCESS);
}

// unlinks semaphores by their name from arguments, if exist
int	sem_unlink_helper(char *sem_name1, char *sem_name2, char *sem_name3)
{
	if (sem_name1)
		sem_unlink(sem_name1);
	if (sem_name2)
		sem_unlink(sem_name2);
	if (sem_name3)
		sem_unlink(sem_name3);
	return (SUCCESS);
}

// terminates processes forked so far if fork error is encountered
int	handle_fork_error(pid_t *pids, int i)
{
	while (--i)
		kill(pids[i], SIGTERM);
	return (write(2, "Error forking processes.\n", 25), ERROR);
}
