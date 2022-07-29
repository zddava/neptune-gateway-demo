#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include "proc.h"
#include "logger.h"
#include "rule.h"
#include "ng_types.h"

int ng_enable __read_mostly = 1;

// sysctl {
static struct ctl_table_header *ng_table_header;

static struct ctl_table ng_table[] = {
    {
        .procname = "log_level",
        .data = &log_level,
        .maxlen = sizeof(int),
        .mode = 0666,
        .proc_handler = proc_dointvec,
    },
    {
        .procname = "enable",
        .data = &ng_enable,
        .maxlen = sizeof(int),
        .mode = 0666,
        .proc_handler = proc_dointvec,
    },
    {},
};

static struct ctl_table ng_root_table[] = {
    {
        .procname = SYSCTL_ROOT,
        .mode = 0555,
        .child = ng_table,
    },
    {},
};

int init_sysctl(void)
{
  ng_table_header = register_sysctl_table(ng_root_table);
  if (ng_table_header == NULL)
  {
    NG_ERROR("init sysctl failure");
    return EXIT_FAILURE;
  }

  NG_DEBUG("ng-sentry: init sysctl success");
  return EXIT_SUCCESS;
}

int exit_sysctl(void)
{
  if (ng_table_header)
  {
    unregister_sysctl_table(ng_table_header);
  }

  NG_DEBUG("ng-sentry: exit sysctl success");
  return EXIT_SUCCESS;
}

// sysctl }

// procfs {

struct proc_dir_entry *ng_proc_root;

static void *isp_config_seq_start(struct seq_file *s, loff_t *pos)
{
  if (*pos == 0)
  {
    return SEQ_START_TOKEN;
  }

  return isp_config_at(*pos - 1);
}

static void *isp_config_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
  (*pos)++;
  return isp_config_at(*pos - 1);
}

static void isp_config_seq_stop(struct seq_file *s, void *v)
{
}

static int isp_config_seq_show(struct seq_file *s, void *v)
{
  ng_isp_item_t *item = (ng_isp_item_t *)v;
  static volatile int index = 1;
  if (v == SEQ_START_TOKEN)
  {
    index = 1;
    seq_printf(s, "%-4s\t%-10s\t%-10s\t%-3s\n", "index", "first ip", "last ip", "isp");
    return 0;
  }

  seq_printf(s, "%-4d\t%-10u\t%-10u\t%3d\n", index++, item->first, item->last, item->isp);
  return 0;
}

static const struct seq_operations rcg_seq_file_ops = {
    .start = isp_config_seq_start,
    .next = isp_config_seq_next,
    .stop = isp_config_seq_stop,
    .show = isp_config_seq_show,
};

static int isp_config_open(struct inode *inode, struct file *file)
{
  return seq_open(file, &rcg_seq_file_ops);
}

const struct file_operations isp_config_ops = {
    .owner = THIS_MODULE,
    .open = isp_config_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release,
};

int init_procfs(void)
{
  struct proc_dir_entry *isp_config_entry;

  ng_proc_root = proc_mkdir(PROC_FS_ROOT, NULL);
  if (!ng_proc_root)
  {
    NG_WARN("create proc/%s error", PROC_FS_ROOT);
    return EXIT_FAILURE;
  }

  isp_config_entry = proc_create(PROC_FS_ISP_CONFIG, 0444, ng_proc_root, &isp_config_ops);

  return EXIT_SUCCESS;
}

int exit_procfs(void)
{
  remove_proc_entry(PROC_FS_ISP_CONFIG, ng_proc_root);
  remove_proc_entry(PROC_FS_ROOT, NULL);

  return EXIT_SUCCESS;
}

// procfs }
