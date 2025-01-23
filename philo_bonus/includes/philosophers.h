/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 17:26:55 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/23 13:04:39 by zpiarova         ###   ########.fr       */
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

/* MACROS */
# define ERROR 1
# define SUCCESS 0
# define TIMEZONE_MILI 3600000
# define TIMEZONE_MICRO 3600000000

/* SEMAPHORES */
# define FORK_SEM "/fork_sem"
# define MSG_SEM "/msg_sem"
# define STOP_SEM "/stop_sem"
# define FULLNESS_SEM "/fullness_sem"

/* STRUCTS & OTHER DT */
typedef enum e_action
{
	FORK_L,
	FORK_R,
	EATS,
	SLEEPS,
	THINKS,
	DEATH,
	FINISH,
	FULL,
	STOP,
	RECEIVED,
	STOP_STATUS,
	CHANGE
}	t_action;

typedef struct s_shared
{
	sem_t			*fork_sem;
	sem_t			*msg_sem;
	sem_t			*fullness_sem;
	sem_t			*stop_sem;
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
	// variables that are set in program and belong to one philo at all times
	unsigned int	times_eaten;
	long long		last_eaten;
	bool			stop_simulation;
	char			*mutex_sem_name;
	sem_t			*mutex_local_sem;
	pthread_t		death_checker;
	pthread_t		full_checker;
	pthread_t		stop_sim_checker;
	pthread_mutex_t	lock;
	int				exit_status;
	long long		start_time;
	// only shared resources available to all threads for both read/write
	t_shared		*shared;
}					t_philo;

typedef struct s_resources
{
	t_philo			*philos;
	t_shared		*shared;
	pid_t			*pids;
	t_philo			*philo;
}					t_resources;

/*  FUNCTIONS */

/* actions.c */
int				p_eat(t_philo *philo);
int				p_sleep(t_philo *philo);
int				p_think(t_philo *philo);
int				take_forks(t_philo *philo);
int				leave_forks(t_philo *philo);

/* philo_bonus.c */
void			child_routine(t_philo *philo);
int				start_simulation(t_philo *p, t_shared *s, pid_t *pids, int n);
int				main(int argc, char **argv);

/* cleanup.c */
int				close_global_semaphores(t_shared *shared);
int				destroy_global_semaphores(t_shared *shared);
int				destroy_local_res(t_philo *philo);
int				destroy_glob_res_child(t_philo *ph, t_shared *s, pid_t *pids);
int				destroy_glob_res_parent(t_philo *ph, t_shared *s, pid_t *pids);

/* init.c */
int				init_global_semaphores(t_shared *shared, int total);
int				init_philo_data(t_philo *p, int i, char **argv, t_shared *s);
int				init_glob_res(t_philo **p, pid_t **pid, t_shared *s, char **a);
int				init_local_res(t_philo *philo);

/* input_val.c */
int				handle_error_input(int argc, char **argv);

/* threads.c */
void			*stop_routine(void	*arg);
void			*death_routine(void *arg);
void			*full_routine(void *arg);
int				run_full_checker_thread(t_philo *philo);

/* utils.c && logging.c */
long long		get_time_in_ms(void);
long long		get_time_in_micros(void);
bool			check_stop_sim(t_philo *philo);
int				log_msg(t_philo *philo, t_action action);
int				handle_fork_error(pid_t *pids, int i);
int				sem_close_helper(sem_t *sem1, sem_t *sem2, sem_t *sem3);
int				sem_unlink_helper(char *sem_n1, char *sem_n2, char *sem_n3);

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
