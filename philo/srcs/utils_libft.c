/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_libft.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 08:40:42 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/06 08:41:38 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

unsigned int	ft_atou(const char *nptr)
{
	unsigned int	u;
	int				i;

	i = 0;
	u = 0;
	if (!ft_strncmp(nptr, "4294967295", 10))
		return (0);
	while (nptr[i] == 32 || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	if (nptr[i] == '+' || nptr[i] == '-')
	{
		if (nptr[i] == '-')
			return (0);
		i++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		u = u * 10 + (nptr[i] - '0');
		i++;
	}
	return (u);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t			i;
	unsigned char	*ds1;
	unsigned char	*ds2;

	i = 0;
	ds1 = (unsigned char *)s1;
	ds2 = (unsigned char *)s2;
	if (n == 0)
		return (0);
	while (i < n && ds1[i])
	{
		if (ds1[i] != ds2[i] || i == n - 1)
			return (ds1[i] - ds2[i]);
		i++;
	}
	return (ds1[i] - ds2[i]);
}

size_t	get_uint_size(unsigned int n)
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

char	*ft_utoa(unsigned int n)
{
	int				size;
	char			*result;
	unsigned long	num;

	size = get_uint_size(n);
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

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

void	ft_putchar(char c)
{
	write(1, &c, 1);
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
