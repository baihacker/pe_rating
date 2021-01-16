#ifndef __PE_LIB_H__
#define __PE_LIB_H__

#include <vector>
#include <string>
using namespace std;

struct SolverInfo {
  string name;
  vector<string> tokens;
};

vector<SolverInfo> parseSolverInfo(const string& data);

string readFile(const string& path);

#endif