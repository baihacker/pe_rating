#include "pe_lib.h"

#include <regex>
#include <iostream>
using namespace std;

static regex line_reg("<td><span class=\".*?</td></tr>");
static regex separator("</td><td>");
static regex decorated_name_reg("title=\".*?\">(.*?)</span>");

vector<SolverInfo> parseSolverInfo(const string& data) {
  vector<SolverInfo> result;

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
    if (tokens[4].size() >= 10 &&
        tokens[4].substr(tokens[4].size() - 10) == "</td></tr>") {
      tokens[4] = tokens[4].substr(0, tokens[4].size() - 10);
    }
    result.push_back({tokens[1], tokens});
  }
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