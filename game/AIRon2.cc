#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Ron2


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
  using VI = vector<int>;
  using VVI = vector<VI>;
  using VB = vector<bool>;
  using VVB = vector<VB>;

  const vector<Dir> dirs = {Up,Down,Left,Right};

  int distV (Pos P, Pos V) {
    return abs(P.i - V.i) + abs(P.j - V.j);
  }

  bool encanteri2(vector<int> nombres, int i, vector<int>& resultats, vector<int>& solucions, vector<int>& grups) {
    if (i == 15) return true;

    for (int j = 0; j < 5; ++j) {
        if (resultats[j] - nombres[i] >= 0 and grups[j] > 0) {
            solucions[i] = j;
            resultats[j] -= nombres[i];
            --grups[j];
            if (encanteri2(nombres, i + 1, resultats, solucions, grups)) return true;
            ++grups[j];
            resultats[j] += nombres[i];
        }
    }

    return false;
  }

  void encanteri(vector <int>& nombre) {
    int suma = 0;
    for (int i = 0; i < 15; ++i) suma += nombre[i]; 
    suma /= 5;
    vector<int> resultats (5, suma);
    vector<int> grups(5, 3);
    vector <int> solucio (15, 0);
    encanteri2(nombre, 0, resultats, solucio, grups);
    nombre = solucio;
  }

  void bfs (queue <Pos>& posicions, int k, VVI& R) {
    queue <Pos> aux;
    while (not posicions.empty()) {
      Pos posnew = posicions.front();
      int i = posnew.i;
      int j = posnew.j;
      posicions.pop();
      if (R[i][j] == -1 and cell(i, j).type != Wall) {
          R[i][j] = k;
          for (auto d : dirs) {
            Pos pos = posnew + d;
            if (pos_ok(pos)) aux.push(pos);
          }
      }
  }
  if (not aux.empty()) bfs(aux, k + 1, R);
  } 

  bool escapa_voldemort(int id, Pos p, int distancia, int k, VVB& T) {
    if (distancia == 0) return false;
    if (k >= 5) return true;
    T[p.i][p.j] = true;
    for (auto d : dirs) {
      Pos posnew = p + d;

      if (pos_ok(posnew) and cell(posnew.i, posnew.j).type != Wall and not T[posnew.i][posnew.j]) {
        if (escapa_voldemort(id, posnew, distV(pos_voldemort(), posnew), k + 1, T)) {
          if (k == 0) move (id, d);
          return true;
        }
      }
    }
    return false;
  }

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    VI wiz = wizards(me());

    int g = ghost(me());

    Pos v = pos_voldemort();

    queue <Pos> posicions, enemics;

    VVI R(60, VI(60, -1));
    VVI T (60, VI(60, -1));
    for (int i = 0; i < 60; ++i) 
     for (int j = 0; j < 60; ++j) {
      if (cell(i, j).book) posicions.push(Pos(i, j));
      if (unit(cell(i, j).id).type == Ghost and unit(cell(i, j).id).player != me()) enemics.push(Pos (i, j));
      if (cell(i, j).id != -1 and unit(cell(i, j).id).type == Wizard and unit(cell(i, j).id).player != me()) enemics.push(Pos(i, j));
    }

    bfs (posicions, 0, R);
    bfs (enemics, 0, T);
    for (int id : wiz) {

      if (distV(v, unit(id).pos) <= 7) {
        VVB L (60, VB(60, false));
        escapa_voldemort(id, unit(id).pos, distV(v, unit(id).pos), 0, L);
      }

      if (round() > 50 and T[unit(id).pos.i][unit(id).pos.j] < R [unit(id).pos.i][unit(id).pos.j]) {
        for (auto d : dirs) {
          Pos posnew = unit(id).pos + d;
          if (pos_ok(posnew) and T[posnew.i][posnew.j] < T[unit(id).pos.i][unit(id).pos.j] and cell(posnew.i, posnew.j).type != Wall) {
            if (cell(posnew.i, posnew.j).id != -1) {move(id, d); break;}
            else if (unit(cell(posnew.i, posnew.j).id).player != me()) {move(id, d); break;}
          }
        }
      }
      for (auto d : dirs) {
        Pos posnew = unit(id).pos + d;
        if (pos_ok(posnew) and R[posnew.i][posnew.j] < R[unit(id).pos.i][unit(id).pos.j] and cell(posnew.i, posnew.j).type != Wall) {
          if (cell(posnew.i, posnew.j).id != -1) {move(id, d); break;}
          else if (unit(cell(posnew.i, posnew.j).id).player != me()) {move(id, d); break;}
        }
      }

      move (id, dirs[random(0, 3)]);
    }

    if (unit(g).rounds_pending == 0) {
      vector<int> nombre = spell_ingredients();
      encanteri(nombre);

      spell (g, nombre);
    }

   if (distV(v, unit(g).pos) <= 7) {
        VVB L (60, VB(60, false));
        escapa_voldemort(g, unit(g).pos, distV(v, unit(g).pos), 0, L);
      }
    if (round() < 150 and T[unit(g).pos.i][unit(g).pos.j] < 10) {
      for (auto d : dirs) {
        Pos posnew = unit(g).pos + d;
        if (pos_ok(posnew) and T[posnew.i][posnew.j] > T[unit(g).pos.i][unit(g).pos.j] and cell(posnew.i, posnew.j).type != Wall) {move(g, d); break;}
      }

    }
    for (auto d : dirs) {
        Pos posnew = unit(g).pos + d;
        if (pos_ok(posnew) and R[posnew.i][posnew.j] < R[unit(g).pos.i][unit(g).pos.j] and cell(posnew.i, posnew.j).type != Wall) {
          if (cell(posnew.i, posnew.j).id != -1) {move(g, d); break;}
          else if (unit(cell(posnew.i, posnew.j).id).player != me()) {move(g, d); break;}
      }
    }

    move (g, dirs[random(0, 3)]);
  }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
