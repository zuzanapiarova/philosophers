/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   num_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:52:30 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/13 16:53:42 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"


size_t	get_uint_size(unsigned long n)
{
	size_t	size;

	size = 0;
	if (n <= 0)
		size = 1;
	while (n != 0)
	{
		size++;
		n /= 10;
	}
	return (size);
}

char	*ft_utoa(unsigned long n)
{
	int					size;
	char				*result;
	unsigned long long	num;

	size = get_uint_size((unsigned long long)n);
	result = (char *)malloc((size + 1) * sizeof(char));
	if (!result)
		return (NULL);
	num = (unsigned long) n;
	if (n < 0)
	{
		num *= -1;
	}
	result[size] = '\0';
	while (size > 0)
	{
		result[size - 1] = num % 10 + '0';
		num /= 10;
		size--;
	}
	if (n < 0)
		result[0] = '-';
	return (result);
}

void	ft_putnbr(long long nb)
{
	if (nb < 0)
	{
		ft_putchar('-');
		nb = -nb;
	}
	if (nb >= 10)
	{
		ft_putnbr(nb / 10);
		nb = nb % 10;
	}
	if (nb < 10)
		ft_putchar(nb + 48);
}
