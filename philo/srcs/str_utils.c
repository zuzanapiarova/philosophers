/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zpiarova <zpiarova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:52:57 by zpiarova          #+#    #+#             */
/*   Updated: 2025/01/13 17:22:22 by zpiarova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

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

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*result;
	int		i;
	int		length;

	i = 0;
	length = (int)ft_strlen(s1) + (int)ft_strlen(s2);
	result = (char *)malloc((length + 1) * sizeof(char));
	if (!result)
		return (NULL);
	while (i < (int)ft_strlen(s1))
	{
		result[i] = s1[i];
		i++;
	}
	while (i < ((int)ft_strlen(s1) + (int)ft_strlen(s2)))
	{
		result[i] = s2[i - ft_strlen(s1)];
		i++;
	}
	result[i] = '\0';
	return (result);
}
