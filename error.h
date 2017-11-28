/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhalford <jack@crans.org>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/03/14 15:34:21 by jhalford          #+#    #+#             */
/*   Updated: 2017/05/15 17:37:29 by ariard           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_H
# define ERROR_H

# include <stdarg.h>
# include <stdlib.h>
# include <stdio.h>

/*
** DEBUG with malloc
*/
# define DG_MSG		"{inv}{ran}%5i{yel}%21s {bol}{blu}%-3d{eoc}"
# define DG_ARGS	getpid(), getpid(), ft_path_notdir(__FILE__), __LINE__
# define DG(s, ...)	dprintf(STDBUG,DG_MSG s "{eoc}\n",DG_ARGS,##__VA_ARGS__)

/*
** DEBUG with no malloc
*/
# define DG2		ft_putstr(__FILE__"\t");ft_putnbr(__LINE__)
# define DGW(d)		DG2;d;ft_putchar('\n')
# define DGS(s)		DGW(ft_putstr(": "s"="))
# define DGSN(s, n)	DGW(ft_putstr(": "s"=");ft_putnbr(n))
# define DGSH(s, n)	DGW(ft_putstr(": "s"=");ft_putnbr_hex(n))

# define ERR_PROTO(u, m)	"%s: %s\n", u, m
# define ERR_MSG(u, m)		dprintf(2, ERR_PROTO(u, m))
# define ERR_SET(n, ...)	error_set(n, ##__VA_ARGS__)
# define ERRMSG_MAX_SIZE	150

enum	e_errors
{
	E_NOERR,
	E_CO_INV,
	E_CO_INVL,
	E_CO_MULT,
	E_CO_MISS,
	E_CO_MISSL,
	E_SYS_NOFILE,
	E_SYS_ISDIR,
	E_SYS_NOPERM,
	E_MAX,
};

extern char	g_error_msg[E_MAX][ERRMSG_MAX_SIZE];
extern char	*g_errmsg;
extern int	g_errnum;
extern char	**g_argv;

int		error_set(int n, ...);
int		ft_perror(char *utility);
void	ft_usage(char *format, ...);

#endif
