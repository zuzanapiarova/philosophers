/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 15:30:20 by zuzanapiaro       #+#    #+#             */
/*   Updated: 2024/12/30 15:08:43 by zuzanapiaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

void *myturn(void *arg)
{
	(void)arg;
	int i = 0;
	while (i < 10)
	{
		write(1, "A ", 2);
		ft_putnbr(i);
		write(1, "\n", 1);
		sleep(1);
		i++;
	}
	return NULL;
}

void *yourturn()
{
	int i = 0;
	while (i < 3)
	{
		write(1, "B ", 2);
		ft_putnbr(i);
		write(1, "\n", 1);
		sleep(2);
		i++;
	}
	return NULL;
}

void *turns(void *arg)
{
	t_philo *philo = (t_philo *)arg;
	int i = 0;

	while (i < philo->total)
	{
		printf("Hello from thread %d ", (int)philo->id);
		printf("%d\n", philo->total);
		sleep(2);
		i++;
	}
	return (NULL);
}

// void init_info(t_info *info, int argc, char **argv)
// {
// 	//
// }

t_philo *init_philo(int i, int argc, char **argv)
{
	t_philo			*philo;

	philo = (t_philo *)malloc(sizeof(t_philo));
	philo->id = i + 1;
	philo->total = ft_atou(argv[1]);
	philo->die = ft_atou(argv[2]);
	philo->eat = ft_atou(argv[3]);
	philo->sleep = ft_atou(argv[4]);
	if (argc == 6)
		philo->times_to_eat = ft_atou(argv[5]);
	else
		philo->times_to_eat = -1;
	philo->death = false;
	philo->forks = NULL;
	philo->dead = 0;
	philo->thinks = 0;
	philo->sleeps = 0;
	philo->eats = 0;
	philo->times_eaten = 0;
	philo->last_eaten = get_time_in_ms();
	//philo->lock = (pthread_mutex_t *)malloc(info->total * sizeof(pthread_mutex_t));
	if (pthread_create(&philo->thread, NULL, &turns, (void *)philo) != 0)
		return (NULL);
	return (philo);
}

void *monitoring(void *arg) {
    t_philo *philo = (t_philo *)arg;

    while (1)
	{
        for (int i = 0; i < philo->total; i++) {
            pthread_mutex_lock(philo->lock); // Lock philosopher data
            long current_time = get_time_in_ms();
            if (current_time - philo->last_eaten > philo->die) {
                log_msg(philo, DEATH);
                pthread_mutex_unlock(philo->lock);
                return (NULL); // Exit the monitor thread
            }
            pthread_mutex_unlock(philo->lock);
        }
        usleep(1000); // Sleep for a short period to avoid excessive CPU usage
    }
}

void *routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	while ((int)philo->times_eaten < philo->times_to_eat)
	{
	// maybe do this in a while loop? condition could be while info->death != true
	// some algorithm for deciding for when to eat and which philosopher should eat
	// check if right and left fork are free
	// if yes grab them and lock them
		take_fork(philo);
	// start eating
		p_eat(philo);
	// if we have number_of_times_to_eat we decrease it
	// unlock forks/mutexes
		leave_forks(philo);
	// start sleeping
		p_sleep(philo);
	// start thinking
	// make sure the time without eating has not surpassed its limit
	// if it has, set info->death to true and then loop should not run
	}
	return (NULL);
}

// TODO: add protections of the functions pthread and mutex fail
int	start_simulation(int argc, char **argv, int total)
{
	int			i;
	//pthread_t	*monitor;
	t_philo		**philos;

	/* pthread_t a;
	if (pthread_create(&a, NULL, myturn, NULL) != 0)
		return (ERROR); */
	// start the monitoring thread and its mutex "philo->lock"
	/* monitor = (pthread_t *)malloc(sizeof(pthread_t) * info->total);
	if (pthread_create(monitor, NULL, &monitoring, (void *)info) != 0)
		return (ERROR); */
	/* i = 0;
	info->forks = (pthread_mutex_t *)malloc(info->total * sizeof(pthread_mutex_t));
	while (i < info->total)
	{
		pthread_mutex_init(&info->forks[i], NULL);
		i++;
	} */
	/* pthread_t b;
	if (pthread_create(&b, NULL, yourturn, NULL) != 0)
		return (ERROR); */
	philos = (t_philo **)malloc(sizeof(t_philo *) * total);
	if (!philos)
		return (ERROR);
	i = 0;
	while (i < total)
	{
		philos[i] = init_philo(i, argc, argv); // i + 1 because they are numbered from one to n
		if (!philos[i])
		{
			i--;
			while (i > 0)
			{
				pthread_join(philos[i]->thread, NULL); // Wait for threads to finish
				free(philos[i]); // Free philosopher memory
				i--;
			}
			free(philos); // Free the array
			return (ERROR);
		}
		printf("created thread %d\n", philos[i]->id);
		i++;
	}

	/* pthread_join(a, NULL);
	pthread_join(b, NULL); */
	i = 0;
	while (i < total)
	{
		pthread_join(philos[i]->thread, NULL);
		free(philos[i]); // Free philosopher memory
		i++;
	}
	free(philos); // Free the array
	return (0);
}

int main(int argc, char **argv)
{
	if (handle_error_input(argc, argv) == 1)
		return (ERROR);
	if (start_simulation(argc, argv, ft_atou(argv[1])) == 1)
		return (ERROR);
	return (0);
}
