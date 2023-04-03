/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*													+:+ +:+		 +:+	 */
/*   By: lmiehler <marvin@42.fr>					+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2022/11/29 15:10:53 by lmiehler		  #+#	#+#			 */
/*   Updated: 2022/11/29 15:11:51 by lmiehler         ###   ########.fr       */
/*																			*/
/* ************************************************************************** */

#include <stdlib.h>
#include <stddef.h>
#include "libft.h"

char	*ft_strdup(const char *s)
{
	size_t	len;
	char	*ptr;
	char	*i;

	len = ft_strlen(s) + 1;
	ptr = malloc(sizeof(*ptr) * len);
	i = ptr;
	if (ptr == NULL)
		return (NULL);
	while (*s)
		*i++ = *s++;
	*i = '\0';
	return (ptr);
}
