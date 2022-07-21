#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

// MODULE_LICENSE("Proprietary");
// MODULE_AUTHOR("zhangdd@hzwesoft.com");
// MODULE_DESCRIPTION("neptune gateway sentry");
// MODULE_VERSION("1.0.0");

static int __init sentry_init(void)
{
  printk("neptune sentry init!\n");
  // 读取用户态配置文件
  // 初始化实时调试
  // 初始化字符设备
  // 初始化netlink
  // 添加netfilter钩子，特定域名丢包，识别特定域名包
  return 0;
}

static void __exit sentry_exit(void)
{
  printk("neptune sentry exit!\n");
}

module_init(sentry_init);
module_exit(sentry_exit);
