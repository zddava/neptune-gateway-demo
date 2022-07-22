
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/slab.h>

#include "rule.h"
#include "logger.h"
#include "general.h"

struct rule_handler_entry
{
  char *action;
  int (*handler)(void);
};

struct rule_handler_entry rule_handler_map[] = {
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

void read_config_content(char **config_buffer)
{
  struct file *fp = NULL;
  struct inode *inode = NULL;
  off_t file_size;
  mm_segment_t fs;

  fp = filp_open(RULE_CONFIG_FILE, O_RDONLY, 0);
  if (IS_ERR(fp))
  {
    NG_ERROR("open %s failed\n", RULE_CONFIG_FILE);
    return;
  }

  inode = fp->f_inode;
  file_size = inode->i_size;
  if (file_size == 0)
  {
    NG_WARN("%s content is empty\n", RULE_CONFIG_FILE);
    return;
  }

  *config_buffer = (char *)kzalloc(sizeof(char) * file_size, GFP_KERNEL);
  if (NULL == *config_buffer)
  {
    NG_ERROR("allocate memory failure \n");
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

int read_config_file()
{
  char *config_buffer = NULL;

  read_config_content(&config_buffer);
  if (!config_buffer)
  {
    return EXIT_FAILURE;
  }

  NG_INFO("%s", config_buffer);

  return EXIT_SUCCESS;
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
