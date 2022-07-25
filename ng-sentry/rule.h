#include <linux/types.h>

#define REG_RULE_CONFIG_FILE "/etc/ng/rcg_rule.conf"
#define ISO_RULE_CONFIG_FILE "/etc/ng/iso_rule.conf"

#define MAX_URL_LEN 128
#define MAX_IP_LEN 18
#define MAX_RCG_RULE_TAG_LEN 8
#define MAX_RULE_LINE_LEN 256

typedef struct
{
  struct list_head head;
  char host_url[MAX_URL_LEN];
  char host_ip[MAX_IP_LEN];
  char tag[MAX_RCG_RULE_TAG_LEN];
  int (*handler)(void);
} ng_rule_item;

int load_rcg_config_file(void);

int load_isp_config_file(void);

int handle_accept(void);
int handle_drop(void);
int handle_mark(void);
