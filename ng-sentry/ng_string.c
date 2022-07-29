#include <linux/string.h>

#include "ng_types.h"
#include "ng_string.h"

char *strtok(char *str, const char *delim)
{
  static char *strtok_left = NULL;
  char *p_str;
  const char *p_delim;
  bool match_flag;

  if (str == NULL)
  {
    str = strtok_left;
  }
  if (str == NULL)
  {
    return NULL;
  }

  while (*str != 0)
  {
    p_delim = delim;
    match_flag = FALSE;

    while (*p_delim != 0)
    {
      if (*p_delim++ == *str)
      {
        match_flag = TRUE;
        break;
      }
    }

    if (!match_flag)
    {
      break;
    }

    str++;
  }

  p_str = str;
  while (*p_str != 0)
  {
    p_delim = delim;
    match_flag = FALSE;

    while (*p_delim != 0)
    {
      if (*p_delim++ == *p_str)
      {
        match_flag = TRUE;
        *p_str = 0;
        break;
      }
    }

    p_str++;
    if (match_flag)
    {
      strtok_left = p_str;
      return str;
    }
  }

  strtok_left = NULL;
  return str;
}

char *fstrtok(char *str, const char *delim)
{
  static char *strtok_left = NULL;

  char *p_str;
  const char *p_delim;
  bool match_flag;
  char *delim_point;

  if (str == NULL)
  {
    str = strtok_left;
  }
  if (str == NULL)
  {
    return NULL;
  }

  p_str = str;
  p_delim = delim;
  match_flag = FALSE;
  while (*p_delim != 0)
  {
    if (*p_delim++ == *p_str++)
    {
      match_flag = TRUE;
      continue;
    }

    match_flag = FALSE;
    break;
  }

  if (match_flag)
  {
    str = p_str;
  }

  p_str = str;
  while (*p_str != 0)
  {
    delim_point = p_str;
    p_delim = delim;
    match_flag = FALSE;

    while (*p_delim != 0)
    {
      if (*p_delim++ == *p_str)
      {
        match_flag = TRUE;
        p_str++;
        continue;
      }

      match_flag = FALSE;
      break;
    }

    if (match_flag)
    {
      *delim_point = 0;
      strtok_left = p_str;
      return str;
    }
    else
    {
      p_str = delim_point + 1;
    }
  }

  strtok_left = NULL;
  return str;
}

int split(char *dest[], char *str, const char *delim)
{
  int i = 0;
  char *result = NULL;
  result = strtok(str, delim);
  while (result != NULL)
  {
    dest[i++] = result;
    result = strtok(NULL, delim);
  }
  return i;
}

int fsplit(char *dest[], char *str, const char *delim)
{
  int i = 0;
  char *result = NULL;
  result = fstrtok(str, delim);
  while (result != NULL)
  {
    dest[i++] = result;
    result = fstrtok(NULL, delim);
  }
  return i;
}
