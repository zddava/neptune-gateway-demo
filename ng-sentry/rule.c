
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/list.h>

#include "rule.h"
#include "logger.h"
#include "ng_string.h"
#include "ng_types.h"

struct rcg_rule_handler_entry
{
  char *action;
  int (*handler)(void);
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

  NG_DEBUG("read config file(%s):\n", filename);
  NG_DEBUG("%s", config_buffer);

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
  ng_rule_item *item;
  int i;

  if (len < 3)
  {
    return;
  }

  item = kzalloc(sizeof(ng_rule_item), GFP_KERNEL);
  if (item == NULL)
  {
    NG_ERROR("malloc config memory error: %s\n", config_line);
    return;
  }

  strcpy(item->host_url, rule_arr[0]);
  strcpy(item->host_ip, rule_arr[1]);
  if (len > 3)
  {
    strcpy(item->tag, rule_arr[3]);
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
    kfree(item);
    return;
  }

  list_add(&(item->head), &rcg_config_list_head);
}

void add_iso_rule_item(char *config_line)
{ // ip min::ip max::isp
}

int load_rcg_config_file(void)
{
  return read_line_based_config(REG_RULE_CONFIG_FILE, add_rcg_rule_item);
}

int load_isp_config_file(void)
{
  return read_line_based_config(ISO_RULE_CONFIG_FILE, add_rcg_rule_item);
}

int handle_accept(void)
{
  return EXIT_SUCCESS;
}

int handle_drop(void)
{
  return EXIT_SUCCESS;
}

int handle_mark(void)
{
  return EXIT_SUCCESS;
}
