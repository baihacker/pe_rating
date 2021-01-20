import os

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
  print load_db("pe_scores.txt")