from itertools import groupby
import os

ip_list = []
mask_list = []

values=[]

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
    ip_list.append(ip)
    mask_list.append(int(mask))

    # values.append(ip_to_int(ip) >> 16)

print(values)
