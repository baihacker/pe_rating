#include "../EloR/EloR.h"

#include <cstdio>
#include <regex>
#include <iostream>
using namespace std;

static regex line_reg("<tr><td><span style=\"color: rgb\\(85, 85, 85\\);.*?</td></tr>");
static regex name_reg("bold;\">.*?</span></td><td>(.*?)</td>");
static regex decorated_name_reg("help;\">(.*?)</span>");

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

vector<string> getRecentFileList()
{
  vector<string> result;
  for (int i = 586; i <= 595; ++i)
  {
    char buff[256];
    sprintf(buff, "../data/pe/recent/pe%d.txt", i);
    result.push_back(buff);
  }
  return result;
}

void genRecentRatings()
{
  map<string, Player> data;
  for (const auto& iter: getRecentFileList())
  {
    vector<string> solvers = parseRanks(readFile(iter));
    handleHistory(data, solvers);
  }
  vector<Player::RankTuple> vec;
  for (auto& iter: data) vec.push_back(iter.second.toTuple());
  sort(vec.begin(), vec.end());
  reverse(vec.begin(), vec.end());
  for (auto& iter: vec) cout << get<1>(iter) << " " << get<0>(iter) << " " << get<2>(iter) << endl;
}
int main()
{
  genRecentRatings();
  return 0;
}