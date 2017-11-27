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

t_ping		g_ping =
{
	.pid = -1,
	.pkt_size = 56,
	.pkt_sent = -1,
	.pkt_recv = 0,
};

int			resolve_host(char *node, t_ping *ping)
{
	struct addrinfo		*result;
	struct addrinfo		hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;
	if (getaddrinfo(node, NULL, &hints, &result) != 0)
	{
		perror("getaddrinfo");
		exit(1);
	}
	ping->sa = result;
	inet_ntop(AF_INET, &(((struct sockaddr_in*)ping->sa->ai_addr)->sin_addr),
			ping->ip4, INET_ADDRSTRLEN);
	return (0);
}

void		display_time_exceeded(struct sockaddr *addr, struct icmp *icmp)
{
	char		ipbuf[INET_ADDRSTRLEN];

	bzero(ipbuf, INET_ADDRSTRLEN);
	getnameinfo(addr, sizeof(struct sockaddr_in), ipbuf, INET_ADDRSTRLEN,
			NULL, 0, NI_NUMERICHOST);
	printf("From %s icmp_seq=%d Time to live exceeded\n",
			ipbuf, icmp->icmp_id);
}

void		display(void *buf, int bytes, struct sockaddr *addr)
{
	struct ip		*ip;
	struct icmp		*icmp;
	int				hlen;
	double			triptime;
	char			hbuf[NI_MAXHOST];
	char			ipbuf[INET_ADDRSTRLEN];

	ip = buf;
	(void)bytes;
	hlen = ip->ip_hl << 2;
	icmp = (struct icmp*)(buf + hlen);
	if (icmp->icmp_type == ICMP_TIME_EXCEEDED)
		return (display_time_exceeded(addr, icmp));
	else if (icmp->icmp_id != g_ping.pid)
	{
		dprintf(2, "id = %i\n", icmp->icmp_id);
		return ;
	}
	triptime = time_milli() - *(double*)(icmp + 1);
	rs_push(&g_ping.rs, triptime);
	g_ping.pkt_recv++;

	bzero(hbuf, NI_MAXHOST);
	bzero(ipbuf, INET_ADDRSTRLEN);
	getnameinfo(addr, sizeof(struct sockaddr_in), ipbuf, INET_ADDRSTRLEN,
			NULL, 0, NI_NUMERICHOST);
	if (getnameinfo(addr, sizeof(struct sockaddr_in), hbuf, NI_MAXHOST, NULL, 0, 0) == 0)
		printf("%zu bytes from %s (%s): icmp_seq=%d ttl=%i time=%.1f ms\n",
				bytes - hlen - sizeof(struct icmphdr), hbuf, ipbuf, icmp->icmp_seq, ip->ip_ttl, triptime);
	else
		printf("%zu bytes from %s: icmp_seq=%d ttl=%i time=%.1f ms\n",
				bytes - hlen - sizeof(struct icmphdr), ipbuf, icmp->icmp_seq, ip->ip_ttl, triptime);

}

void		ping(int signo)
{
	char			pkt[g_ping.pkt_size + sizeof(struct icmp)];
	double			*msg;
	struct icmp		*hdr;
	double			epoch;

	(void)signo;
	hdr = (struct icmp*)&pkt;
	bzero(&pkt, sizeof(pkt));
	hdr->icmp_type = ICMP_ECHO;
	hdr->icmp_id = g_ping.pid;
	hdr->icmp_seq = ++g_ping.pkt_sent;
	msg = (double*)(pkt + sizeof(struct icmp));
	epoch = time_milli();
	memcpy(msg, (void*)&epoch, sizeof(double));
	hdr->icmp_cksum = cksum(&pkt, sizeof(pkt));
	sendto(g_ping.sock, &pkt, sizeof(pkt), 0, g_ping.sa->ai_addr,
			sizeof(struct sockaddr));
	alarm(1);
}

void		stats_recap(int signo)
{
	double		loss;

	(void)signo;
	rs_final(&g_ping.rs);
	loss = FT_PCT(g_ping.pkt_recv, g_ping.pkt_sent);
	printf("\n--- %s ping statistics ---", g_ping.sa->ai_canonname);
	printf("\n%d packets transmitted, %d packets received, %0.1f%% packet loss",
			g_ping.pkt_sent, g_ping.pkt_recv, loss);
	printf("\nrtt min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
			g_ping.rs.min, g_ping.rs.avg, g_ping.rs.max, g_ping.rs.stdev);
	freeaddrinfo(g_ping.sa);
	exit(0);
}

int			main(int ac, char **av)
{
	if (ac != 2)
	{
		dprintf(2, "%s <addr>\n", av[0]);
		exit(1);
	}
	resolve_host(av[1], &g_ping);
	if (g_ping.sa->ai_canonname)
		printf("PING %s (%s): %zu(%zu) data bytes\n",
				g_ping.sa->ai_canonname, g_ping.ip4,
				g_ping.pkt_size, g_ping.pkt_size + sizeof(struct icmp));
	else
		printf("PING %s: %zu(%zu) data bytes\n", g_ping.ip4,
				g_ping.pkt_size, g_ping.pkt_size + sizeof(struct icmp));
	if ((g_ping.sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("socket");
		exit(1);
	}
	if (setsockopt(g_ping.sock, 0, IP_TTL, (int[]){255}, sizeof(int)) != 0)
	{
		perror("setsockopt");
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
