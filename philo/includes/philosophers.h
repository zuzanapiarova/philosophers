/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 17:26:55 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/15 13:39:20 by zpiarova         ###   ########.fr       */
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

# define ERROR 1
# define SUCCESS 0
# define TIMEZONE_MILI 3600000
# define TIMEZONE_MICRO 3600000000

typedef enum e_action
{
	FORK_L, // TOOK LEFT FORK
	FORK_R, // TOOK RIGHT FORK
	EATS, // STARTED EATING
	SLEEPS, // STARTED SLEEPING
	THINKS, // STARTED THINKING
	DEATH, // DIED
	FINISH
}				t_action;

typedef struct s_shared
{
	bool			stop_simulation;
	long long		start_time;
	pthread_mutex_t	*forks;
	pthread_mutex_t	msg_lock;
	pthread_mutex_t	stop_lock;
}				t_shared;

typedef struct s_philo
{
	// variables from user input that are static
	int				id;
	int				total;
	unsigned int	eat;
	unsigned int	sleep;
	unsigned int	die;
	int				times_to_eat; // int because is -1 if not assigned
	long long		start_time;
	pthread_t		thread;
	// variables that change but belong to one thread at all times
	unsigned int	times_eaten;
	long long		last_eaten;
	bool			finished; //finished eating
	pthread_mutex_t	lock;
	// only shared resources available to all threads for both read/write
	pthread_mutex_t	*forks;
	pthread_mutex_t	*msg_lock;
	pthread_mutex_t	*stop_lock;
	bool			*stop_simulation;
}					t_philo;

/* actions.c + actions_forks.c */
int				p_eat(t_philo *philo);
int				p_sleep(t_philo *philo);
int				p_think(t_philo *philo);
int				fork_r(t_philo *p, pthread_mutex_t *rf, pthread_mutex_t *lf);
int				fork_l(t_philo *p, pthread_mutex_t *lf, pthread_mutex_t *rf);
int				take_forks(t_philo *philo);
int				leave_forks(t_philo *philo);

/* philo.c */
int				start_simulation(int argc, char **argv, int total);
int				main(int argc, char **argv);

/* init.c */
int				init_shared_resources(t_shared *shared, int total);
int				init_philo_data(t_philo *p, int i, char **argv, t_shared *s);
int				allocate_philo_array(t_philo **p, t_shared *s, int t);
int				fill_philo_array(t_philo *p, t_shared *s, char **argv, int t);
int				init_monitor(pthread_t *mon, t_philo *p, t_shared *s, int t);

/* cleanup.c */
int				cleanup(t_philo *p, t_shared *s, pthread_t *monitor, int t);

/* routines.c */
void			*routine(void *arg);
void			*monitoring(void *arg);

/* input_val.c */
int				handle_error_input(int argc, char **argv);

/* utils.c */
int				log_msg(t_philo *philo, t_action action);
long long		get_time_in_ms(void);
long long		get_time_in_micros(void);
bool			check_stop_sim(t_philo *philo);

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

#endif
