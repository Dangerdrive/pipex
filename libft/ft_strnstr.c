/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-alen <fde-alen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/17 18:59:21 by fde-alen          #+#    #+#             */
/*   Updated: 2024/01/10 21:27:57 by fde-alen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * Locates a substring in a string, examining only the first 'len' characters.
 *
 * This function searches for the first occurrence of the substring 'little' in
 * the string 'big', where not more than 'len' characters are searched. Characters
 * that appear after a '\0' character are not searched. If 'little' is an empty string,
 * 'big' is returned; if 'little' occurs nowhere in 'big', NULL is returned.
 *
 * @param[in] big The string in which the search is performed.
 * @param[in] little The substring to be located.
 * @param[in] len The number of characters to be searched in 'big'.
 * 
 * @return A pointer to the first character of the first occurrence of 'little'.
 * If 'little' is not found, NULL is returned.
 */
char	*ft_strnstr(const char *big, const char *little, size_t len)
{	
	size_t	llen;

	llen = ft_strlen(little);
	if (llen == 0)
		return ((char *)big);
	if (llen > len)
		return (NULL);
	while (*big && len >= llen)
	{
		if (ft_memcmp(big, little, llen) == 0)
			return ((char *)big);
		big++;
		len--;
	}
	return (NULL);
}
