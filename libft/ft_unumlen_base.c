/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unumlen_base.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-alen <fde-alen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/11 12:10:09 by fde-alen          #+#    #+#             */
/*   Updated: 2024/01/11 16:44:35 by fde-alen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * Calculates the length of an unsigned number when represented in a specified base.
 *
 * This function computes the number of digits needed to represent an unsigned long
 * number in a given base. It iteratively divides the number by the base, counting
 * how many divisions are required until the number is reduced to zero. This count
 * reflects the number of digits needed.
 *
 * @param[in] num The unsigned long number whose length is to be calculated.
 * @param[in] base The base in which the number's length is to be calculated.
 *
 * @return The number of digits required to represent 'num' in the given base.
 */
int	ft_unumlen_base(unsigned long num, int base)
{
	int	len;

	len = 1;
	while (num / base)
	{
		num /= base;
		len++;
	}
	return (len);
}
