#include "../EloR/EloR.h"

#include <cstdio>
#include <regex>
#include <iostream>
#include <set>
using namespace std;

#if 1
// For auto downloaded data
static regex line_reg("<tr><td><span style=\"font-size:80%;.*?</td></tr>");
static regex name_reg("font-weight:bold;color:#555;\">.*?</span></td><td>(.*?)</td>");
static regex decorated_name_reg("help;\" title=\".*?\">(.*?)</span>");
#else
// For manually copied data from browser (IE)
static regex line_reg("<tr><td><span style=\"color: rgb\\(85, 85, 85\\);.*?</td></tr>");
static regex name_reg("bold;\">.*?</span></td><td>(.*?)</td>");
static regex decorated_name_reg("help;\">(.*?)</span>");
#endif

vector<string> parseRanks(const string& data)
{
  vector<string> result;

  sregex_iterator token(data.begin(), data.end(), line_reg);
  sregex_iterator end;
  for (;token != end; ++token)
  {
    const string& line = token->str();
    smatch match;
    if (regex_search(line, match, name_reg))
    {
      smatch dnamematch;
      string s(match[0].str());
      if (regex_search(s, dnamematch, decorated_name_reg))
      {
        result.push_back(dnamematch[1]);
      }
      else
      {
        result.push_back(match[1]);
      }
    }
  }
  return result;
}

string readFile(const string& path)
{
  const int buffer_size = 1 << 20;
  string buffer;
  buffer.resize(buffer_size);

  FILE* fp = fopen(path.c_str(), "rb");
  if (fp == NULL)
  {
    return "";
  }
  
  string result;
  for (;;)
  {
    int read = fread((void*)buffer.c_str(), 1, buffer_size, fp);
    if (read == 0) break;
    result.append(buffer.begin(), buffer.begin() + read);
  }
  fclose(fp);

  return result;
}

vector<string> genFileList(const string& dir, int start, int end)
{
  vector<string> result;
  for (int i = start; i <= end; ++i)
  {
    char buff[256];
    sprintf(buff, "%s/pe%d.txt", dir.c_str(), i);
    result.push_back(buff);
  }
  return result;
}

string alignName(const string& name)
{
  string result = name;
  while (result.length() < 24) result.append(" ");
  return result;
}

void genStatistics(const vector<string>& fileList, int top = -1, const string& format = "console")
{
  map<string, Player> data;
  set<string> all_guys;
  vector<vector<string> > solver_data;
  for (const auto& iter: fileList)
  {
    vector<string> solvers = parseRanks(readFile(iter));
    if (solvers.empty()) continue;
#if 0
    solver_data.push_back(solvers);
    for (auto& iter: solvers) all_guys.insert(iter);
#endif
    if (top > 0 && solvers.size() > top) solvers.resize(top);
    handleHistory(data, solvers);
  }
#if 0
  int id = 0;
  for (auto& iter: solver_data)
  {
    cerr << ++id << endl;
    vector<string> vec = iter;
    const int size = vec.size();
    set<string> have;
    for (auto& iter1: vec) have.insert(iter1);
    for (auto& iter1: all_guys)
      if (!have.count(iter1)) vec.push_back(iter1);
    
    const int n = vec.size();
    vector<int> ranklo(n);
    vector<int> rankhi(n);
    for (int i = 0; i < size; ++i) ranklo[i] = rankhi[i] = i;
    for (int i = size; i < n; ++i) ranklo[i] = size;
    for (int i = size; i < n; ++i) rankhi[i] = n-1;
    handleHistory(data, vec, ranklo, rankhi);
  }
#endif

  vector<Player::RankTuple> rank;
  for (auto& iter: data) rank.push_back(iter.second.toTuple());
  sort(rank.begin(), rank.end());
  //reverse(rank.begin(), rank.end());
  
  cout << "participants = " << rank.size() << endl;
  if (format == "json")
  {
    for (auto& iter: rank)
    {
      cout << "(\"" << get<1>(iter) << "\"," << -get<0>(iter) << "," << get<2>(iter) << ")," << endl;
    }
  }
  else 
  {
    for (auto& iter: rank)
    {
      cout << alignName(get<1>(iter)) << "\t" << -get<0>(iter) << "\t" << get<2>(iter) << endl;
    }
  }
}

int main(int argc, char *argv[])
{
  string dir = "../data/pe/recent/";
  int start = 0;
  int end = 1000;
  int top = -1;
  string format = "console";

  for (int i = 1; i < argc;)
  {
    if (argv[i][0] == '-' && (argv[i][1] == 'd' || argv[i][1] == 'D') && i + 1 < argc)
    {
      dir = argv[i+1];
      i += 2;
    }
    else if (argv[i][0] == '-' && (argv[i][1] == 's' || argv[i][1] == 'S') && i + 1 < argc)
    {
      start = atoi(argv[i+1]);
      i += 2;
    }
    else if (argv[i][0] == '-' && (argv[i][1] == 'e' || argv[i][1] == 'E') && i + 1 < argc)
    {
      end = atoi(argv[i+1]);
      i += 2;
    }
    else if (argv[i][0] == '-' && (argv[i][1] == 't' || argv[i][1] == 'T') && i + 1 < argc)
    {
      top = atoi(argv[i+1]);
      i += 2;
    }
    else if (argv[i][0] == '-' && (argv[i][1] == 'f' || argv[i][1] == 'F') && i + 1 < argc)
    {
      format = argv[i+1];
      i += 2;
    }
    else
    {
      ++i;
    }
  }
  cout << "dir = " << dir << endl;
  cout << "start = " << start << endl;
  cout << "end = " << end << endl;
  cout << "top = " << top << endl;
  cout << "format = " << format << endl;
  genStatistics(genFileList(dir, start, end), top, format);
  return 0;
}