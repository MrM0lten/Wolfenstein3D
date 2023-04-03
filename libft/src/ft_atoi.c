/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*													+:+ +:+		 +:+	 */
/*   By: lmiehler <marvin@42.fr>					+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2022/11/29 15:25:10 by lmiehler		  #+#	#+#			 */
/*   Updated: 2022/12/02 12:38:35 by lmiehler         ###   ########.fr       */
/*																			*/
/* ************************************************************************** */

int	ft_atoi(const char *nptr)
{
	int		sign;
	long	ret;

	ret = 0;
	sign = 1;
	while (*nptr == ' ' || *nptr == '\f' || *nptr == '\n'
		|| *nptr == '\r' || *nptr == '\t' || *nptr == '\v')
		++nptr;
	if (*nptr == '+' || *nptr == '-')
		sign = ',' - *nptr++;
	while (*nptr >= '0' && *nptr <= '9')
		ret = ret * 10 + (*nptr++ - '0');
	return ((int)(sign * ret));
}
