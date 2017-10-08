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

int				g_pid = -1;
int				g_pkt_rec = 0;
char			g_domain[256];
struct s_stats	g_rs;

void			display(void *buf, int bytes, struct sockaddr_in *addr)
{
	struct ip		*ip;
	struct icmp		*icmp;
	struct s_packet	*pkt;
	int				hlen;
	double			triptime;

	ip = buf;
	(void)bytes;
	hlen = ip->ip_hl << 2;
	pkt = (struct s_packet*)(buf + hlen);
	icmp = &pkt->hdr;
	if (icmp->icmp_id != g_pid)
		return ;
	triptime = time_milli() - *(double*)&pkt->msg;
	rs_push(triptime);
	g_pkt_rec++;
	printf("%d bytes from %s: icmp_seq=%d ttl=%i time=%.3f ms\n",
			ip->ip_len,
			inet_ntop(AF_INET, &(addr->sin_addr), g_domain, INET_ADDRSTRLEN),
			icmp->icmp_seq, ip->ip_ttl, triptime);
}

void			ping(struct sockaddr_in *addr)
{
	int				sd;
	int				cnt;
	struct s_packet	pkt;
	double			epoch;

	if ((sd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
		return (perror("sender socket"));
	if (setsockopt(sd, 0, IP_TTL, (int[]){255, 0}, sizeof(int)) != 0)
		perror("set TTL option");
	cnt = 0;
	while (1)
	{
		bzero(&pkt, sizeof(pkt));
		pkt.hdr.icmp_type = ICMP_ECHO;
		pkt.hdr.icmp_id = g_pid;
		pkt.hdr.icmp_seq = cnt++;
		epoch = time_milli();
		ft_memcpy(pkt.msg, (void*)&epoch, sizeof(epoch));
		pkt.hdr.icmp_cksum = cksum(&pkt, sizeof(pkt));
		if (sendto(sd, &pkt, sizeof(pkt), 0,
					(struct sockaddr*)addr, sizeof(*addr)) <= 0)
			return (perror("sendto"));
		sleep(1);
	}
}

void			stats_recap(int signo)
{
	double		loss;

	(void)signo;
	rs_calcmore();
	loss = FT_PCT(g_pkt_rec, g_rs.count);
	printf("\n--- %s ping statistics ---", g_domain);
	printf("\n%d packets transmitted, %d packets received, %0.1f%% packet loss",
			g_rs.count, g_pkt_rec, loss);
	printf("\nround-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms",
			g_rs.min, g_rs.avg, g_rs.max, g_rs.stdev);
	exit(0);
}

struct addrinfo	*resolve_host(char *hostname)
{
	struct addrinfo		*result;
	struct addrinfo		hints;
	struct sockaddr_in	*addr;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;
	if (getaddrinfo(hostname, NULL, &hints, &result) != 0)
	{
		perror("getaddrinfo");
		exit(1);
	}
	addr = (struct sockaddr_in*)result->ai_addr;
	inet_ntop(AF_INET, &(addr->sin_addr), g_domain, INET_ADDRSTRLEN);
	if (result->ai_canonname)
		ft_strcpy(g_domain, result->ai_canonname);
	return (result);
}

int				main(int ac, char **av)
{
	struct addrinfo		*result;

	if (ac != 2)
	{
		ft_usage("%s <addr>\n", av[0]);
		exit(1);
	}
	g_pid = getpid();
	result = resolve_host(av[1]);
	if (result->ai_canonname)
		printf("PING %s (%s): %i data bytes\n",
			result->ai_canonname, g_domain, PACKETSIZE);
	else
		printf("PING %s: %i data bytes\n", g_domain, PACKETSIZE);
	if (fork() == 0)
	{
		signal(SIGINT, stats_recap);
		rs_clear();
		listener(PF_INET, SOCK_RAW, IPPROTO_ICMP, &display);
	}
	ping((struct sockaddr_in*)result->ai_addr);
	return (0);
}
