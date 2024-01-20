/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-alen <fde-alen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 17:57:21 by fde-alen          #+#    #+#             */
/*   Updated: 2024/01/11 17:13:01 by fde-alen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	count_words(const char *s, char c)
{
	size_t	i;
	size_t	word_count;

	i = 0;
	word_count = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			word_count++;
			while (s[i] && s[i] != c)
				i++;
		}
		else
			i++;
	}
	return (word_count);
}

static size_t	word_length(const char *s, char c)
{
	size_t	word_len;

	word_len = 0;
	while (s[word_len] && s[word_len] != c)
		word_len++;
	return (word_len);
}

static char	*copy_word(const char *s, size_t len)
{
	char	*word;

	word = (char *)malloc(len + 1);
	if (!word)
		return (NULL);
	ft_strlcpy(word, s, len + 1);
	return (word);
}

static void	*ft_free_split(char **split, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		free(split[i]);
		i++;
	}
	free(split);
	return (NULL);
}

/**
 * Allocates and returns an array of strings obtained by splitting 's'
 * using the character 'c' as a delimiter. The array is ended by a NULL pointer.
 * 
 * @param[in] s The string to be split.
 * @param[in] c The delimiter character.
 * 
 * @return The array of new strings resulting from the split. Returns NULL
 *         if the allocation fails or 's' is NULL.
 */
char	**ft_split(char const *s, char c)
{
	char	**split;
	size_t	i;
	size_t	j;

	if (!s)
		return (NULL);
	split = (char **)malloc((count_words(s, c) + 1) * sizeof(char *));
	if (!split)
		return (NULL);
	i = 0;
	j = 0;
	while (i < count_words(s, c))
	{
		while (s[j] && s[j] == c)
			j++;
		split[i] = copy_word(&s[j], word_length(&s[j], c));
		if (!split[i])
		{
			return (ft_free_split(split, i));
		}
		i++;
		j += word_length(&s[j], c);
	}
	split[i] = NULL;
	return (split);
}
