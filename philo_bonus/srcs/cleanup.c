/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 10:43:17 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/20 13:27:19 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

void handle_signal(int sig)
{
    printf("Process exiting due to signal %d\n", sig);
    // resources to cleanup in each process if signal is received
    //sem_destroy(&sem);  // Clean up semaphore
    exit(0);            // Exit the process
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
    sem_close(shared->full_sem);
	sem_unlink(FULL_SEM);
	return (SUCCESS);
}