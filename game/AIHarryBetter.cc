#include "Player.hh"

#include <vector>

#define sz(a) static_cast<int>(a.size())
#define dbg(a) cerr << #a << " = " << a << endl;

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME HarryBet

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

  const int THRESHOLD_ESCAPE = 7;
  const int THRESHOLD_ATTACK_GHOST = 5;

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

  int dist(Pos p, Pos q) {
    return abs(p.i-q.i) + abs(p.j-q.j);
  }

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


  bool valid_pos(Pos p) {
    // if (not (cell(p).id == -1 or unit(cell(p).id).player != me() or unit(cell(p).id).is_in_conversion_process())) {
    //   dbg(p) dbg(cell(p).id) dbg(unit(cell(p).id).player)
    // }

    return pos_ok(p) and cell(p).type == Corridor;
    //and (cell(p).id == -1 or unit(cell(p).id).player != me() or unit(cell(p).id).is_in_conversion_process());
  }

  bool empty_pos(Pos p) {
    return valid_pos(p) and (cell(p).is_empty() or cell(p).book);
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

  vector<vector<S>> BFS_id(vector<int> ids, bool all_dirs) {
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
        if (valid_pos(new_p) and M[new_p.i][new_p.j].d == INF) {
          M[new_p.i][new_p.j] = S(s.id, s.d + 1, s.pos, (s.d == 0 ? dir : s.from));
          q.push({new_p, M[new_p.i][new_p.j]});
        }
      }
    }
    return M;
  }

  vector<vector<S>> BFS_pos(vector<Pos> pos, bool all_dirs) {
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
        if (valid_pos(new_p) and M[new_p.i][new_p.j].d == INF) {
          M[new_p.i][new_p.j] = S(s.id, s.d + 1, s.pos, dir);
          q.push({new_p, M[new_p.i][new_p.j]});
        }
      }
    }
    return M;
  }

  bool try_move(int id, bool all_dirs, bool attack, bool get_closer, vector<vector<int>> &D) {
    vector<Dir> dirs = (all_dirs ? ALL_DIRS : DIRS);
    Pos p = unit(id).pos;
    for (Dir dir : dirs) {
      Pos new_p = p + dir;
      if ((attack ? valid_pos(new_p) : empty_pos(new_p)) and (get_closer ? D[new_p.i][new_p.j] < D[p.i][p.j] : D[new_p.i][new_p.j] > D[p.i][p.j])) {
        move(id, dir);
        return true;
      }
    }
    for (Dir dir : dirs) {
      Pos new_p = p + dir;
      if ((attack ? valid_pos(new_p) : empty_pos(new_p) and D[new_p.i][new_p.j] == D[p.i][p.j])) {
        move(id, dir);
        return true;
      }
    }
    return false;
  }


  bool try_move(int id, bool all_dirs, bool attack, bool get_closer, vector<vector<S>> &M) {
    vector<vector<int>> D(BOARD_ROWS, vector<int>(BOARD_COLS));
    for (int i = 0; i < BOARD_ROWS; ++i) for (int j = 0; j < BOARD_COLS; ++j) D[i][j] = M[i][j].d;
    return try_move(id, all_dirs, attack, get_closer, D);
  }

  void move_wizards() {
    // TODO: HELP NEARBY WIZARDS THAT ARE CONVERTING
    vector<int> my_wizards = wizards(me());
    map<int, bool> used_wizards;

    // MOVE WIZARDS IN CONVERTING PROCESS TO GOOD WIZARDS SO THEY HEAL, MAYBE MOVE GOOD WIZARDS TO GET CLOSER
    vector<int> bad_wizards, good_wizards;
    for (int wizard_id : my_wizards) {
      if (unit(wizard_id).is_in_conversion_process()) {
        bad_wizards.push_back(wizard_id);
      }
      else {
        good_wizards.push_back(wizard_id);
      }
    }
    vector<vector<S>> M_good = BFS_id(good_wizards, false);
    vector<vector<S>> M_bad = BFS_id(bad_wizards, false);
    for (int wizard_id : bad_wizards) {
      if (used_wizards.count(wizard_id)) {
        continue;
      }
      Pos p = unit(wizard_id).pos;
      bool reachable = unit(wizard_id).rounds_pending > M_good[p.i][p.j].d;
      if (reachable and try_move(wizard_id, false, false, true, M_good)) {
        used_wizards[wizard_id] = true;
      }
    }
    for (int wizard_id : good_wizards) {
      if (used_wizards.count(wizard_id)) {
        continue;
      }
      Pos p = unit(wizard_id).pos;
      auto [bad_id, d, bad_p, dir] = M_bad[p.i][p.j];
      bool reachable = unit(bad_id).rounds_pending > M_bad[p.i][p.j].d;
      if (reachable and try_move(wizard_id, false, true, true, M_bad)) {
        used_wizards[wizard_id] = true;
      }
    }

    // MOVE WIZARDS TO ATTACK GHOST  
    vector<int> other_ghosts;
    map<int, bool> attacked_ghosts;
    for (int pl = 0; pl < 4; ++pl) if (pl != me()) other_ghosts.push_back(ghost(pl));
    vector<vector<S>> M_ghosts = BFS_id(other_ghosts, false);
    for (int wizard_id : my_wizards) {
      if (used_wizards.count(wizard_id)) {
        continue;
      }
      Pos p = unit(wizard_id).pos;
      auto [ghost_id, d, ghost_p, dir] = M_ghosts[p.i][p.j];
      bool reachable = d < THRESHOLD_ATTACK_GHOST and not attacked_ghosts.count(ghost_id);
      if (reachable and unit(ghost_id).resting_rounds() == 0 and try_move(wizard_id, false, true, true, M_ghosts)) {
        used_wizards[wizard_id] = true;
        attacked_ghosts[ghost_id] = true;
      }
    }

    // MOVE WIZARDS TO COLLECT BOOKS
    // TODO: CHECK IF SOME ENEMY WIZARD WILL REACH BEFORE ME
    vector<Pos> books_pos = get_books_pos();
    vector<vector<S>> M_books = BFS_pos(books_pos, false);
    for (int wizard_id : my_wizards) {
      if (used_wizards.count(wizard_id)) {
        continue;
      }
      Pos p = unit(wizard_id).pos;
      auto [book_id, d, book_p, dir] = M_books[p.i][p.j];
      if (valid_pos(p + INVERSE_DIR[dir])) {
        move(wizard_id, INVERSE_DIR[dir]);
        used_wizards[wizard_id] = true;
      }
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
    vector<vector<S>> M = BFS_id(other_wizards, false);
    vector<vector<int>> D(BOARD_ROWS, vector<int>(BOARD_COLS));
    for (int i = 0; i < BOARD_ROWS; ++i) {
      for (int j = 0; j < BOARD_COLS; ++j) {
        D[i][j] = min(M[i][j].d, dist(Pos(i, j), pos_voldemort()));
      }
    }
    // POSSIBLE PROBLEM, THE GHOST MAY NOT MOVE
    Pos p = unit(ghost(me())).pos;
    if (D[p.i][p.j] < THRESHOLD_ESCAPE) {
      try_move(ghost(me()), true, false, false, D);
    }
    else {
      // LOOK FOR BOOKS
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
