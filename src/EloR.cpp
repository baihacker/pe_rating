#include "EloR.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
using namespace std;

ostream& operator<<(ostream& os, const Rating& r) {
  os << int(r.mu) << "+/-" << int(r.sig);
  return os;
}

// returns something near the mean if the ratings are consistent; near the
// median if they're far apart offC and offM are constant and slope offsets,
// respectively
double robustMean(const vector<Rating>& ratings, double offC, double offM) {
  double lo = -1000, hi = 5000;
  while (hi - lo > 1e-9) {
    double mid = (lo + hi) / 2;
    double sum = offC + offM * mid;
    for (const Rating& r : ratings) sum += tanh((mid - r.mu) / r.sig) / r.sig;
    if (sum > 0)
      hi = mid;
    else
      lo = mid;
  }
  return (lo + hi) / 2;
}

// ratings is a list of the participants, ordered from first to last place
// returns: performance of the player in ratings[id] who tied against
// ratings[lo..hi]
double performance(vector<Rating> ratings, int id, int lo, int hi) {
  int N = ratings.size();
  assert(0 <= lo && lo <= id && id <= hi && hi <= N - 1);
  double offset = 0;
  for (int i = 0; i < lo; ++i) offset += 1.0 / ratings[i].sig;
  for (int i = hi + 1; i < N; ++i) offset -= 1.0 / ratings[i].sig;
  ratings.push_back(ratings[id]);
  return robustMean(ratings, offset);
}

void handleHistory(map<string, Player>& players, const vector<string>& names,
                   const vector<int>& lo, const vector<int>& hi) {
  const int N = names.size();
  vector<Rating> compRatings;
  compRatings.resize(N);
  for (int i = 0; i < N; ++i) {
    Player& player = players[names[i]];
    player.setNameIfNecessary(names[i]);

    const Rating& r = player.posterior;
    double compVar =
        r.sig * r.sig + sig_noise * sig_noise + sig_perf * sig_perf;
    compRatings.emplace_back(r.mu, sqrt(compVar));
  }

  // begin rating updates
  for (int i = 0; i < N; ++i) {
    Player& player = players[names[i]];
    player.add_noise_uniform();

    double perf = performance(compRatings, i, lo[i], hi[i]);
    player.perfs.emplace_back(perf, sig_perf);

    player.prevRating = player.conservativeRating();
    player.updatePosterior();
    player.maxRating = max(player.maxRating, player.conservativeRating());
  }
}

void handleHistory(map<string, Player>& players, const vector<string>& names) {
  const int n = names.size();
  vector<int> rank(n, 0);
  for (int i = 0; i < n; ++i) rank[i] = i;
  handleHistory(players, names, rank, rank);
}