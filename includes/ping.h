/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhalford <jack@crans.org>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/04/22 14:10:24 by jhalford          #+#    #+#             */
/*   Updated: 2017/10/08 14:36:37 by jhalford         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
# define FT_PING_H

# include "libft.h"
# include <fcntl.h>
# include <errno.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <resolv.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <sys/wait.h>

typedef struct s_ping	t_ping;

struct			s_ping
{
	size_t			pkt_size;
	pid_t			pid;
	t_rs			rs;
	int				pkt_sent;
	int				pkt_recv;
	int				sock;
	struct addrinfo	*sa;
	union
	{
		char		ip4[INET_ADDRSTRLEN];
		char		ip6[INET6_ADDRSTRLEN];
	}				ip;
#define ip4			ip.ip4
#define ip6			ip.ip6
};

extern t_ping		g_ping;

#endif
