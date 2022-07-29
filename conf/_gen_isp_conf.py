# 生成isp测试配置文件

import ipaddress

def ip_to_int(ip):
  i = 0
  ret = 0
  for ip_part in ip.split('.')[::-1]:
    ret = ret + int(ip_part) * (256**i)
    i+=1

  return ret

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

values=[]

with open("isp_rule.conf", 'w') as f:
  for ip_net in isp_ips:
    line = str(ip_net["first"])+"::"+str(ip_net["last"])+"::"+ip_net["isp"]+"\n"
    f.write(line)
    values.append(ip_net["first"])

  f.flush()

print(min(values), max(values))
