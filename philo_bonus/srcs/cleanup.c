/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 10:43:17 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/23 11:11:02 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// destorys local resources - localy created threads, semaphore, local sem name
int	destroy_local_res(t_philo *philo)
{
	pthread_join(philo->death_checker, NULL);
	pthread_join(philo->stop_sim_checker, NULL);
	sem_close(philo->mutex_local_sem);
	sem_unlink(philo->mutex_sem_name);
	free(philo->mutex_sem_name);
	return (SUCCESS);
}

// destroys global resources in parent = both closes and unlinks semaphores
int	destroy_glob_res_parent(t_philo *ph, t_shared *shared, pid_t *pids)
{
	destroy_global_semaphores(shared);
	free(ph);
	free(pids);
	return (SUCCESS);
}

// destroys global resources in child = only closes semaphores, does not unlink
int	destroy_glob_res_child(t_philo *ph, t_shared *shared, pid_t *pids)
{
	close_global_semaphores(shared);
	free(ph);
	free(pids);
	return (SUCCESS);
}

// closes globaly declared named semaphores, does not unlink
int	close_global_semaphores(t_shared *shared)
{
	sem_close(shared->fork_sem);
	sem_close(shared->msg_sem);
	sem_close(shared->stop_sem);
	sem_close(shared->fullness_sem);
	return (SUCCESS);
}

// closes and unlinks globaly declared named semaphores
int	destroy_global_semaphores(t_shared *shared)
{
	sem_close(shared->fork_sem);
	sem_unlink(FORK_SEM);
	sem_close(shared->msg_sem);
	sem_unlink(MSG_SEM);
	sem_close(shared->fullness_sem);
	sem_unlink(FULLNESS_SEM);
	sem_close(shared->stop_sem);
	sem_unlink(STOP_SEM);
	return (SUCCESS);
}
