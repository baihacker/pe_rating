#ifndef __ELOR_H__
#define __ELOR_H__

// author: Aram Ebtekar https://github.com/EbTech/EloR
// refactored by: baihacker

#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <tuple>
using namespace std;

const double sig_limit =
    100;  // limiting uncertainty for a player who competed a lot
const double sig_perf = 2222;   // variation in individual performances
const double sig_newbie = 350;  // uncertainty for a new player
const double sig_noise =
    sqrt(1.0 / (1.0 / sig_limit / sig_limit - 1.0 / sig_perf / sig_perf) -
         sig_limit * sig_limit);

struct Rating {
  Rating() = default;
  Rating(double m, double s) : mu(m), sig(s) {}  // create player

  double mu;   // mean of skill belief
  double sig;  // uncertainty of skill belief
};

ostream& operator<<(ostream& os, const Rating& r);
double robustMean(const vector<Rating>& ratings, double offC = 0,
                  double offM = 0);

struct Player {
  typedef tuple<int, string, int, int, int> RankTuple;
  Player()
      : maxRating(0),
        strongPrior(1500, sig_newbie),
        posterior(1500, sig_newbie) {}

  // apply noise to one variable for which we have many estimates
  void add_noise_uniform() {
    double decay =
        sqrt(1.0 + sig_noise * sig_noise / posterior.sig / posterior.sig);
    strongPrior.sig *= decay;
    for (Rating& r : perfs) r.sig *= decay;
  }

  void updatePosterior() {
    double sigInvSq = 1.0 / strongPrior.sig / strongPrior.sig;
    double mu = robustMean(perfs, -strongPrior.mu * sigInvSq, sigInvSq);
    for (const Rating& r : perfs) sigInvSq += 1.0 / r.sig / r.sig;
    posterior = Rating(mu, 1.0 / sqrt(sigInvSq));
  }

  int conservativeRating() const  // displayed rating
  {
    return int(posterior.mu - 2 * (posterior.sig - sig_limit) + 0.5);
  }

  void setNameIfNecessary(const string& name) {
    if (this->name.length() == 0) {
      this->name = name;
    }
  }

  RankTuple toTuple() const {
    return make_tuple(-conservativeRating(), name, maxRating, prevRating,
                      perfs.back().mu);
  }

  string name;
  Rating strongPrior;  // future optimization: if perfs gets too long, merge
                       // results into strongPrior
  Rating posterior;
  int prevRating, maxRating;
  vector<Rating> perfs;
};

void handleHistory(map<string, Player>& players, const vector<string>& names,
                   const vector<int>& lo, const vector<int>& hi);
void handleHistory(map<string, Player>& players, const vector<string>& names);
#endif