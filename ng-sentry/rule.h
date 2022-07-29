#include <linux/types.h>
#include <linux/skbuff.h>

#define REG_RULE_CONFIG_FILE "/etc/ng/rcg_rule.conf"
#define ISO_RULE_CONFIG_FILE "/etc/ng/isp_rule.conf"

#define MAX_URL_LEN 128
#define MAX_IP_LEN 18
#define MAX_RCG_RULE_TAG_LEN 8
#define MAX_RULE_LINE_LEN 256

#define MAX_ISP_IP_NETWORK_LEN 8192

#define CTC 1
#define CUCC 2
#define CMCC 3

typedef struct flow_info
{
  struct sk_buff *skb;
  uint32_t srcip;
  uint32_t dstip;
  uint16_t sport;
  uint16_t dport;
  int protocol;
} flow_info_t;

typedef struct ng_rcg_item
{
  struct list_head head;
  char host_url[MAX_URL_LEN];
  uint32_t host_ip;
  uint32_t mark;
  int (*handler)(flow_info_t *, uint32_t mark);
} ng_rcg_item_t;

typedef struct ng_isp_item
{
  uint32_t first;
  uint32_t last;
  int isp;
} ng_isp_item_t;

int load_rcg_config_file(void);
int load_isp_config_file(void);

int match_rcg_item(ng_rcg_item_t **, flow_info_t *);
int match_isp_item(ng_isp_item_t **, flow_info_t *);

ng_isp_item_t *isp_config_at(int index);

int handle_accept(flow_info_t *, uint32_t);
int handle_drop(flow_info_t *, uint32_t);
int handle_mark(flow_info_t *, uint32_t);
