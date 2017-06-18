/**
 * Author Mike Mirzayanov (mirzayanovmr@gmail.com)
 * http://codeforces.com/blog/entry/20762
 * http://codeforces.com/contest/1/submission/13861109
 *
 * Migrated to c++ by baihacker (bailiangsky@gmail.com)
 */
#include "pe_rating.h"

#include <cstdio>
#include <cmath>
#include <regex>
#include <iostream>
#include <iomanip>
#include <set>
#include <map>
using namespace std;

struct Contestant
{
  string party;
  double rank;
  double points;
  int rating;
  int needRating;
  double seed;
  int delta;

  Contestant(string party = "", int rank = 0, double points = 0, int rating = 0)
    : party(party), rank(rank), points(points), rating(rating), needRating(0), seed(0), delta(0)
  {
  }
};

class CodeforcesRatingCalculator
{
public:
  static const int INITIAL_RATING = 1500;

public:
  int aggregateRating(const vector<int>& ratingChanges)
  {
    int rating = INITIAL_RATING;

    for (auto ratingChange : ratingChanges) {
        rating += ratingChange;
    }

    return rating;
  }

  int getMaxRating(const vector<int>& ratingChanges)
  {
    int maxRating = 0;

    int rating = INITIAL_RATING;
    for (auto ratingChange : ratingChanges)
    {
        rating += ratingChange;
        maxRating = max(rating, maxRating);
    }

    return maxRating;
  }

  map<string, int> calculateRatingChanges(const map<string, int>& previousRatings,
      const vector<string>& standingsRows)
  {
    vector<Contestant> contestants;

    const int n = standingsRows.size();
    for (int i = 0; i < n; ++i)
    {
      const auto& name = standingsRows[i];
      int rank = i+1;
      int previousRating = INITIAL_RATING;
      auto where = previousRatings.find(name);
      if (where != previousRatings.end())
      {
        previousRating = where->second;
      }
      contestants.push_back({name, rank, 51-1.*rank, previousRating});
    }

    process(contestants);

    map<string, int> ratingChanges;
    for (auto contestant : contestants)
    {
      ratingChanges[contestant.party] = contestant.delta;
    }

    return ratingChanges;
  }

  static double getEloWinProbability(double ra, double rb)
  {
    return 1.0 / (1 + pow(10, (rb - ra) / 400.0));
  }


  static double getEloWinProbability(const Contestant& a, const Contestant& b)
  {
    return getEloWinProbability(a.rating, b.rating);
  }

  int composeRatingsByTeamMemberRatings(const vector<int>& ratings)
  {
    double left = 100;
    double right = 4000;

    for (int tt = 0; tt < 20; tt++)
    {
      double r = (left + right) / 2.0;

      double rWinsProbability = 1.0;
      for (auto rating : ratings)
      {
        rWinsProbability *= getEloWinProbability(r, rating);
      }

      double rating = log10(1 / (rWinsProbability) - 1) * 400 + r;

      if (rating > r)
      {
        left = r;
      }
      else
      {
        right = r;
      }
    }

    return (int) round((left + right) / 2);
  }

  double getSeed(const vector<Contestant>& contestants, int rating)
  {
    Contestant extraContestant("", 0, 0, rating);

    double result = 1;
    for (const auto& other : contestants)
    {
      result += getEloWinProbability(other, extraContestant);
    }

    return result;
  }

  int getRatingToRank(const vector<Contestant>& contestants, double rank)
  {
    int left = 1;
    int right = 8000;

    while (right - left > 1)
    {
      int mid = (left + right) / 2;

      if (getSeed(contestants, mid) < rank)
      {
        right = mid;
      }
      else
      {
        left = mid;
      }
    }

    return left;
  }

  void reassignRanks(vector<Contestant>& contestants)
  {
    sortByPointsDesc(contestants);

    for (auto& contestant : contestants)
    {
      contestant.rank = 0;
      contestant.delta = 0;
    }

    int first = 0;
    double points = contestants[0].points;
    for (int i = 1; i < contestants.size(); i++)
    {
      if (contestants[i].points < points)
      {
        for (int j = first; j < i; j++)
        {
            contestants[j].rank = i;
        }
        first = i;
        points = contestants[i].points;
      }
    }

    {
      double rank = contestants.size();
      for (int j = first; j < contestants.size(); j++)
      {
        contestants[j].rank = rank;
      }
    }
  }

  void sortByPointsDesc(vector<Contestant>& contestants)
  {
    sort(contestants.begin(), contestants.end(), [=] (const Contestant& a, const Contestant& b) {
      return a.points > b.points;
    });
  }

  void process(vector<Contestant>& contestants)
  {
    if (contestants.empty()) {
      return;
    }

    reassignRanks(contestants);

    for (auto& a : contestants)
    {
      a.seed = 1;
      for (const auto& b : contestants)
      {
        if (a.party != b.party)
        {
          a.seed += getEloWinProbability(b, a);
        }
      }
    }

    for (auto& contestant : contestants)
    {
      double midRank = sqrt(contestant.rank * contestant.seed);
      contestant.needRating = getRatingToRank(contestants, midRank);
      contestant.delta = (contestant.needRating - contestant.rating) / 2;
    }

    sortByRatingDesc(contestants);

    // Total sum should not be more than zero.
    {
      int sum = 0;
      for (const auto& c : contestants)
      {
        sum += c.delta;
      }
      int inc = -sum / (int)contestants.size() - 1;
      for (auto& contestant : contestants)
      {
        contestant.delta += inc;
      }
    }

    // Sum of top-4*sqrt should be adjusted to zero.
    {
      int sum = 0;
      int zeroSumCount = min((int) (4 * round(sqrt(contestants.size()))), (int)contestants.size());
      for (int i = 0; i < zeroSumCount; i++)
      {
        sum += contestants[i].delta;
      }
      int inc = min(max(-sum / zeroSumCount, -10), 0);
      for (auto& contestant : contestants)
      {
        contestant.delta += inc;
      }
    }

    validateDeltas(contestants);
  }

  void validateDeltas(vector<Contestant>& contestants)
  {
    sortByPointsDesc(contestants);

    for (int i = 0; i < contestants.size(); i++)
    {
      for (int j = i + 1; j < contestants.size(); j++)
      {
        if (contestants[i].rating > contestants[j].rating)
        {
          ensure(contestants[i].rating + contestants[i].delta >= contestants[j].rating + contestants[j].delta, string("First rating invariant failed: ") + contestants[i].party + " vs. " + contestants[j].party + ".");
        }
        if (contestants[i].rating < contestants[j].rating)
        {
          if (contestants[i].delta < contestants[j].delta)
          {
          }
          ensure(contestants[i].delta >= contestants[j].delta,
                  string("Second rating invariant failed: ") + contestants[i].party + " vs. " + contestants[j].party + ".");
        }
      }
    }
  }

  void ensure(bool b, const string& message)
  {
    if (!b)
    {
      cerr << message << endl;
      exit(-1);
    }
  }

  void sortByRatingDesc(vector<Contestant>& contestants)
  {
    sort(contestants.begin(), contestants.end(), [=] (const Contestant& a, const Contestant& b) {
      return a.rating > b.rating;
    });
  }
};

int getRoundNumber(const string& filename)
{
  int l = filename.length();
  return atoi(filename.substr(l-7, 3).c_str());
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

void genCfStatistics(const vector<string>& fileList, int top, const string& format)
{
  CodeforcesRatingCalculator calculator;
  map<string, int> data;
  map<string, int> max_rating;
  for (const auto& iter: fileList)
  {
    //if (getRoundNumber(iter) < 495) continue;
    vector<string> solvers = parseRanks(readFile(iter));
    if (solvers.empty()) continue;
    //modify77(iter, solvers);
    if (top > 0 && solvers.size() > top) solvers.resize(top);
    auto changes = calculator.calculateRatingChanges(data, solvers);
    for (auto& change : changes)
    {
      if (data.count(change.first))
      {
        data[change.first] += change.second;
      }
      else
      {
        data[change.first] = CodeforcesRatingCalculator::INITIAL_RATING + change.second;
      }
      max_rating[change.first] = max(max_rating[change.first], data[change.first]);
    }
  }

  vector<pair<int, string> > vec;
  for (auto& iter: data)
  {
    vec.push_back({iter.second, iter.first});
  }
  sort(vec.begin(), vec.end(), [=](const pair<int, string>& a, const pair<int, string>& b) {
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

  if (format == "json")
  {
    for (int i = 0; i < size; ++i)
    {
      const auto& iter = vec[i];
      cout << "(" << i + 1 << ",\"" << iter.second << "\"," << iter.first << "," << max_rating[iter.second] << ")," << endl;
    }
  }
  else
  {
    cout << left << setw(8) << "rank" << setw(24) << "id" << setw(10) << "rating" << setw(10) << "max rating" << endl;
    for (int i = 0; i < size; ++i)
    {
      const auto& iter = vec[i];
      cout << left << setw(8) << i + 1 << setw(24) << iter.second << setw(10) << iter.first << setw(10) << max_rating[iter.second] << endl;
    }
  }
}