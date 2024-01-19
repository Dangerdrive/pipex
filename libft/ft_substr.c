/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-alen <fde-alen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 18:30:37 by fde-alen          #+#    #+#             */
/*   Updated: 2024/01/10 21:28:57 by fde-alen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * Extracts a substring from a string.
 *
 * This function creates a new string which is a substring of 'str'. The substring
 * begins at the index 'start' and has a maximum length of 'len'. If 'start' is greater
 * than the length of 'str', an empty string is returned. If 'len' exceeds the length
 * of 'str' starting from 'start', the substring extends to the end of 'str'. Memory for
 * the new string is obtained with malloc, and needs to be freed by the caller.
 *
 * @param[in] str The source string from which to extract the substring.
 * @param[in] start The start index of the substring in 'str'.
 * @param[in] len The maximum length of the substring.
 *
 * @return A pointer to the newly created substring, or NULL if the allocation fails.
 */
char	*ft_substr(char const *str, unsigned int start, size_t len)
{
	char			*substr;
	unsigned int	i;

	if (len > ft_strlen(str) - start)
		len = ft_strlen(str) - start;
	if (start > ft_strlen(str))
		len = 0;
	substr = (char *)malloc(sizeof(char) * (len + 1));
	if (!substr)
		return (NULL);
	i = 0;
	while (*str && i < start)
	{
		i++;
		str++;
	}
	while (*str && i - start < len)
	{
		*substr = *str;
		i++;
		str++;
		substr++;
	}
	*substr = '\0';
	return (substr - (len));
}
