/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 15:30:20 by zuzanapiaro       #+#    #+#             */
/*   Updated: 2025/01/03 20:21:50 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// !!! TODO: 1 always dies the last time before eating for the last time 
// TODO: implement mutexes oroperly, now they allow two resources access to the shared mutex - fork
// TODO: implement better monitoring
// TODO: implement stopping as soon as one dies
// TODO: the time is probably not in or timezone but the UTC- fix!
// TODO: now we do actions functions but there is not lock between their loc, so it can happpen that the thread can do one line like start to eat, and then not print the msg straight forward but only after a while - implement another lock?
// Q&A: What is the starting state for a philosopher?
// Q&A: Does the philospher stop eating-let forks-start sleeping right after each other?
// Q&A: How long does each philosopher have to think? Can they stop sleeping, start thinking and then take forks and start eating right away, having thinked for no time ???
// Q&A: When the philo eats for the last time, does its thread wait until the others finish before joining or it can go straight to joining straight away? Even though this is not it its routine? So it returns and then what? It continues in the calling function?
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
		p_sleep(philo);
	// start thinking
		log_msg(philo, THINKS);
	// make sure the time without eating has not surpassed its limit
	// if it has, set info->death to true and then loop should not run
	}
	printf("finished loop in thread %d\n", philo->id);
	return (NULL);
}

// TODO: add protections if the functions pthread and mutex fail
int	start_simulation(int argc, char **argv, int total)
{
	int				i;
	t_philo			*philos;
	pthread_mutex_t *forks;
	pthread_t		monitor;
	pthread_mutex_t	msg_lock;

	(void)argc;
	// creating forks
	i = 0;
	forks = (pthread_mutex_t *)malloc(total * sizeof(pthread_mutex_t));
	while (i < total)
	{
		pthread_mutex_init(&forks[i], NULL);
		i++;
	}
	// creating shared central lock for stdout when printing a message 
	pthread_mutex_init(&msg_lock, NULL);
	// creating philos
	philos = (t_philo *)malloc(sizeof(t_philo) * total);
	if (!philos)
		return (ERROR);
	i = 0;
	while (i < total)
	{
		//printf("created thread %d\n", philos[i].id);
		if (init_philo(&philos[i], i, argv, &forks, &msg_lock) == ERROR)
		{
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
	// start monitoring thread
	if (pthread_create(&monitor, NULL, &monitoring, (void *)philos) != 0)
		return (ERROR);
	printf("started monitoring thread\n");
	i = 0;
	while (i < total)
	{
		pthread_join(philos[i].thread, NULL);
		pthread_mutex_destroy(&forks[i]);
		pthread_mutex_destroy(&philos[i].lock);
		i++;
	}
	printf("joined philo threads\n");
	pthread_mutex_destroy(&msg_lock);
	printf("destroyed msg mutex\n");
	pthread_join(monitor, NULL);
	printf("joined monitor thread\n");
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
