#include "pe_rating.h"
#include "EloR/EloR.h"

#include <cstdio>
#include <regex>
#include <iostream>
#include <iomanip>
#include <set>
using namespace std;

void genElorStatistics(const vector<string>& fileList, int top,
                       const string& format) {
  map<string, Player> data;
  set<string> all_guys;
  vector<vector<string> > solver_data;
  for (const auto& iter : fileList) {
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
  for (auto& iter : data) rank.push_back(iter.second.toTuple());
  sort(rank.begin(), rank.end());
  // reverse(rank.begin(), rank.end());

  const int size = rank.size();

  cout << "This rating is based on Aram Ebtekar's algorithm: "
          "https://github.com/EbTech/EloR"
       << endl;
  cout << endl;
  cout << "sig_limit = " << (int)sig_limit << endl;
  cout << "sig_perf = " << (int)sig_perf << endl;
  cout << "sig_newbie = " << (int)sig_newbie << endl;
  cout << "sig_noise = " << sig_noise << endl;
  cout << "participants = " << size << endl;
  cout << endl;
  if (format == "json") {
    for (int i = 0; i < size; ++i) {
      const auto& iter = rank[i];
      cout << "(" << i + 1 << ",\"" << get<1>(iter) << "\"," << -get<0>(iter)
           << "," << get<2>(iter) << ")," << endl;
    }
  } else {
    cout << left << setw(8) << "rank" << setw(24) << "id" << setw(10)
         << "rating" << setw(10) << "max rating" << endl;
    for (int i = 0; i < size; ++i) {
      const auto& iter = rank[i];
      cout << left << setw(8) << i + 1 << setw(24) << get<1>(iter) << setw(10)
           << -get<0>(iter) << setw(10) << get<2>(iter) << endl;
    }
  }
}