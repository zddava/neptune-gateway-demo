# 测试mask分布情况

from itertools import groupby
from operator import itemgetter

mask_list = []

def ip_to_int(ip):
  i = 0
  ret = 0
  for ip_part in ip.split('.')[::-1]:
    ret = ret + int(ip_part) * (256**i)
    i+=1

  return ret

with open('all.txt', 'r') as f:
  for line in f:
    line = line.rstrip()
    if line.startswith('#') or line == "":
      continue

    ip, mask = line.split("/")
    mask_list.append(int(mask))

mask_list.sort()
masks = []

for mask in mask_list:
  masks.append({"mask": int(mask)})

g = groupby(masks, itemgetter("mask"))

for key, group in g:
  sum = 0
  for grp in group:
    sum+=1
  
  print(key, sum)
