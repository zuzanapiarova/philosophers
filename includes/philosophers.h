#ifndef PHILO_D
# define PHILO_D

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>

typedef enum e_action
{
	FORK, // TOOK FORK
	EATS, // STARTED EATING
	SLEEPS, // STARTED SLEEPING
	THINKS, // STARTED THINKING
	DEATH // DIED
}   t_action;

typedef struct		s_info
{
	int				total;
	unsigned int	eat;
	unsigned int	sleep;
	unsigned int	die;
	unsigned int	times_to_eat;
} 					t_info;

typedef struct	s_philo
{
	int			id;
	pthread_t	thread;
	bool		eats;
	bool		sleeps;
	bool		thinks;
	bool		dead;
}				t_philo;

/* philo.c */
void			init_info(t_info *info, int argc, char **argv);
void			log_msg(t_info *info, int id, t_action action);

/* utils_libft.c */
unsigned int	ft_atou(const char *nptr);
int				ft_strncmp(const char *s1, const char *s2, size_t n);
char			*ft_utoa(unsigned int n);
size_t			ft_strlen(const char *s);
size_t			get_uint_size(unsigned int n);

#endif
