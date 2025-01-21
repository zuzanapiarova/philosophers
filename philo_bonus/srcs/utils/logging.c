/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logging.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zuzanapiarova <zuzanapiarova@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:51:21 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/20 21:40:34 by zuzanapiaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/philosophers.h"

// "\033[31m" red
// "\033[33m" yellow
// "\033[34m" blue
// "\033[35m" magenta
// "\033[36m" cyan
// "\033[37m" white
// "\033[38;2;255;255;179m" - Pastel Yellow
// "\033[38;5;208m" - Pastel Orange
// "\033[38;2;255;182;193m" - Pastel Pink
// "\033[38;2;144;238;144m" - Pastel Green
// "\033[38;5;146m" - Pastel Purple
// "\033[38;5;153m" - Pastel Blue
// "\033[38;5;213m" - Pastel Magenta
// assigns collor to each action
char	*get_color(t_action action)
{
	char	*color;

	if (action == FORK_L)
		color = "\033[38;2;255;182;193m";
	else if (action == FORK_R)
		color = "\033[38;2;255;182;193m";
	else if (action == EATS)
		color = "\033[38;2;144;238;144m";
	else if (action == SLEEPS)
		color = "\033[38;5;153m";
	else if (action == THINKS)
		color = "\033[38;2;255;255;179m";
	else if (action == DEATH)
		color = "\033[31m";
	else if (action == FINISH)
		color = "\033[38;5;146m";
	else if (action == FULL)
		color = "\033[38;5;214m";
	else
		color = "\033[37m";
	return (color);
}

// gets correcponding log msg based on action
char	*get_msg(t_action action)
{
	char	*msg;

	if (action == FORK_L)
		msg = "has taken left fork\n";
	else if (action == FORK_R)
		msg = "has taken right fork\n";
	else if (action == EATS)
		msg = "is eating for ";
	else if (action == SLEEPS)
		msg = "is sleeping\n";
	else if (action == THINKS)
		msg = "is thinking\n";
	else if (action == DEATH)
		msg = "died\033[37m\n";
	else if (action == FINISH)
		msg = "simulation finished\033[37m\n";
	else if (action == FULL)
		msg = "is full\033[37m\n";
	else if (action == STOP)
		msg = "simulation stopped\033[37m\n";
	else if (action == LAST)
		msg = "SENDING STOP SIGNAL\033[37m\n";
	else if (action == RECEIVED)
		msg = "stop signal received\033[37m\n";
	else
		msg = "";
	return (msg);
}

// prints message to the console based on parameter action
// in format: timestamp_in_ms id action
// use mutex to lock access to terminal while one log msg is being printed out
// prevents messages from being mixed up
// & when testing, can change time to be in microseconds by removing / 1000
int	log_msg(t_philo *philo, t_action action)
{
	char	*msg;
	char	*time;
	char	*color_time;

	msg = get_msg(action);
	time = ft_utoa(((get_time_in_micros() - philo->shared->start_time)));
	color_time = ft_strjoin(get_color(action), time);
	free(time);
	sem_wait(philo->shared->msg_sem);
	write(1, color_time, ft_strlen(color_time));
	write(1, "\t  ", 3);
	if (action != FINISH)
		ft_putnbr(philo->id);
	else
		write(1, "X", 1);
	write(1, " ", 1);
	write(1, msg, ft_strlen(msg));
	if (action == EATS)
	{
		ft_putnbr(philo->times_eaten);
		write(1, ". time\n", 7);
	}
	sem_post(philo->shared->msg_sem);
	free(color_time);
	return (SUCCESS);
}


