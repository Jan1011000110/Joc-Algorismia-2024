#include "Player.hh"

#include <vector>

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Harry

struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */
  
  /**
   * Helper functions
   */

  static vector<int> solve_spell (vector<int> a) {
    int N = (int) a.size();

    vector<int> pref(N + 1);
    for (int i = 0; i < N; ++i) {
      pref[i + 1] = pref[i] + a[i];
    }
    int S = pref[N];

    // dp[i][s1][s2] = -1 <=> not a reachable state
    // dp[i][s1][s2] = 0 <=> reachable state and we place i-th item into first group
    // dp[i][s1][s2] = 1 <=> reachable state and we place i-th item into second group
    // dp[i][s1][s2] = 2 <=> reachable state and we place i-th item into third group
    // dp[i][s1][s2] = 3 <=> reachable state and we havent placed nothigh yet
    vector<vector<vector<int>>> dp(N + 1, vector<vector<int>>(S + 1, vector<int>(S + 1, -1)));
    dp[0][0][0] = 3; // mark this state as a reachable one

    for (int i = 0; i < N; ++i) {
      for (int s1 = 0; s1 <= S; ++s1) {
        for (int s2 = 0; s2 <= S; ++s2) {
          if (dp[i][s1][s2] == -1) { // we haven't been able to reach this state
            continue;
          }
          if (s1 + a[i] <= S) { // place into first group
            dp[i + 1][s1 + a[i]][s2] = 0; 
          }
          if (s2 + a[i] <= S) { // place into second group
            dp[i + 1][s1][s2 + a[i]] = 1;
          }
          if (pref[i] - s1 - s2 + a[i] <= S) { // place into third group
            dp[i + 1][s1][s2] = 2;
          }
        }
      }
    }

    cerr << "HERE" << endl;
    // reconstruct the groups
    vector<int> groups;
    int s1 = S / 3, s2 = S / 3;
    for (int i = N; i >= 1; --i) {
      if (dp[i][s1][s2] == 0) {
        s1 -= a[i - 1];
        groups.push_back(0);
      }
      else if (dp[i][s1][s2] == 1) {
        s2 -= a[i - 1];
        groups.push_back(1);
      }
      else if (dp[i][s1][s2] == 2) {
        groups.push_back(2);
      }
      else {
        assert(false);
      }
    } 
    reverse(groups.begin(), groups.end());
    return groups;
  }

  void cast_spell() {
    int id = ghost(me());
    if (unit(id).resting_rounds() > 0) { // ghost can't cast spells yet
      return;
    }
    vector<int> a = spell_ingredients();
    vector<int> groups = solve_spell(a);
    // int s1 = 0, s2 = 0, s3 = 0;
    // for (int i = 0; i < (int) a.size(); ++i) {
    //   cerr << a[i] << " ";
    //   if (groups[i] == 0) s1 += a[i];
    //   else if (groups[i] == 1) s2 += a[i];
    //   else s3 += a[i];
    // }
    // cerr << endl;
    // cerr << s1 << " " << s2 << " " << s3 << endl;
    // cerr << "TOTAL SUM = " << s1+s2+s3 << endl;
    spell(id, groups);
  }

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    cast_spell();
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
