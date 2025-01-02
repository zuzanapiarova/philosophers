/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 15:30:20 by zuzanapiaro       #+#    #+#             */
/*   Updated: 2025/01/02 20:50:39 by zuzanapiaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TODO: implement mutexes oroperly, now they allow two resources access to the shared mutex - fork
// TODO: implement better monitoring
// TODO: implement stopping as soon as one dies
#include "../includes/philosophers.h"

int init_philo(t_philo *philo, int i, char **argv, pthread_mutex_t **forks)
{
	philo->id = i + 1;
	philo->total = ft_atou(argv[1]);
	philo->die = ft_atou(argv[2]);
	philo->eat = ft_atou(argv[3]);
	philo->sleep = ft_atou(argv[4]);
	if (argv[5])//(argc == 6)
		philo->times_to_eat = ft_atou(argv[5]);
	else
		philo->times_to_eat = -1;
	philo->dead = false;
	philo->forks = *forks;
	philo->thinks = 0;
	philo->sleeps = 0;
	philo->eats = 0;
	philo->times_eaten = 0;
	philo->last_eaten = get_time_in_ms();
	pthread_mutex_init(&philo->lock, NULL);
	pthread_mutex_init(&philo->msg_lock, NULL);
	// philo->lock = (pthread_mutex_t *)malloc(philo->total * sizeof(pthread_mutex_t));
	// philo->msg_lock = (pthread_mutex_t *)malloc(philo->total * sizeof(pthread_mutex_t));
	if (pthread_create(&philo->thread, NULL, &routine, (void *)philo) != 0)
		return (ERROR);
	return (SUCCESS);
}

void *monitoring(void *arg)
{
    t_philo *philo;
	int		i;

	philo = (t_philo *)arg;
	i  = 0;
    while (1)
	{
        while (i < philo->total)
		{
            pthread_mutex_lock(&philo->lock); // Lock philosopher data
            long current_time = get_time_in_ms();
            if (current_time - philo->last_eaten > philo->die)
			{
                log_msg(philo, DEATH);
				pthread_mutex_unlock(&philo->lock);
				return (NULL); // Exit the monitor thread
            }
            pthread_mutex_unlock(&philo->lock);
        }
        usleep(1000); // Sleep for a short period to avoid excessive CPU usage
		i++;
    }
}

void *routine(void *arg)
{
	t_philo *philo;

	philo = (t_philo *)arg;
	while (philo->times_to_eat == -1 || (int)philo->times_eaten < philo->times_to_eat) // if there is no times_to_eat defined, it will be -1 and never increased so condition will be always true
	{
	// some algorithm for deciding for when to eat and which philosopher should eat
	// check if right and left fork are free
	// if yes grab them and lock them
	take_forks(philo);
	// start eating
		p_eat(philo);
	// unlock forks/mutexes
		leave_forks(philo);
	// start sleeping
	//	p_sleep(philo);
	// start thinking
	// make sure the time without eating has not surpassed its limit
	// if it has, set info->death to true and then loop should not run
	}
	return (NULL);
}

// TODO: add protections of the functions pthread and mutex fail
int	start_simulation(int argc, char **argv, int total)
{
	int				i;
	t_philo			*philos;
	pthread_mutex_t *forks;

	(void)argc;
	// start the monitoring thread and its mutex "philo->lock"
	/* pthread_t	*monitor;
	monitor = (pthread_t *)malloc(sizeof(pthread_t) * info->total);
	if (pthread_create(monitor, NULL, &monitoring, (void *)info) != 0)
		return (ERROR); */
	i = 0;
	forks = (pthread_mutex_t *)malloc(total * sizeof(pthread_mutex_t));
	while (i < total)
	{
		printf("created fork %d\n", i);
		pthread_mutex_init(&forks[i], NULL);
		i++;
	}
	philos = (t_philo *)malloc(sizeof(t_philo) * total);
	if (!philos)
		return (ERROR);
	i = 0;
	while (i < total)
	{
		printf("created thread %d\n", philos[i].id);
		if (init_philo(&philos[i], i, argv, &forks) == ERROR)
		{
			printf("hi\n");
			i--;
			while (i > 0)
			{
				pthread_join(philos[i].thread, NULL);
				i--;
			}
			free(philos);
			return (ERROR);
		}
		i++;
	}
	printf("finished creation\n");
	i = 0;
	while (i < total)
	{
		pthread_join(philos[i].thread, NULL);
		//pthread_mutex_destroy(&forks[i]);
		// pthread_mutex_destroy(philos[i].lock);
		// pthread_mutex_destroy(philos[i].fork_lock);
		i++;
	}
	free(philos);
	return (0);
}

int main(int argc, char **argv)
{
	if (handle_error_input(argc, argv) == ERROR)
		return (ERROR);
	if (start_simulation(argc, argv, ft_atou(argv[1])) == ERROR)
		return (ERROR);
	return (0);
}
