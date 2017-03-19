import urllib2
import time
import socket

socket.setdefaulttimeout(20)

cookie=''
maxid=595

def fetch(id):
  opener = urllib2.build_opener()
  opener.addheaders.append(('Cookie', cookie))
  url = "https://projecteuler.net/fastest=%s"%id
  f = opener.open(url)
  result = f.read().decode(encoding='utf8',errors='ignore')
  return result

def fetch_all():
  for i in range(277, maxid+1):
    
    filename = 'pe%d.txt'%i
    print(filename)

    result = fetch(i)
    with open(filename, 'wb') as tempf:
      tempf.write(result)
    time.sleep(1)

if __name__ == "__main__":
  fetch_all()