/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhalford <jhalford@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/03/14 16:47:00 by jhalford          #+#    #+#             */
/*   Updated: 2017/03/25 01:53:25 by jhalford         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"

char	g_error_msglist[E_MAX][ERRMSG_MAX_SIZE] =
{
	"unknown error 0",
	"invalid option -%c",
	"invalid option --%s",
	"option '%c' awaits argument(s): please don't combine",
	"option '%c': missing argument",
	"option '%s': missing argument",
	"%s: no such file or directory",
	"%s: Is a directory",
	"%s: Permission denied",
};

int		g_errnum = 0;
char	*g_errmsg = NULL;
char	**g_argv;

int		error_set(int n, ...)
{
	va_list	ap;

	g_errnum = n;
	va_start(ap, n);
	if (g_errmsg)
	{
		free(g_errmsg);
		g_errmsg = NULL;
	}
	vasprintf(&g_errmsg, g_error_msglist[n], ap);
	return (g_errnum);
}

int		ft_perror(char *utility)
{
	utility = utility ? utility : g_argv[0];
	ERR_MSG(utility, g_errmsg);
	return (g_errnum);
}

void	ft_usage(char *format, ...)
{
	va_list	ap;
	char	*usage;

	va_start(ap, format);
	usage = NULL;
	vasprintf(&usage, format, ap);
	dprintf(2, "usage: %s\n", usage);
	if (usage)
	{
		free(usage);
		usage = NULL;
	}
}
