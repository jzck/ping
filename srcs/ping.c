/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhalford <jack@crans.org>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/04/22 14:10:24 by jhalford          #+#    #+#             */
/*   Updated: 2017/10/08 14:35:02 by jhalford         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"
#define TIME_START		0
#define TIME_END		1
#define TIME_TRIPTIME	2

t_ping		g_ping =
{
	.pid = -1,
	.pkt_sent = -1,
	.pkt_recv = 0,
};

void		display(void *buf, int bytes, struct sockaddr_in *addr)
{
	struct ip		*ip;
	struct icmp		*icmp;
	struct s_packet	*pkt;
	int				hlen;
	double			triptime;
	char			strbuf[INET_ADDRSTRLEN];

	ip = buf;
	(void)bytes;
	hlen = ip->ip_hl << 2;
	pkt = (struct s_packet*)(buf + hlen);
	icmp = &pkt->hdr;
	if (icmp->icmp_id != g_ping.pid)
		return ;
	triptime = time_milli() - *(double*)&pkt->msg;
	rs_push(&g_ping.rs, triptime);
	g_ping.pkt_recv++;
	printf("%d bytes from %s: icmp_seq=%d ttl=%i time=%.3f ms\n",
			ip->ip_len,
			inet_ntop(AF_INET, &(addr->sin_addr), strbuf, INET_ADDRSTRLEN),
			icmp->icmp_seq, ip->ip_ttl, triptime);
}

void		ping(int signo)
{
	struct s_packet	pkt;
	double			epoch;

	(void)signo;
	bzero(&pkt, sizeof(pkt));
	pkt.hdr.icmp_type = ICMP_ECHO;
	pkt.hdr.icmp_id = g_ping.pid;
	pkt.hdr.icmp_seq = ++g_ping.pkt_sent;
	epoch = time_milli();
	ft_memcpy(pkt.msg, (void*)&epoch, sizeof(epoch));
	pkt.hdr.icmp_cksum = cksum(&pkt, sizeof(pkt));
	sendto(g_ping.sock, &pkt, sizeof(pkt), 0, g_ping.sa->ai_addr,
			sizeof(struct sockaddr));
}

void		stats_recap(int signo)
{
	double		loss;

	(void)signo;
	rs_final(&g_ping.rs);
	loss = FT_PCT(g_ping.pkt_recv, g_ping.pkt_sent);
	printf("\n--- %s ping statistics ---", g_ping.ip4);
	printf("\n%d packets transmitted, %d packets received, %0.1f%% packet loss",
			g_ping.rs.count, g_ping.pkt_recv, loss);
	printf("\nround-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms",
			g_ping.rs.min, g_ping.rs.avg, g_ping.rs.max, g_ping.rs.stdev);
	freeaddrinfo(g_ping.sa);
	exit(0);
}

int			resolve_host(char *hostname, t_ping *ping)
{
	struct addrinfo		*result;
	struct addrinfo		hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;
	if (getaddrinfo(hostname, NULL, &hints, &result) != 0)
	{
		perror("getaddrinfo");
		exit(1);
	}
	ping->sa = result;
	inet_ntop(AF_INET, &(((struct sockaddr_in*)ping->sa->ai_addr)->sin_addr), ping->ip4, INET_ADDRSTRLEN);
	return (0);
}

int			main(int ac, char **av)
{
	if (ac != 2)
	{
		ft_usage("%s <addr>\n", av[0]);
		exit(1);
	}
	resolve_host(av[1], &g_ping);
	if (g_ping.sa->ai_canonname)
		printf("PING %s (%s): %i data bytes\n",
				g_ping.sa->ai_canonname, g_ping.ip4, PACKETSIZE);
	else
		printf("PING %s: %i data bytes\n", g_ping.ip4, PACKETSIZE);
	if ((g_ping.sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		ft_dprintf(2, "socket(2) error\n");
		exit(1);
	}
	if (setsockopt(g_ping.sock, 0, IP_TTL, (int[]){255, 0}, sizeof(int)) != 0)
	{
		ft_dprintf(2, "setsockopt(2) error\n");
		exit(1);
	}
	g_ping.pid = getpid();
	rs_init(&g_ping.rs);
	signal(SIGINT, stats_recap);
	signal(SIGALRM, ping);
	alarm(1);
	listener(PF_INET, SOCK_RAW, IPPROTO_ICMP, &display);
	return (0);
}
