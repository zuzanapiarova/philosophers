/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_val.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 16:39:39 by zuzanapiaro       #+#    #+#             */
/*   Updated: 2024/12/29 17:28:48 by zuzanapiaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

// validate not enough arguments

// 1. validate input to be unsigned ints

// 2. validate numbers to be not 0 if needed or other possible values

int handle_error_input(int argc, char **argv)
{
	int i;
	int flag;
	int val;
	char *str;

	if (argc != 5 && argc != 6)
	{
		write(1, "Program expects 4 or 5 arguments. Please call ./philo with:\n", 60);
		write(1, "\tnumber_of_philosophers\n", 24);
		write(1, "\ttime_to_die (ms)\n", 18);
		write(1, "\ttime_to_eat (ms)\n", 18);
		write(1, "\ttime_to_sleep (ms)\n", 20);
		write(1, "\toptional [number_of_times_each_philosopher_must_eat] (ms)\n", 60);
		return (1);
	}
	// TODO: protect against numbers bigger than MAX_INT*2 and the addition of ms arguments to current time being bigger than max int *2, aleo against numbers smaller than 0
	i = 1;
	flag = 0;
	while (argv[i])
	{
		val = ft_atou(argv[i]);
		str = ft_utoa(val);
		//printf("argv: %s, val: %u\n", argv[i], val);
		if (val < 0 || ft_strncmp(argv[i], str, ft_strlen(argv[i])))
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
		if (flag == 0 && i == 1 && val < 1)
			return (write(1, "Number of philosophers must be at least 1.\n", 43), 1);
		i++;
	}
	if (flag == 1)
		return (1);
	// TODO: protect arguments against being zero ?
	// at least eat time because each philosopher must eat, as defined in subject
	// protect agains one philo, with message "One philosopher means one fork. He could not eat without 2 forks. You sent him off for an assured death.:("
	return (0);
}
