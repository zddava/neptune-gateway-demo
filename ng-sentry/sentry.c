#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "logger.h"
#include "rule.h"
#include "proc.h"
#include "hook.h"
#include "ng_types.h"

MODULE_LICENSE("Proprietary");
MODULE_AUTHOR("zhangdd@hzwesoft.com");
MODULE_DESCRIPTION("neptune gateway sentry demo");
MODULE_VERSION("1.0.0");

static int __init sentry_init(void)
{
  int ret;
  NG_DEBUG("ng-sentry: neptune sentry init!\n");
  // 读取用户态配置文件
  ret = load_rcg_config_file();
  if (ret == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }

  ret = load_isp_config_file();
  if (ret == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }

  // 测试代码 {
  // ng_rcg_item_t *rcg_item;
  // ng_isp_item_t *isp_item;

  // flow_info_t flow;
  // flow.dstip = 2016919800;

  // if (match_rcg_item(&rcg_item, &flow))
  // {
  //   NG_INFO("rcg matched: %s", rcg_item->host_url);
  // }
  // else
  // {
  //   NG_INFO("rcg not matched");
  // }

  // flow_info_t flow2;
  // flow2.dstip = 3026530571;

  // if (match_isp_item(&isp_item, &flow))
  // {
  //   NG_INFO("isp matched: %ul, %ul, %d", isp_item->first, isp_item->last, isp_item->isp);
  // }
  // else
  // {
  //   NG_INFO("isp not matched");
  // }
  // 测试代码 }

  // 初始化实时调试
  init_sysctl();
  init_procfs();
  // 初始化netlink
  // 添加netfilter钩子，特定域名丢包，识别特定域名包
  init_net_hook();
  return 0;
}

static void __exit sentry_exit(void)
{
  exit_sysctl();
  exit_procfs();
  exit_net_hook();

  NG_DEBUG("ng-sentry: neptune sentry exit!\n");
}

module_init(sentry_init);
module_exit(sentry_exit);
