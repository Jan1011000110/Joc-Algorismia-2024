#include "Player.hh"

#include <vector>

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Harry2

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

  const int INF = 1E9;

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

  const int THRESHOLD_DIST = 7;

  const int NUM_INGREDIENTS = 15;
  const int NUM_GROUPS = 5;
  const int NUM_INGREDIENTS_IN_GROUP = NUM_INGREDIENTS/NUM_GROUPS;

  const vector<Dir> ALL_DIRS = {Down, DR, Right, RU, Up, UL, Left, LD};
  const vector<Dir> DIRS = {Down, Right, Up, Left};

  map<Dir, Dir> INVERSE_DIR = {
        {Down, Up},
        {Up, Down},
        {Left, Right},
        {Right, Left},
        {DR, UL},
        {RU, LD},
        {UL, DR},
        {LD, RU}
    };

  
  struct S {
    int id;
    int d;
    Pos pos;
    Dir from;
    S() : id(-1), d(1E9) {}
    S(int id_, int d_, Pos pos_, Dir from_) : id(id_), d(d_), pos(pos_), from(from_) {}
    bool operator <(const S &other) {
      return d < other.d;
    }
    friend bool operator <(const S &a, const S &b) {
      return a < b;
    }
    friend S max(const S &a, const S &b) {
      return a < b ? a : b;
    }
  };

  
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


  bool valid_pos(Pos p, bool is_vol) {
    return pos_ok(p) and (is_vol or cell(p).type == Corridor);
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

  vector<vector<S>> BFS_id(vector<int> ids, bool all_dirs, bool is_vol) {
    vector<Dir> dirs = (all_dirs ? ALL_DIRS : DIRS);
    vector<vector<S>> M(BOARD_ROWS, vector<S>(BOARD_COLS));
    queue<pair<Pos, S>> q;
    for (int id : ids) {
      Pos p = unit(id).pos;
      q.push({p, S(id, 0, p, Up)});
    }
    while (not q.empty()) {
      auto [p, s] = q.front();
      q.pop();
      for (Dir dir : dirs) {
        Pos new_p = p + dir;
        if (valid_pos(new_p, is_vol) and M[new_p.i][new_p.j].d == INF) {
          M[new_p.i][new_p.j] = S(s.id, s.d + 1, s.pos, (s.d == 0 ? dir : s.from));
          q.push({new_p, M[new_p.i][new_p.j]});
        }
      }
    }
    return M;
  }

  vector<vector<S>> BFS_pos(vector<Pos> pos, bool all_dirs, bool is_vol) {
    vector<Dir> dirs = (all_dirs ? ALL_DIRS : DIRS);
    vector<vector<S>> M(BOARD_ROWS, vector<S>(BOARD_COLS));
    queue<pair<Pos, S>> q;
    int id = 0;
    for (Pos p : pos) {
      q.push({p, S(id++, 0, p, Up)});
    }
    while (not q.empty()) {
      auto [p, s] = q.front();
      q.pop();
      for (Dir dir : DIRS) {
        Pos new_p = p + dir;
        if (valid_pos(new_p, is_vol) and M[new_p.i][new_p.j].d == INF) {
          M[new_p.i][new_p.j] = S(s.id, s.d + 1, s.pos, dir);
          q.push({new_p, M[new_p.i][new_p.j]});
        }
      }
    }
    return M;
  }

  void move_wizards() {
    // TODO: HELP NEARBY WIZARDS THAT ARE CONVERTING
    vector<int> my_wizards = wizards(me());
    int N = my_wizards.size();
    vector<bool> used_wizards(N, false);
    vector<int> bad_wizards, good_wizards;
    for (int wizard_id : my_wizards) {
      if (unit(wizard_id).is_in_conversion_process()) {
        bad_wizards.push_back(wizard_id);
      }
      else {
        good_wizards.push_back(wizard_id);
      }
    }
    vector<vector<S>> M_bad = BFS_id(good_wizards, false, false);
    for (int wizard_id : bad_wizards) {
      
    }
    
    vector<Pos> books_pos = get_books_pos();
    vector<vector<S>> M_books = BFS_pos(books_pos, false, false);
    for (int wizard_id : my_wizards) {
      if (used_wizards[wizard_id]) {
        continue;
      }
      Pos p = unit(wizard_id).pos;
      auto [book_id, d, book_p, dir] = M_books[p.i][p.j];
      move(wizard_id, INVERSE_DIR[dir]);
      used_wizards[wizard_id] = true;
    }
  }

  void move_ghost() {
    vector<int> other_wizards;
    for (int pl = 0; pl < 4; ++pl) {
      if (pl != me()) {
        vector<int> wz = wizards(pl);
        for (auto id : wz) other_wizards.push_back(id);
      }
    }
    vector<vector<S>> M_other = BFS_id(other_wizards, false, false);
    vector<vector<S>> M_vol = BFS_pos({pos_voldemort()}, true, true);
    vector<vector<S>> M(BOARD_ROWS, vector<S>(BOARD_COLS));
    for (int i = 0; i < BOARD_ROWS; ++i) {
      for (int j = 0; j < BOARD_COLS; ++j) {
        M[i][j] = M_other[i][j].d < M_vol[i][j].d ? M_other[i][j] : M_vol[i][j];
      }
    }
    // POSSIBLE PROBLEM, THE GHOST MAY NOT MOVE
    Pos ghost_p = unit(ghost(me())).pos;
    for (Dir dir : ALL_DIRS) {
      Pos new_p = ghost_p + dir;
      if (valid_pos(new_p, false) and cell(new_p).is_empty() and M[new_p.i][new_p.j].d > M[ghost_p.i][ghost_p.j].d) {
        move(ghost(me()), dir);
      }
    }
  }

  void move_units() {
    move_ghost();
    move_wizards();
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
