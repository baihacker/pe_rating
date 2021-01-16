#include "pe_lib.h"
#include "cf.h"
#include "EloR/EloR.h"

#include <cstdio>
#include <regex>
#include <iostream>
using namespace std;

void genElorStatistics(const vector<string>& fileList, int top = -1,
                       const string& format = "console") {
  map<string, Player> data;
  set<string> all_guys;
  vector<vector<string> > solver_data;
  for (const auto& iter : fileList) {
    vector<SolverInfo> solvers = parseSolverInfo(readFile(iter));
    vector<string> solver_names;
    for (auto& iter : solvers) solver_names.push_back(iter.name);
    if (solver_names.empty()) continue;
#if 0
    solver_data.push_back(solver_names);
    for (auto& iter: solver_names) all_guys.insert(iter);
#endif
    if (top > 0 && solver_names.size() > top) solver_names.resize(top);

    handleHistory(data, solver_names);
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

int getRoundNumber(const string& filename) {
  int l = filename.length();
  return atoi(filename.substr(l - 7, 3).c_str());
}

#if 0
int modifier[][2] = {
  {483, 1},
  {484, 2},
  {485, 9},
  {486, 1},
  {487, 1},
  {488, 4},
  {489, 3},
  {491, 1},
  {482, 1},
};

void modify77(const string& filename, vector<string>& solvers)
{
  const int round = getRoundNumber(filename);
  const int size = sizeof(modifier)/sizeof(modifier[0]);
  for (int i = 0; i < size; ++i)
  {
    if (round == modifier[i][0])
    {
      solvers.insert(solvers.begin() + modifier[i][1] - 1, "77");
      return;
    }
  }
}
#endif

void genCfStatistics(const vector<FileId>& fileList, int top = -1,
                     const string& format = "console") {
  CodeforcesRatingCalculator calculator;
  map<string, int> data;
  map<string, int> max_rating;
  for (const auto& iter : fileList) {
    // if (getRoundNumber(iter) < 495) continue;
    vector<SolverInfo> solvers = parseSolverInfo(readFile(iter.path));
    vector<string> solver_names;
    for (auto& iter : solvers) solver_names.push_back(iter.name);
    if (solver_names.empty()) continue;
    // modify77(iter, solver_names);
    if (top > 0 && solver_names.size() > top) solver_names.resize(top);
    auto changes = calculator.calculateRatingChanges(data, solver_names);
    for (auto& change : changes) {
      if (data.count(change.first)) {
        data[change.first] += change.second;
      } else {
        data[change.first] =
            CodeforcesRatingCalculator::INITIAL_RATING + change.second;
      }
      max_rating[change.first] =
          max(max_rating[change.first], data[change.first]);
    }
  }

  vector<pair<int, string> > vec;
  for (auto& iter : data) {
    vec.push_back({iter.second, iter.first});
  }
  sort(vec.begin(), vec.end(),
       [=](const pair<int, string>& a, const pair<int, string>& b) {
         if (a.first != b.first) return a.first > b.first;
         return a.second < b.second;
       });

  const int size = vec.size();

  cout << "This rating is based on Codeforces' rating system." << endl;
  cout << "Author Mike Mirzayanov (mirzayanovmr@gmail.com)" << endl;
  cout << "http://codeforces.com/blog/entry/20762" << endl;
  cout << "http://codeforces.com/contest/1/submission/13861109" << endl;
  cout << endl;
  cout << "participants = " << size << endl;
  cout << endl;

  if (format == "json") {
    for (int i = 0; i < size; ++i) {
      const auto& iter = vec[i];
      cout << "(" << i + 1 << ",\"" << iter.second << "\"," << iter.first << ","
           << max_rating[iter.second] << ")," << endl;
    }
  } else {
    cout << left << setw(8) << "rank" << setw(24) << "id" << setw(10)
         << "rating" << setw(10) << "max rating" << endl;
    for (int i = 0; i < size; ++i) {
      const auto& iter = vec[i];
      cout << left << setw(8) << i + 1 << setw(24) << iter.second << setw(10)
           << iter.first << setw(10) << max_rating[iter.second] << endl;
    }
  }
}

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
