
#define SYSCTL_ROOT "neptune"

#define PROC_FS_ROOT "neptune"
#define PROC_FS_ISP_CONFIG "isp_config"

int init_sysctl(void);

int exit_sysctl(void);

int init_procfs(void);

int exit_procfs(void);

extern int ng_enable;
