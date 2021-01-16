#include "pe_rating.h"

#include <cstdio>
#include <regex>
#include <iostream>
using namespace std;

static regex line_reg("<td><span class=\".*?</td></tr>");
static regex separator("</td><td>");
static regex decorated_name_reg("title=\".*?\">(.*?)</span>");

vector<string> parseRanks(const string& data) {
  vector<string> result;

  sregex_iterator token(data.begin(), data.end(), line_reg);
  sregex_iterator end;
  for (; token != end; ++token) {
    const string& line = token->str();
    vector<string> tokens;
    {
      sregex_token_iterator p(line.begin(), line.end(), separator, -1);
      sregex_token_iterator end;
      while (p != end) tokens.push_back(*p++);
    }
    if (tokens.size() != 5) {
      cerr << "cannot parse: " << line << endl;
      continue;
    }
    // Fix decorarted name
    smatch dnamematch;
    if (regex_search(tokens[1], dnamematch, decorated_name_reg)) {
      tokens[1] = dnamematch[1];
    }
    // Remove </td></tr>
    if (tokens[4].size() >= 10 && tokens[4].substr(tokens[4].size() - 10) == "</td></tr>") {
      tokens[4] = tokens[4].substr(0, tokens[4].size() - 10);
    }
    result.push_back(tokens[1]);
  }
  return result;
}

string readFile(const string& path) {
  const int buffer_size = 1 << 20;
  string buffer;
  buffer.resize(buffer_size);

  FILE* fp = fopen(path.c_str(), "rb");
  if (fp == NULL) {
    return "";
  }

  string result;
  for (;;) {
    int read = fread((void*)buffer.c_str(), 1, buffer_size, fp);
    if (read == 0) break;
    result.append(buffer.begin(), buffer.begin() + read);
  }
  fclose(fp);

  return result;
}

vector<FileId> genFileList(const string& dir, int start, int end) {
  vector<FileId> result;
  for (int i = start; i <= end; ++i) {
    char buff[256];
    sprintf(buff, "%s/pe%d.txt", dir.c_str(), i);
    result.push_back({buff, i});
  }
  return result;
}

void testRegex() { parseRanks(readFile("fetch/pe477.txt")); }

int main(int argc, char* argv[]) {
  string dir = "data/pe/";
  int start = 0;
  int end = 1000;
  int top = -1;
  string format = "console";

  for (int i = 1; i < argc;) {
    if (argv[i][0] == '-' && (argv[i][1] == 'd' || argv[i][1] == 'D') &&
        i + 1 < argc) {
      dir = argv[i + 1];
      i += 2;
    } else if (argv[i][0] == '-' && (argv[i][1] == 's' || argv[i][1] == 'S') &&
               i + 1 < argc) {
      start = atoi(argv[i + 1]);
      i += 2;
    } else if (argv[i][0] == '-' && (argv[i][1] == 'e' || argv[i][1] == 'E') &&
               i + 1 < argc) {
      end = atoi(argv[i + 1]);
      i += 2;
    } else if (argv[i][0] == '-' && (argv[i][1] == 't' || argv[i][1] == 'T') &&
               i + 1 < argc) {
      top = atoi(argv[i + 1]);
      i += 2;
    } else if (argv[i][0] == '-' && (argv[i][1] == 'f' || argv[i][1] == 'F') &&
               i + 1 < argc) {
      format = argv[i + 1];
      i += 2;
    } else {
      ++i;
    }
  }

  cout << "Author baihacker (bailiangsky@gmail.com)" << endl;
  cout << "https://github.com/baihacker" << endl;
  cout << endl;

  cout << "dir = " << dir << endl;
  cout << "start = " << start << endl;
  cout << "end = " << end << endl;
  cout << "top = " << top << endl;
  cout << "format = " << format << endl;
  cout << endl;

  genCfStatistics(genFileList(dir, start, end), top, format);
  return 0;
}
