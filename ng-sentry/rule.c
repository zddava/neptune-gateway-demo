
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/netfilter.h>

#include "rule.h"
#include "logger.h"
#include "ng_string.h"
#include "ng_types.h"

struct rcg_rule_handler_entry
{
  char *action;
  int (*handler)(flow_info_t *, uint32_t mark);
};

struct rcg_rule_handler_entry rcg_rule_handler_map[] = {
    {
        .action = "ACCEPT",
        .handler = handle_accept,
    },
    {
        .action = "DROP",
        .handler = handle_drop,
    },
    {
        .action = "MARK",
        .handler = handle_mark,
    },
};

struct list_head rcg_config_list_head = LIST_HEAD_INIT(rcg_config_list_head);
ng_isp_item_t *isp_config[8192];

int isp_config_index;

void read_config_content(char **config_buffer, char *filename)
{
  struct file *fp = NULL;
  struct inode *inode = NULL;
  off_t file_size;
  mm_segment_t fs;

  fp = filp_open(filename, O_RDONLY, 0);
  if (IS_ERR(fp))
  {
    NG_ERROR("open %s failed\n", filename);
    return;
  }

  inode = fp->f_inode;
  file_size = inode->i_size;
  if (file_size == 0)
  {
    NG_WARN("%s content is empty\n", filename);
    return;
  }

  *config_buffer = (char *)kzalloc(sizeof(char) * file_size, GFP_KERNEL);
  if (NULL == *config_buffer)
  {
    NG_ERROR("allocate rcg config memory failure\n");
    return;
  }

  {
    fs = get_fs();
    set_fs(KERNEL_DS);

    kernel_read(fp, *config_buffer, file_size, &(fp->f_pos));

    set_fs(fs);
  }

  filp_close(fp, NULL);
}

/* 读取以行为单位的配置 */
int read_line_based_config(char *filename, void (*line_handler)(char *))
{
  char *config_buffer = NULL;
  char *p_cur = NULL;
  char *line_start = NULL;
  char line_buffer[MAX_RULE_LINE_LEN];

  read_config_content(&config_buffer, filename);
  if (!config_buffer)
  {
    return EXIT_FAILURE;
  }

  p_cur = line_start = config_buffer;

  while (*p_cur++)
  {
    if (*p_cur == '\n')
    {
      memset(line_buffer, 0, sizeof(line_buffer));
      strncpy(line_buffer, line_start, p_cur - line_start);

      line_handler(line_buffer);
      line_start = p_cur + 1;
    }
  }

  if (p_cur != line_start)
  { // 多出来的一行或者只有一行没有换行
    memset(line_buffer, 0, sizeof(line_buffer));
    strncpy(line_buffer, line_start, p_cur - line_start);

    line_handler(line_buffer);
  }

  kfree(config_buffer);

  return EXIT_SUCCESS;
}

void add_rcg_rule_item(char *config_line)
{ // url::ip::action::[tag]   www.baidu.com::192.168.100.100::DROP::
  char *rule_arr[4];
  int len = fsplit(rule_arr, config_line, "::");
  ng_rcg_item_t *item;
  int i;

  if (len < 3)
  {
    return;
  }

  item = kzalloc(sizeof(ng_rcg_item_t), GFP_KERNEL);
  if (item == NULL)
  {
    NG_ERROR("malloc config memory error: %s\n", config_line);
    return;
  }

  strcpy(item->host_url, rule_arr[0]);
  sscanf(rule_arr[1], "%d", &(item->host_ip));
  if (len > 3)
  {
    sscanf(rule_arr[3], "%d", &(item->mark));
  }

  for (i = 0; i < sizeof(rcg_rule_handler_map) / sizeof(struct rcg_rule_handler_entry); i++)
  {
    if (strcmp(rcg_rule_handler_map[i].action, rule_arr[2]) == 0)
    {
      item->handler = rcg_rule_handler_map[i].handler;
      break;
    }
  }

  if (NULL == item->handler)
  {
    NG_WARN("rcg handler not found: %s", config_line);
    kfree(item);
    return;
  }

  list_add(&(item->head), &rcg_config_list_head);
}

void add_isp_rule_item(char *config_line)
{ // ip min::ip max::isp
  char *rule_arr[3];
  int len = fsplit(rule_arr, config_line, "::");
  ng_isp_item_t *item = NULL;

  if (len < 3)
  {
    return;
  }

  item = kzalloc(sizeof(ng_isp_item_t), GFP_KERNEL);
  if (item == NULL)
  {
    NG_ERROR("malloc config memory error: %s\n", config_line);
    return;
  }

  sscanf(rule_arr[0], "%d", &(item->first));
  sscanf(rule_arr[1], "%d", &(item->last));

  if (strcmp(rule_arr[2], "ctc") == 0)
  {
    item->isp = CTC;
  }
  else if (strcmp(rule_arr[2], "cucc") == 0)
  {
    item->isp = CUCC;
  }
  else if (strcmp(rule_arr[2], "cmcc") == 0)
  {
    item->isp = CMCC;
  }
  else
  {
    NG_WARN("isp not found: %s", config_line);
    kfree(item);
    return;
  }

  isp_config[isp_config_index++] = item;
}

int load_rcg_config_file(void)
{
  return read_line_based_config(REG_RULE_CONFIG_FILE, add_rcg_rule_item);
}

int load_isp_config_file(void)
{
  isp_config_index = 0;
  return read_line_based_config(ISO_RULE_CONFIG_FILE, add_isp_rule_item);
}

int match_rcg_item(ng_rcg_item_t **item, flow_info_t *flow)
{
  ng_rcg_item_t *i, *n;

  if (list_empty(&rcg_config_list_head))
  {
    return FALSE;
  }

  list_for_each_entry_safe(i, n, &rcg_config_list_head, head)
  {
    if (flow->dstip == i->host_ip)
    {
      *item = i;
      return TRUE;
    }
  }

  return FALSE;
}

int ip_network_search(ng_isp_item_t **matched, uint32_t ip)
{
  int start = 0;
  int end = isp_config_index - 1;
  ng_isp_item_t *item;

  while (start <= end)
  {
    int mid = (start + end) / 2;
    item = isp_config[mid];

    if (ip < item->first)
    {
      end = mid - 1;
    }
    else if (ip > item->last)
    {
      start = mid + 1;
    }
    else
    {
      *matched = item;
      return TRUE;
    }
  }

  return FALSE;
}

int match_isp_item(ng_isp_item_t **item, flow_info_t *flow)
{
  return ip_network_search(item, flow->dstip);
}

ng_isp_item_t *isp_config_at(int index)
{
  if (index < isp_config_index)
  {
    return isp_config[index];
  }

  return NULL;
}

int handle_accept(flow_info_t *flow, uint32_t mark)
{
  return NF_ACCEPT;
}

int handle_drop(flow_info_t *flow, uint32_t mark)
{
  return NF_DROP;
}

int handle_mark(flow_info_t *flow, uint32_t mark)
{
  flow->skb->mark = mark;
  return NF_ACCEPT;
}
