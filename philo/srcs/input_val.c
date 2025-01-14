/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_val.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 08:42:15 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/14 10:12:16 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// validates number of arguments
int	validate_num_of_arguments(int argc)
{
	if (argc != 5 && argc != 6)
	{
		write(1, "\033[31mProgram expects 4 or 5 arguments. ", 39);
		write(1, "Run ./philo [1] [2] [3] [4] (5): \033[37m\n", 40);
		write(1, "\033[38;2;255;182;193m\t[1] number_of_philosophers\n", 47);
		write(1, "\t[2] time_to_die (ms)\n", 22);
		write(1, "\t[3] time_to_eat (ms)\n", 22);
		write(1, "\t[4] time_to_sleep (ms)\n", 24);
		write(1, "\t(5) (optional)", 15);
		write(1, "number_of_times_each_philosopher_must_eat (ms)\033[37m\n",
			53);
		return (ERROR);
	}
	return (SUCCESS);
}

// validates if input is one of accepted number types
// TODO: protect against numbers bigger than MAX_INT*2 and ...
// TODO: ... and the addition of ms arguments to current time being ...
// TODO: ... bigger than max int *2, aleo against numbers smaller than 0
int	validate_arguments(char **argv)
{
	int	i;
	int	flag;

	i = 1;
	flag = 0;
	while (argv[i])
	{
		if (check_arg(argv[i]) == ERROR)
		{
			if (flag == 0)
			{
				write(1, "\033[31mArguments to program", 27);
				write(1, "must be positive integer values.\033[31m\n", 67);
				flag = 1;
			}
			write(1, "\033[38;2;255;182;193m--> change argument:\t", 40);
			write(1, argv[i], ft_strlen(argv[i]));
			write(1, "\n", 1);
		}
		i++;
	}
	if (flag == 1)
		return (ERROR);
	return (SUCCESS);
}

// handles logical errors like 0 or other logically wrong inputs
int	handle_logical_errors(char **argv)
{
	int			i;
	long long	val;

	i = 0;
	while (argv[i])
	{
		val = ft_atou(argv[i]);
		if (i == 1 && val < 1)
		{
			write(1, "\033[31mNumber of philosophers must be ", 36);
			write(1, "at least 1 for program to start\n", 32);
			return (ERROR);
		}
		if (i == 2 && val == 0)
		{
			write(1, "\033[31mYou made them die instantly. :(\033[37m\n", 46);
			return (ERROR);
		}
		i++;
	}
	return (SUCCESS);
}

int	handle_error_input(int argc, char **argv)
{
	if (validate_num_of_arguments(argc) == ERROR)
		return (ERROR);
	if (validate_arguments(argv) == ERROR)
		return (ERROR);
	if (handle_logical_errors(argv) == ERROR)
		return (ERROR);
	return (0);
}
