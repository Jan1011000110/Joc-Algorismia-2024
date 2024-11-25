#include "Player.hh"

#include <vector>

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME SocUnPajerin

struct PLAYER_NAME : public Player {

  /**
   * Do not modify this function.
   * Factory: returns a new instance of this class.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */

  const int NUM_ROUNDS = 200;
  const int BOARD_ROWS = 60;
  const int BOARD_COLS = 60;

  const int POINTS_FOR_CONVERTING_WIZARD = 100;
  const int POINTS_PER_OWNED_CELL = 1;

  const int BOOK_MAGIC_STRENGTH = 50;

  const int ROUNDS_FOR_CONVERTING = 5;
  const int ROUNDS_ATTACK_RESTING_GHOST = 15;
  const int ROUNDS_SPELL_RESTING_GHOST = 60;	
  const int ROUNDS_NO_ATTACK_GHOST = 5;

  const int NUM_INGREDIENTS = 15;
  const int NUM_GROUPS = 5;
  const int NUM_INGREDIENTS_IN_GROUP = NUM_INGREDIENTS/NUM_GROUPS;

  const vector<Dir> ALL_DIRS = {Down, DR, Right, RU, Up, UL, Left, LD};
  const vector<Dir> DIRS = {Down, Right, Up, Left};

  
  /**
   * Helper functions
   */

  bool f(int i, int target, vector<int> &groups, vector<int> &sum, vector<int> &cnt, vector<int> &a) {
    if (i == NUM_INGREDIENTS) {
      return true;
    }
    for (int g = 0; g < NUM_GROUPS; ++g) {
      if (cnt[g] < NUM_INGREDIENTS_IN_GROUP and sum[g] + a[i] <= target) {
        groups[i] = g;

        cnt[g] += 1;
        sum[g] += a[i];
        if (f(i + 1, target, groups, sum, cnt, a)) {
          return  true;
        }
        cnt[g] -= 1;
        sum[g] -= a[i];
      }
    }
    return false;
  }

  vector<int> solve_spell (vector<int> &a) {
    int S = 0;
    for (int i = 0; i < NUM_INGREDIENTS; ++i) {
      S += a[i];
    }
    vector<int> groups(NUM_INGREDIENTS), sum(NUM_GROUPS), cnt(NUM_GROUPS);
    f(0, S / NUM_GROUPS, groups, sum, cnt, a);
    return groups;
  }

  void cast_spell() {
    int id = ghost(me());
    if (unit(id).resting_rounds() > 0 or round() > NUM_ROUNDS - 50) { // ghost is resting or last 50 rounds
      return;
    }
    vector<int> a = spell_ingredients();
    vector<int> groups = solve_spell(a);
    spell(id, groups);
  }


  bool valid_pos(Pos p, bool is_voldemort) {
    return pos_ok(p) and (is_voldemort or cell(p).type == Corridor);
  }

  vector<Pos> get_books_pos() {
    vector<Pos> books_pos;
    for (int i = 0; i < BOARD_ROWS; ++i) {
      for (int j = 0; j < BOARD_COLS; ++j) {
        Pos p(i, j);
        if (cell(p).book) {
          books_pos.push_back(p);
        }
      }
    }
    return books_pos;
  }

  vector<vector<int>> BFS(vector<Pos> &pos, bool is_voldemort) {
    vector<Dir> dirs = (is_voldemort ? ALL_DIRS : DIRS);
    vector<vector<int>> dist(BOARD_ROWS, vector<int>(BOARD_COLS, -1));
    queue<pair<Pos, int>> q;
    for (Pos p : pos) {
      q.push({p, 0});
      dist[p.i][p.j] = 0;
    }
    while (not q.empty()) {
      auto [p, d] = q.front();
      q.pop();
      for (Dir dir : dirs) {
        Pos new_p = p + dir;
        if (valid_pos(new_p, is_voldemort) and dist[new_p.i][new_p.j] == -1) {
          dist[new_p.i][new_p.j] = d + 1;
          q.push({new_p, d + 1});
        }
      }
    }
    return dist;
  }

  void move_wizards(vector<vector<int>> &dist_books) {

  }

  void move_units() {
    vector<Pos> books_pos = get_books_pos();
    vector<vector<int>> dist_books = BFS(books_pos, false);
  }

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    cast_spell();
    move_units();
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
