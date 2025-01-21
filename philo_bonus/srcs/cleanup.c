/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 10:43:17 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/21 20:43:26 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int	close_semaphores(t_shared *shared)
{
	sem_close(shared->fork_sem);
	sem_close(shared->msg_sem);
	sem_close(shared->stop_sem);
	sem_close(shared->fullness_sem);
	return (SUCCESS);
}

int	destroy_semaphores(t_shared *shared)
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
