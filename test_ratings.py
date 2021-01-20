#-*- coding: utf8 -*-
import os
import sys
import json
import urllib
import re
import time
import os
import socket

def load_file_data(f):
  if os.path.exists(f):
    with open(f, 'rb') as tempf:
      result = tempf.read();
      return result
  else:
    return None

def load_db(file):
  db = {}
  text = load_file_data(file)
  if text != None:
    db = eval(text)
  return db

if __name__ == "__main__":
  db = load_db("pe_ratings.txt")
  for iter in db:
    print iter["label"]
    it = iter["data"]
    if "baihacker" in it:
      print it["baihacker"]