#ifndef PHILO_D
# define PHILO_D

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>

#define ERROR 1
#define SUCCESS 0
#ifndef TIMEZONE_MS
# define TIMEZONE_MS 3600000	
#endif 

typedef enum	e_action
{
	FORK_L, // TOOK LEFT FORK
	FORK_R, // TOOK RIGHT FORK
	EATS, // STARTED EATING
	SLEEPS, // STARTED SLEEPING
	THINKS, // STARTED THINKING
	DEATH // DIED
}				t_action;

typedef struct		s_philo
{
	int				id;
	pthread_t		thread;
	pthread_mutex_t	*forks;
	int				total;
	unsigned int	eat;
	unsigned int	sleep;
	unsigned int	die;
	int				times_to_eat; // is int because can be -1 if not assigned
	bool			dead;
	bool			eats;
	bool			sleeps;
	bool			thinks;
	unsigned int	times_eaten;
	suseconds_t		last_eaten; // !! watch out because it is in microseconds, when needed to use, do /1000
	pthread_mutex_t	lock;
	pthread_mutex_t	*msg_lock;
}					t_philo;

/* actions.c */
void			*monitoring(void *arg);
int				p_eat(t_philo *philo);
int				p_sleep(t_philo *philo);
int				take_forks(t_philo *philo);
int				leave_forks(t_philo *philo);

/* philo.c */
int				init_philo(t_philo *philo, int i, char **argv, pthread_mutex_t **forks, pthread_mutex_t *msg_lock);
void			*routine(void *arg);


/* input_val.c */
int				handle_error_input(int argc, char **argv);

/* utils.c */
int				log_msg(t_philo *philo, t_action action);
long long		get_time_in_ms(void);

/* utils_libft.c */
unsigned int	ft_atou(const char *nptr);
int				ft_strncmp(const char *s1, const char *s2, size_t n);
char			*ft_utoa(unsigned int n);
size_t			ft_strlen(const char *s);
size_t			get_uint_size(unsigned int n);
void			ft_putnbr(long long nb);

#endif
