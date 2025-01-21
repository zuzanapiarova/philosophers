/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 17:26:55 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/21 17:56:35 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <sys/time.h>
# include <pthread.h>
# include <stdbool.h>
# include <string.h>
# include <limits.h>
# include <semaphore.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <signal.h>

# define ERROR 1
# define SUCCESS 0
# define TIMEZONE_MILI 3600000
# define TIMEZONE_MICRO 3600000000
# define FORK_SEM "/fork_sem69"
# define MSG_SEM "/msg_sem69"
# define STOP_SEM "/stop_sem69"
# define FULL_SEM "/stop_sem69"
# define MONITORING_SEM "/monitoring_sem69"

typedef enum e_action
{
	FORK_L, // TOOK LEFT FORK
	FORK_R, // TOOK RIGHT FORK
	EATS, // STARTED EATING
	SLEEPS, // STARTED SLEEPING
	THINKS, // STARTED THINKING
	DEATH, // DIED
	FINISH, // SIMULATION IS FINISHED
	FULL, // PHILO ATE FOR THE LAST TIME
	STOP, // TEST - SIMULATION IS STOPPED
	LAST, // TEST - SEMAPHORE SIGNAL FOR DEATH WA SSENT
	RECEIVED, // TEST - CHECKS IF STOP SIGNAL WAS RECEIVED
	STOP_STATUS // TEST - shows value of stop_simulation at each check
}				t_action;

typedef struct s_shared
{
	long long		start_time;
	sem_t			*fork_sem;
	sem_t			*msg_sem;
	sem_t			*monitoring_sem;
	sem_t			*stop_sem;
	sem_t			*full_sem;
}					t_shared;

typedef struct s_philo
{
	// variables from user input that are static
	int				id;
	int				total;
	unsigned int	eat;
	unsigned int	sleep;
	unsigned int	die;
	int				times_to_eat; // int because is -1 if not assigned
	// variables that change but belong to one thread at all times
	unsigned int	times_eaten;
	long long		last_eaten;
	bool			stop_simulation;
	char			*mutex_sem_name;
	sem_t			*mutex_local_sem;
	pthread_t		death_checker;
	pthread_t		full_checker;
	pthread_t		stop_sim_checker;
	pthread_mutex_t	lock;
	// only shared resources available to all threads for both read/write
	t_shared		*shared;
}					t_philo;

typedef struct s_resources
{
	t_philo		*philos;
	t_shared	*shared;
	pid_t		*pids;
	t_philo		*philo;
}				t_resources;

/* actions.c + actions_forks.c */
int				p_eat(t_philo *philo);
int				p_sleep(t_philo *philo);
int				p_think(t_philo *philo);
int				take_forks(t_philo *philo);
int				leave_forks(t_philo *philo);

/* philo_bonus.c */
int				start_simulation(char **argv, int total);
int				main(int argc, char **argv);


/* cleanup.c */
int				close_semaphores(t_shared *shared);
int				destroy_semaphores(t_shared *shared);

/* init.c */
int				init_shared_resources(t_shared *shared, int total);
int				init_philo_data(t_philo *p, int i, char **argv, t_shared *shared);
int				init_resources(t_philo **philos, pid_t **pids, t_shared *shared, char **argv);

/* input_val.c */
int				handle_error_input(int argc, char **argv);

/* threads.c */
void			*stop_routine(void	*arg);
// void			*full_routine(void	*arg);
void			*death_routine(void *arg);

/* utils.c && logging.c */
long long		get_time_in_ms(void);
long long		get_time_in_micros(void);
bool			check_stop_sim(t_philo *philo);
int				log_msg(t_philo *philo, t_action action);

/* utils_libft.c */
unsigned int	ft_atou(const char *nptr);
int				ft_strncmp(const char *s1, const char *s2, size_t n);
char			*ft_utoa(unsigned long n);
size_t			ft_strlen(const char *s);
size_t			get_uint_size(unsigned long n);
void			ft_putnbr(long long nb);
char			*ft_strjoin(char const *s1, char const *s2);
void			ft_putchar(char c);
int				check_arg(char *val);
char			*get_mutex_sem_name(t_philo *philo);

#endif
