# 测试ip地址分布情况

import ipaddress
from itertools import groupby
from operator import itemgetter

def ip_to_int(ip):
  i = 0
  ret = 0
  for ip_part in ip.split('.')[::-1]:
    ret = ret + int(ip_part) * (256**i)
    i+=1

  return ret

dst_ips = []

with open('dst_ip.txt', 'r') as f:
  for line in f:
    line = line.rstrip()
    if line.startswith('#') or line == "":
      continue

    dst_ips.append(int(line))

dst_ips.sort()

isp_ips = []

def load_isp_config(filename, isp):
  with open(filename, 'r') as f:
    for line in f:
      line = line.rstrip()
      if line.startswith('#') or line == "":
        continue

      ip_net = ipaddress.ip_network(line, strict=False)
      isp_ips.append({"network": line, "first": ip_to_int(ip_net[0].compressed), "last": ip_to_int(ip_net[-1].compressed), "isp": isp})

load_isp_config("cmcc.txt", "cmcc")
load_isp_config("ctc.txt", "ctc")
load_isp_config("cucc.txt", "cucc")

def takeFirst(e):
  return e["first"]

isp_ips.sort(key=takeFirst)

def binary_search(alist, item):
  n = len(alist)
  start = 0
  end = n-1

  while start <= end:
    mid = (start+end)//2
    if item >= alist[mid]['first'] and item <= alist[mid]['last']:
      return alist[mid]
    elif item < alist[mid]['first']:
      end = mid - 1
    else:
      start = mid +1
  
  return None

# intip = ip_to_int("47.104.224.31")
# intip = ip_to_int("202.107.203.23")

# ret = binary_search(isp_ips, intip)

# print(ret)

dist_ip_group = []

g = groupby(dst_ips)
for key, group in g:
  sum = 0
  for grp in group:
    sum+=1
  
  dist_ip_group.append({"ip": key, "count": sum})

def takeCount(e):
  return e["count"]

dist_ip_group.sort(key=takeCount, reverse=True)

sum_ip = 0
sum_match = 0
for ip in dist_ip_group:
  sum_ip = sum_ip + ip['count']
  ret = binary_search(isp_ips, ip['ip'])
  if ret is not None:
    sum_match = sum_match + ip['count']

print(sum_match, sum_ip)

print(sum_match/sum_ip)
