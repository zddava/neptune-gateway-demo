#include <linux/netfilter.h>
#include <linux/skbuff.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/tcp.h>
#include <net/ip.h>

#include "ng_types.h"
#include "rule.h"
#include "hook.h"
#include "proc.h"
#include "logger.h"

int parse_flow(struct sk_buff *skb, flow_info_t *flow)
{
  struct tcphdr *tcp_header;
  struct iphdr *ip_header;
  struct nf_conn *ct;

  if (!skb)
  {
    return FALSE;
  }

  ct = (struct nf_conn *)skb->_nfct;
  if (!ct)
  {
    return FALSE;
  }

  ip_header = ip_hdr(skb);
  if (!ip_header)
  {
    return FALSE;
  }

  flow->skb = skb;
  flow->srcip = ip_header->saddr;
  flow->dstip = ip_header->daddr;
  flow->protocol = ip_header->protocol;

  switch (flow->protocol)
  {
  case IPPROTO_TCP:
    tcp_header = (struct tcphdr *)ip_header + 1;
    flow->dport = htons(tcp_header->dest);
    flow->sport = htons(tcp_header->source);
    return TRUE;
  default:
    return FALSE;
  }

  return FALSE;
}

static u_int32_t ng_sentry_hook(void *priv,
                                struct sk_buff *skb,
                                const struct nf_hook_state *state)
{
  flow_info_t flow;
  ng_rcg_item_t *rcg_item;
  struct nf_conn *ct = (struct nf_conn *)skb->_nfct;

  if (!ng_enable)
  {
    return NF_ACCEPT;
  }

  if (!nf_ct_is_confirmed(ct))
  {
    return NF_ACCEPT;
  }

  memset(&flow, 0, sizeof(flow_info_t));
  if (!parse_flow(skb, &flow))
  {
    return NF_ACCEPT;
  }

  if (!match_rcg_item(&rcg_item, &flow))
  {
    return NF_ACCEPT;
  }

  return rcg_item->handler(&flow, rcg_item->mark);
}

static struct nf_hook_ops ng_sentry_ops[] __read_mostly = {
    {
        .hook = ng_sentry_hook,
        .pf = PF_INET,
        .hooknum = NF_INET_FORWARD,
        .priority = NF_IP_PRI_MANGLE + 1,
    },
};

int init_net_hook(void)
{
  return nf_register_net_hook(&init_net, ng_sentry_ops);
}

int exit_net_hook(void)
{
  nf_unregister_net_hook(&init_net, ng_sentry_ops);

  return EXIT_SUCCESS;
}
