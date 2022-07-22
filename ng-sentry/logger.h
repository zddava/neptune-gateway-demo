extern int log_level;

#define LOG(level, format, ...)    \
  if (level >= log_level)       \
  {                             \
    printk(format, ##__VA_ARGS__); \
  }

#define NG_DEBUG(...) LOG(0, ##__VA_ARGS__)
#define NG_INFO(...) LOG(1, ##__VA_ARGS__)
#define NG_WARN(...) LOG(2, ##__VA_ARGS__)
#define NG_ERROR(...) LOG(3, ##__VA_ARGS__)
