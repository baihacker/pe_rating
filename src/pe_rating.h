#ifndef __PE_RATING_H__
#define __PE_RATING_H__

#include <vector>
#include <string>
using namespace std;

vector<string> parseRanks(const string& data);
string readFile(const string& path);
void genElorStatistics(const vector<string>& fileList, int top = -1,
                       const string& format = "console");
struct FileId {
  string path;
  int id;
};

void genCfStatistics(const vector<FileId>& fileList, int top = -1,
                     const string& format = "console");

#endif