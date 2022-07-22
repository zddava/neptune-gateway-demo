#include <linux/types.h>

#define RULE_CONFIG_FILE "/etc/ng/rule.conf"

#define MAX_URL_LEN 128
#define MAX_IP_LEN 18
#define MAX_RULE_TAG_LEN 8
#define MAX_RULE_LINE_LEN 256

struct ng_rule_item
{
  struct list_head head;
  char host_url[MAX_URL_LEN];
  char host_ip[MAX_IP_LEN];
  char tag[MAX_RULE_TAG_LEN];
  int (*handler)(void);
};

int read_config_file(void);

int handle_accept(void);
int handle_drop(void);
int handle_mark(void);
