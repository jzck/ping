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

#pragma once

# include "rs.h"
# include "cliopts.h"

# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <sys/wait.h>

# include <fcntl.h>
# include <errno.h>
# include <resolv.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>

# define FT_PCT(a, t)	(t ? 100 * (float)(t - a)/(float)t : 0)

typedef struct s_ping	t_ping;

struct			s_ping
{
	t_flag			flag;
	char			**av_data;
	size_t			pkt_size;
	pid_t			pid;
	t_rs			rs;
	int				pkt_sent;
	int				pkt_recv;
	int				ttl;
	float			interval;
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

unsigned short	cksum(void *b, int len);
double			time_milli(void);
void			listener(int domain, int sock, int proto,
		void (*handler)(void *buf, int bytes, struct sockaddr *addr));
