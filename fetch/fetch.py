import urllib2
import time
import socket

socket.setdefaulttimeout(60)

cookie='PHPSESSID=db67214e2d5aefe36c635f08867d3d19; keep_alive=1610819119%23274577%23UK3Hb3ydLSV2dUdhnzWpaoWzWOzXxJni'
minid=353
maxid=633

def fetch(id):
  opener = urllib2.build_opener()
  opener.addheaders.append(('Cookie', cookie))
  url = "https://projecteuler.net/fastest=%s"%id
  f = opener.open(url)
  result = f.read().decode(encoding='utf8',errors='ignore')
  return result

def fetch_all():
  for i in range(minid, maxid+1):

    filename = 'pe%d.txt'%i
    print(filename)

    result = fetch(i)
    with open(filename, 'wb') as tempf:
      tempf.write(result)
    time.sleep(1)

if __name__ == "__main__":
  fetch_all()
