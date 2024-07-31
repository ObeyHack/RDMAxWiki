#ifndef WORKSHOP_NETWORK_EX3_BW_TEMPLATE_H
#define WORKSHOP_NETWORK_EX3_BW_TEMPLATE_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <stdlib.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <infiniband/verbs.h>

struct pingpong_context {
    struct ibv_context		*context;
    struct ibv_comp_channel	*channel;
    struct ibv_pd		*pd;
    struct ibv_mr		*mr;
    struct ibv_cq		*cq;
    struct ibv_qp		*qp;
    void			*buf;
    int				size;
    int				rx_depth;
    int				routs;
    struct ibv_port_attr	portinfo;
};


#define MAX_INLINE 60
#define MEGABIT 1048576
#define MEGA_POWER 20
#define PORT 8543
#define KB 1024

static struct pingpong_context *pp_init_ctx(struct ibv_device *ib_dev, int size,
                                            int rx_depth, int tx_depth, int port,
                                            int use_event, int is_server);

int pp_close_ctx(struct pingpong_context *ctx);

static int pp_post_recv(struct pingpong_context *ctx, int n);

static int pp_post_send(struct pingpong_context *ctx);

int flagged_pp_post_send(struct pingpong_context *ctx, unsigned int flag);

int pp_wait_completions(struct pingpong_context *ctx, int iters);

int init_connection(char* servername, struct pingpong_context** ctx_p);

#endif //WORKSHOP_NETWORK_EX3_BW_TEMPLATE_H
