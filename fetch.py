#! python
import urllib2
import time
import socket
import os

SCRIPT_DIRECTORY = os.path.dirname(os.path.realpath(__file__))

socket.setdefaulttimeout(60)

cookie = ''
minid = 754
maxid = 754


def fetch(id):
  opener = urllib2.build_opener()
  opener.addheaders.append(('Cookie', cookie))
  url = "https://projecteuler.net/fastest=%s" % id
  f = opener.open(url)
  result = f.read().decode(encoding='utf8', errors='ignore')
  return result


def fetch_all():
  for i in range(minid, maxid + 1):

    filename = os.path.join(SCRIPT_DIRECTORY, 'data\\pe\\pe%d.txt' % i)
    print(filename)

    result = fetch(i)
    with open(filename, 'wb') as tempf:
      tempf.write(result)
    time.sleep(1)


if __name__ == "__main__":
  fetch_all()
