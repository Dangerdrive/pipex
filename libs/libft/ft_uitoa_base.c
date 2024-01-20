/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_uitoa_base.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-alen <fde-alen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 22:35:21 by fde-alen          #+#    #+#             */
/*   Updated: 2024/01/11 16:55:05 by fde-alen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * Converts an unsigned long number to its string representation in a given base.
 *
 * This function allocates a new string and converts an unsigned long number ('num') into
 * a string representation in the specified base ('base'). The conversion is performed according
 * to the format specified by 'format' (either lowercase 'x' for lowercase hexadecimal
 * representation or uppercase 'X' for uppercase hexadecimal representation).
 *
 * @param[in] num The unsigned long number to be converted.
 * @param[in] base The base for the conversion, typically 16 for hexadecimal representation.
 * @param[in] format The format specifier ('x' for lowercase, 'X' for uppercase) indicating
 *                   the case of the hexadecimal digits in the string.
 *
 * @return A pointer to the newly allocated string representing the number in the given base.
 *         Returns NULL if the memory allocation fails.
 */
char	*ft_uitoa_base(unsigned long num, int base, const char format)
{
	char	*str;
	int		len;

	len = ft_unumlen_base(num, base);
	str = (char *)malloc(sizeof(char) * (len + 1));
	if (!str)
		return (NULL);
	str[len--] = '\0';
	while (len >= 0)
	{
		if (format == 'x')
			str[len--] = "0123456789abcdef"[num % base];
		if (format == 'X')
			str[len--] = "0123456789ABCDEF"[num % base];
		num /= base;
	}
	return (str);
}
