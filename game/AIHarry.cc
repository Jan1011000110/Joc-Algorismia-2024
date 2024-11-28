#include "Player.hh"

#include <vector>

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Harry

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

  const int THRESHOLD_DIST = 5;

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


  bool valid_pos(Pos p) {
    return pos_ok(p) and cell(p).type == Corridor;
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

  vector<vector<S>> BFS(vector<int> ids, bool is_ghost) {
    vector<Dir> dirs = (is_ghost ? ALL_DIRS : DIRS);
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

  vector<vector<S>> BFS_books(vector<Pos> pos) {
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

  void move_wizards() {
    vector<int> my_wizards = wizards(me());
    vector<Pos> books_pos = get_books_pos();

    map<int, bool> used_book, used_wizard;
    vector<vector<S>> M = BFS_books(books_pos);
    vector<pair<int, S>> candidates;
    for (int wizard_id : my_wizards) {
      Pos p = unit(wizard_id).pos;
      candidates.emplace_back(wizard_id, M[p.i][p.j]);
    }
    vector<int> order(candidates.size());
    iota(order.begin(), order.end(), 0LL);
    sort(order.begin(), order.end(), [&](int i, int j) {
      return candidates[i].second < candidates[j].second;
    });
    //sort(candidates.begin(), candidates.end());
    for (auto it : order) {
      int i = order[it];
      auto [wizard_id, s] = candidates[i];
      auto [book_id, d, p, dir] = s;
      if (not used_book[book_id]) {
        used_book[book_id] = true;
        used_wizard[wizard_id] = true;
        //cerr << pos << endl;
        //cerr << INVERSE_DIR[dir] << endl;
        move(wizard_id, INVERSE_DIR[dir]);
      }
    }
    for (auto wizard_id : my_wizards) {
      Pos p = unit(wizard_id).pos;
      if (not used_wizard[wizard_id]) {
        for (Dir dir : DIRS) {
          Pos np = p + dir;
          if (valid_pos(np) and cell(np).owner != me()) {
            move(wizard_id, dir);
          }
        }
      }
    }
  }

  void move_ghost() {
    vector<vector<S>> M = BFS({ghost(me())}, true);
    int min_d = INF;
    Pos voldemort_p = pos_voldemort();
    for (int i = 0; i < BOARD_ROWS; ++i) {
      for (int j = 0; j < BOARD_COLS; ++j) {
        Pos p = Pos(i, j);
        if (unit(cell(p).id).player != me() or p == voldemort_p) {
          min_d = min(min_d, M[i][j].d);
        }
      }
    }
    if (min_d > THRESHOLD_DIST) { // then get far away from voldemort
      Pos p = pos_voldemort();
      move(ghost(me()), INVERSE_DIR[M[p.i][p.j].from]);
    }
    else {
      for (int i = 0; i < BOARD_ROWS; ++i) {
        for (int j = 0; j < BOARD_COLS; ++j) {
          if (M[i][j].d == min_d) {
            Pos new_p = unit(ghost(me())).pos + INVERSE_DIR[M[i][j].from];
            if (valid_pos(new_p)) {
              move(ghost(me()), INVERSE_DIR[M[i][j].from]);
            }
          }
        }
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
