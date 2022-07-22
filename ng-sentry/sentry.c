#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>

#include "logger.h"
#include "rule.h"

// MODULE_LICENSE("Proprietary");
// MODULE_AUTHOR("zhangdd@hzwesoft.com");
// MODULE_DESCRIPTION("neptune gateway sentry");
// MODULE_VERSION("1.0.0");

// struct list_head config_list_head = LIST_HEAD_INIT(config_list_head);

static int __init sentry_init(void)
{
  NG_DEBUG("neptune sentry init!\n");
  // 读取用户态配置文件
  read_config_file();
  // 初始化实时调试
  // 初始化字符设备
  // 初始化netlink
  // 添加netfilter钩子，特定域名丢包，识别特定域名包
  return 0;
}

static void __exit sentry_exit(void)
{
  NG_DEBUG("neptune sentry exit!\n");
}

module_init(sentry_init);
module_exit(sentry_exit);
