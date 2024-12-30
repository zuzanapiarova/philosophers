/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 15:30:20 by zuzanapiaro       #+#    #+#             */
/*   Updated: 2024/12/30 00:43:41 by zuzanapiaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

long long get_time_in_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == 0)
		return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (ERROR);
}

// use mutex to lock access to terminal while one log msg is being printed out from another thread wanting to print a msg
int	log_msg(t_info *info, t_philo *philo, t_action action)
{
	(void)info;
	ft_putnbr(get_time_in_ms());
	write(1, " ", 1);
	ft_putnbr(philo->id);
	write(1, " ", 1);
	if (action == FORK)
		write(1, " has taken a fork\n", 18);
	else if (action == EATS)
	{
		write(1, " is eating ", 11);
		ft_putnbr(philo->times_eaten);
		write(1, ". time\n", 7);
	}
	else if (action == THINKS)
		write(1, " is thinking\n", 13);
	else if (action == SLEEPS)
		write(1, " is sleeping\n", 13);
	else if (action == DEATH)
		write(1, " died\n", 6);
	return (0);
}

void init_info(t_info *info, int argc, char **argv)
{
	info->total = ft_atou(argv[1]);
	info->die = ft_atou(argv[2]);
	info->eat = ft_atou(argv[3]);
	info->sleep = ft_atou(argv[4]);
	if (argc == 6)
		info->times_to_eat = ft_atou(argv[5]);
	else
		info->times_to_eat = -1;
	info->death = false;
	info->forks = NULL;
	info->philos = NULL;
}

t_philo *init_philo(t_info *info, int id)
{
	t_philo			*philo;

	philo = (t_philo *)malloc(sizeof(t_philo));
	philo->id = id;
	if (pthread_create(&philo->thread, NULL, &routine, (void *)info) != 0)
		return (NULL);
	philo->dead = 0;
	philo->thinks = 0;
	philo->sleeps = 0;
	philo->eats = 0;
	philo->times_eaten = 0;
	philo->last_eaten = get_time_in_ms();
	philo->lock = (pthread_mutex_t *)malloc(info->total * sizeof(pthread_mutex_t));
	return (philo);
}

void *monitoring(void *arg) {
    t_info *info = (t_info *)arg;

    while (1)
	{
        for (int i = 0; i < info->total; i++) {
            pthread_mutex_lock(info->philos[i]->lock); // Lock philosopher data
            long current_time = get_time_in_ms();
            if (current_time - info->philos[i]->last_eaten > info->die) {
                log_msg(info, info->philos[i], DEATH);
                pthread_mutex_unlock(info->philos[i]->lock);
                return (NULL); // Exit the monitor thread
            }
            pthread_mutex_unlock(info->philos[i]->lock);
        }
        usleep(1000); // Sleep for a short period to avoid excessive CPU usage
    }
}

void *routine(void *arg)
{
	t_philo	*philo;
	t_info	*info;

	info = (t_info *)arg;
	philo = info->philos[1]; // TODO: replace 1 with the current thread//philo// from the philos array
	sleep(2);
	while ((int)philo->times_eaten < info->times_to_eat)
	{
	// maybe do this in a while loop? condition could be while info->death != true
	// some algorithm for deciding for when to eat and which philosopher should eat
	// check if right and left fork are free
	// if yes grab them and lock them
		take_fork(info, philo);
	// start eating
		p_eat(info, philo);
	// if we have number_of_times_to_eat we decrease it
	// unlock forks/mutexes
		leave_forks(info, philo);
	// start sleeping
		p_sleep(info, philo);
	// start thinking
	// make sure the time without eating has not surpassed its limit
	// if it has, set info->death to true and then loop should not run
	}
	return (NULL);
}

// TODO: add protections of the functions pthread and mutex fail
int	start_simulation(t_info *info)
{
	int			i;
	pthread_t	*monitor;

	// start the monitoring thread and its mutex "philo->lock"
	monitor = (pthread_t *)malloc(sizeof(pthread_t) * info->total);
	if (pthread_create(monitor, NULL, &monitoring, (void *)info) != 0)
		return (ERROR);
	i = 0;
	info->forks = (pthread_mutex_t *)malloc(info->total * sizeof(pthread_mutex_t));
	while (i < info->total)
	{
		pthread_mutex_init(&info->forks[i], NULL);
		i++;
	}
	info->philos = (t_philo **)malloc(sizeof(t_philo *) * info->total);
	if (!info->philos)
		return (1);
	i = 0;
	while (i < info->total)
	{
		info->philos[i] = init_philo(info, i + 1); // i + 1 because they are numbered from one to n
		if (!info->philos[i])
		{
			while (i - 1 > 0)
			{
				pthread_join(info->philos[i]->thread, NULL); // Wait for threads to finish
				free(info->philos[i]); // Free philosopher memory
				i--;
			}
			free(info->philos); // Free the array
			return (ERROR);
		}
		printf("created thread %d\n", info->philos[i]->id);
		i++;
	}
	i = 0;
	while (i < info->total)
	{
		pthread_join(info->philos[i]->thread, NULL);
		free(info->philos[i]); // Free philosopher memory
		i++;
	}
	free(info->philos); // Free the array
	return (0);
}

int main(int argc, char **argv)
{
	t_info			info;

	if (handle_error_input(argc, argv) == 1)
		return (1);
	init_info(&info, argc, argv);
	printf("total: %d\n", info.total);
	if (start_simulation(&info) == 1)
		return (1);
	return (0);
}
