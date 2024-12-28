/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 15:30:20 by zuzanapiaro       #+#    #+#             */
/*   Updated: 2024/12/28 08:36:37 by zuzanapiaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// use mutex to lock access to terminal while one log msg is being printed out from another thread wanting to print a msg
void log_msg(t_info *info, int id, t_action action)
{
	struct timeval tv;
	char *id_str;

	(void)action;
	(void)info;
	(void)id;
	if (gettimeofday(&tv, NULL) == 0)
	{
		printf("time: %d\n", tv.tv_usec / 1000);
		// write(1, ..., ...);
	}
	write (1, " ", 1);
	id_str = ft_utoa(id);
	write(1, id_str, ft_strlen(id_str));
	free(id_str);
	if (action == FORK)
		write(1, " has taken a fork\n", 18);
	else if (action == EATS)
		write(1, " is eating\n", 11);
	else if (action == THINKS)
		write (1, " is thinking\n", 13);
	else if (action == SLEEPS)
		write(1, " is sleeping\n", 13);
    else if (action == DEATH)
		write(1, " died\n", 6);
}

void init_info(t_info *info, int argc, char **argv)
{
	info->total = ft_atou(argv[1]);
	info->die = ft_atou(argv[2]);
	info->eat = ft_atou(argv[3]);
	info->sleep = ft_atou(argv[4]);
	if (argc == 6)
		info->times_to_eat = ft_atou(argv[5]);
}

int handle_error_input(int argc, char **argv)
{
	int i;
	int flag;
	int val;

	if (argc != 5 && argc != 6)
	{
		write(1, "Program expects arguments. Please call ./philo with:\n", 53);
		write(1, "\tnumber_of_philosophers\n", 24);
		write(1, "\ttime_to_die (ms)\n", 18);
		write(1, "\ttime_to_eat (ms)\n", 18);
		write(1, "\ttime_to_sleep (ms)\n", 20);
		write(1, "\toptional [number_of_times_each_philosopher_must_eat] (ms)\n", 60);
		return (1);
	}
	// TODO: protect against numbers bigger than MAX_INT*2 and the addition of ms arguments to current time being bigger than max int *2, aleo against numbers smaller than 0
	// watch video on unsigned int
	i = 1;
	flag = 0;
	while (argv[i])
	{
		val = ft_atou(argv[i]);
		//printf("argv: %s, val: %u\n", argv[i], val);
		if (ft_atou(argv[i]) < 0 || ft_strncmp(argv[i], ft_utoa(val), ft_strlen(argv[i])))
		{
			if (flag == 0)
			{
				write(1, "Arguments to program must be positive integer values.\n", 54);
				flag = 1;
			}
			write(1, "--> change argument:\t", 22);
			write(1, argv[i], ft_strlen(argv[i]));
			write(1, "\n", 1);
		}
		if (flag == 0 && i == 1 && val < 1)
			return (write(1, "Number of philosophers must be at least 1.\n", 43), 1);
		i++;
	}
	if (flag == 1)
		return (1);
	// TODO: protect argumets against being zero ?
	return (0);
}

void *routine()
{
	printf("hi\n");
	sleep(2);
	return (NULL);
}

t_philo *init_philo(t_info *info, int id)
{
	t_philo *philo;

	(void)info;
	philo = NULL;
	philo->id = id;
	if (pthread_create(&philo->thread, NULL, &routine, NULL) == 0)
		return (NULL);
	philo->dead = 0;
	philo->thinks = 0;
	philo->sleeps = 0;
	philo->eats = 0;
	return (philo);
}

int start_simulation(t_info *info)
{
	int i;
	t_philo **philos;
	pthread_mutex_t* forks;

	i = 0;
	forks = malloc(info->total * sizeof(pthread_mutex_t));
	while (i < info->total)
	{
		pthread_mutex_init(&forks[i], NULL);
		i++;
	}
	philos = (t_philo *)malloc(sizeof(t_philo *) * info->total);
	if (!philos)
		return (1);
	i = 0;
	while (i < info->total)
	{
		philos[i] = init_philo(info, i + 1); // i + 1 because they are numbered from one to n
		if (!philos[i])
			return (1); // TODO: free the array both before and after the wrong el
	}
	printf("philo id: %d\n", philos[3]->id);
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
	//log_msg(&info, 2, FORK);
	return (0);
}
