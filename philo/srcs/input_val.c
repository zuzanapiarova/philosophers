/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_val.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 08:42:15 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/13 18:25:11 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// validate not enough arguments
int	validate_num_of_arguments(int	argc)
{
	if (argc != 5 && argc != 6)
	{
		write(1, "Program expects 4 or 5 arguments. Run ./philo with: \n", 53);
		write(1, "\tnumber_of_philosophers\n", 24);
		write(1, "\ttime_to_die (ms)\n", 18);
		write(1, "\ttime_to_eat (ms)\n", 18);
		write(1, "\ttime_to_sleep (ms)\n", 20);
		write(1, "\toptional [number_of_times_each_philosopher_must_eat] (ms)\n", 60);
		return (ERROR);
	}
	return(SUCCESS);
}

// 1. validate input to be unsigned ints
// TODO: protect against numbers bigger than MAX_INT*2 and the addition of ms arguments to current time being bigger than max int *2, aleo against numbers smaller than 0
int validate_arguments(int argc, char **argv)
{
	int	i;
	int	val;
	int	flag;
	char *str; // TODO: change 
	
	(void)argc;
	i = 1;
	flag = 0;
	while (argv[i])
	{
		val = ft_atou(argv[i]);
		str = ft_utoa((long long)val); // TODO: change 
		if (val < 0 || ft_strncmp(argv[i], str, ft_strlen(argv[i]))) // TODO: change this too because now it takes 00 not as 0
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
		free(str);
		str = NULL;
		i++;
	}
	if (flag == 1)
		return (ERROR);
	return (SUCCESS);
}

// TODO: validate numbers to not be 0 or any other possible logical value
// handles logical errors like 0 or other logically wrong inputs
int	handle_logical_errors(int argc, char **argv)
{
	int			i;
	long long	val;
	
	(void)argc;
	i = 0;
	while (argv[i])
	{
		val = ft_atou(argv[i]);
		if (i == 1 && val < 1)
			return (write(1, "Number of philosophers must be at least 1 for program to start.\n", 64), ERROR);
		if (i == 2 && val == 0)
			return (write(1, "\033[31mThey all died instantly. :(\033[37m\n", 38), ERROR);
		i++;
	}
	return (SUCCESS);
}

int handle_error_input(int argc, char **argv)
{
	if (validate_num_of_arguments(argc) == ERROR)
		return (ERROR);
	// valid arguments - (unsigned) int, (unsigned) long, (unsigned)long long - protect against characters and max int values 
	if (validate_arguments(argc, argv) == ERROR)
		return (ERROR);
	if (handle_logical_errors(argc, argv))
		return (ERROR);
	// at least eat time because each philosopher must eat, as defined in subject
	// protect agains one philo, with message "One philosopher means one fork. He could not eat without 2 forks. You sent him off for an assured death.:("
	return (0);
}
