obj-m += ng-sentry.o
ng-sentry-objs := sentry.o logger.o rule.o ng_string.o ng_util.o proc.o hook.o

SDK_DIR=/opt/openwrt-sdk
PWD=$(shell pwd)
KERNEL_DIR="$(SDK_DIR)/build_dir/target-i386_pentium4_musl/linux-x86_generic/linux-5.4.188/"
TOOLCHAIN_DIR="$(SDK_DIR)/staging_dir/toolchain-i386_pentium4_gcc-8.4.0_musl/bin/i486-openwrt-linux-"
STAGING_DIR="$(SDK_DIR)/staging_dir/"
TARGET_CFLAGS+=-std=gnu99

all:
	make -C $(KERNEL_DIR) ARCH=x86 CROSS_COMPILE=$(TOOLCHAIN_DIR) STAGING_DIR=$(STAGING_DIR) M=$(PWD) modules

clean:
	rm -f *.ko
	rm -f *.o
	rm -f *.mod
	rm -f *.mod.c
	rm -f *.mod.o
	rm -f *.order
	rm -f *.symvers
	rm -f .*.cmd
	rm -f .*.o.d
