#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Martinet


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
	const vector<Dir> dirs_mags = {Down, Right, Up, Left};
	const vector<Dir> dirs_fantasmes = {Down, DR, Right, RU, Up, UL, Left , LD};
	using VI = vector<int>;
	using VVI = vector<VI>;
  /**
   * Play method, invoked once per each round.
   */
	bool encanteri(int i, int s, VI& contador, VI& sumes, const VI& ingredients, VI& solucio){
		if (i == 15) return true;
		for (int j = 0; j < 5; ++j)
			if (contador[j] < 3){
				solucio[i] = j;
				++contador[j];
				sumes[j] += ingredients[i];
				if (sumes[j] <= s and not (contador[j] == 3 and sumes[j] != s))
					if (encanteri(i + 1, s, contador, sumes, ingredients, solucio)) 
						return true;
				--contador[j];
				sumes[j] -= ingredients[i];
			}
		return false;
	}
	
		
	virtual void play () {
		//parametres
		int precaucio = 3;
		int ronda_kamikaze = 200;
		int ronda_datacs = 50;
		int ganes_de_llibre = 30;
		int buscar_altres = 6;
		int fantasmes_rivals = 0;
		
		
		//matriu que em diu on hi ha llibres
		queue< pair<int, int> > cua;
		VVI llibres(board_rows(), VI(board_cols(), ganes_de_llibre));
		for (int i = 0; i < board_rows(); ++i)
			for (int j = 0; j < board_cols(); ++j)
				if (cell(i, j).type != Wall and cell(i, j).book){
					llibres[i][j] = 0;
					cua.push({i, j});
				}
		int it = 0;
		while (not cua.empty() and it < ganes_de_llibre){
			pair<int, int> pos = cua.front(); cua.pop();
			int i = pos.first;
			int j = pos.second;
			it = llibres[i][j];
			for (int k = max(0, i - 1); k < min(board_rows(), i + 2); ++k)
				if (pos_ok(k, j) and cell(k, j).type != Wall and llibres[k][j] > it + 1){
					llibres[k][j] = it + 1;
					cua.push({k, j});
				}
			
			for (int k = max(0, j - 1); k < min(board_cols(), j + 2); ++k)
				if (pos_ok(i, k) and cell(i, k).type != Wall and llibres[i][k] > it + 1){
				llibres[i][k] = it + 1;
				cua.push({i, k});
			}
		}
		
		//matriu que em diu on hi ha el rival per sota meu
		VVI rivals(board_rows(), VI(board_cols(), buscar_altres));
		if (round() > ronda_datacs){
			queue< pair<int, int> > q;
			for (int i = 0; i < board_rows(); ++i)
				for (int j = 0; j < board_cols(); ++j)
					if (cell(i, j).id != -1 and magic_strength(unit(cell(i, j).id).player) < magic_strength(me()) and unit(cell(i, j).id).type != Ghost){
						rivals[i][j] = 0;
						q.push({i, j});
					}
			int yt = 0;
			while (not q.empty() and yt < buscar_altres){
				pair<int, int> pos = q.front(); q.pop();
				int i = pos.first;
				int j = pos.second;
				yt = rivals[i][j];
				for (int k = max(0, i - 1); k < min(board_rows(), i + 2); ++k)
					if (pos_ok(k, j) and cell(k, j).type != Wall and rivals[k][j] > yt + 1){
						rivals[k][j] = yt + 1;
						q.push({k, j});
					}
				for (int k = max(0, j - 1); k < min(board_cols(), j + 2); ++k)
					if (pos_ok(i, k) and cell(i, k).type != Wall and rivals[i][k] > yt + 1){
					rivals[i][k] = yt + 1;
					q.push({i, k});
				}
			}
		}
		
		//matriu que em diu on hi ha els fantasmes rivals a punt de poder fer spell
		VVI fantasmes(board_rows(), VI(board_cols(), fantasmes_rivals));
		if (round() < 150){
			queue< pair<int, int> > q;
			for (int i = 0; i < board_rows(); ++i)
				for (int j = 0; j < board_cols(); ++j)
					if (cell(i, j).id != -1 and unit(cell(i, j).id).type == Ghost and unit(cell(i, j).id).rounds_pending < 10){
						fantasmes[i][j] = 0;
						q.push({i, j});
					}
			int yt = 0;
			while (not q.empty() and yt < fantasmes_rivals){
				pair<int, int> pos = q.front(); q.pop();
				int i = pos.first;
				int j = pos.second;
				yt = fantasmes[i][j];
				for (int k = max(0, i - 1); k < min(board_rows(), i + 2); ++k)
					if (pos_ok(k, j) and cell(k, j).type != Wall and fantasmes[k][j] > yt + 1){
						fantasmes[k][j] = yt + 1;
						q.push({k, j});
					}
				for (int k = max(0, j - 1); k < min(board_cols(), j + 2); ++k)
					if (pos_ok(i, k) and cell(i, k).type != Wall and fantasmes[i][k] > yt + 1){
					fantasmes[i][k] = yt + 1;
					q.push({i, k});
				}
			}
		}
		
		//matriu que em diu com d'aprop de voldemort estic
		//if (round() > ronda_kamikaze) precaucio = 199 - ronda_kamikaze;
		VVI voldemort(board_rows(), VI(board_cols(), precaucio));
		queue< pair<int, int> > Cua;
		voldemort[pos_voldemort().i][pos_voldemort().j] = 0;
		Cua.push({pos_voldemort().i, pos_voldemort().j});
		int It = 0;
		while (not Cua.empty() and It < precaucio){
			pair<int, int> pos = Cua.front(); Cua.pop();
			int i = pos.first;
			int j = pos.second;
			It = voldemort[i][j];
			for (int k = max(0, i - 1); k < min(board_rows(), i + 2); ++k)
				if (pos_ok(k, j) and voldemort[k][j] > It + 1){
					voldemort[k][j] = It + 1;
					Cua.push({k, j});
				}
			
			for (int k = max(0, j - 1); k < min(board_cols(), j + 2); ++k)
				if (pos_ok(i, k) and voldemort[i][k] > It + 1){
				voldemort[i][k] = It + 1;
				Cua.push({i, k});
			}
		}
						
		//moure mags
		for (int id : wizards(me())) {
			int L = voldemort[unit(id).pos.i][unit(id).pos.j];
			int l = llibres[unit(id).pos.i][unit(id).pos.j];
			int l2;
			if (round() > ronda_datacs) l2 = rivals[unit(id).pos.i][unit(id).pos.j];
			else l2 = fantasmes[unit(id).pos.i][unit(id).pos.j];
			
			//suicidarse
			if (round() > ronda_kamikaze and L < precaucio){
				for (Dir d : dirs_mags){
					Pos new_pos = unit(id).pos + d;
					if (pos_ok(new_pos) and voldemort[new_pos.i][new_pos.j] < L and cell(new_pos).type != Wall)
						move(id, d);
				}
			}
			//allunyarse de voldemort
			if (L < precaucio){
				for (Dir d : dirs_mags){
					Pos new_pos = unit(id).pos + d;
					if (pos_ok(new_pos) and voldemort[new_pos.i][new_pos.j] > L and cell(new_pos).type != Wall)
						move(id, d);
				}
				move(id, dirs_mags[random(0, 3)]);
			}
			//atacar altres
			else if (round() > ronda_datacs and l2 < buscar_altres)
				for (Dir d : dirs_mags){
					Pos new_pos = unit(id).pos + d;
					if (pos_ok(new_pos) and rivals[new_pos.i][new_pos.j] < l2)
						move(id, d);
				}
			//atacar fantasmes dels altres
			else if (round() < 150 and l2 < fantasmes_rivals)
				for (Dir d : dirs_mags){
					Pos new_pos = unit(id).pos + d;
					if (pos_ok(new_pos) and fantasmes[new_pos.i][new_pos.j] < l2)
						move(id, d);
				}
			
			
			//buscar llibres
			else if (l < ganes_de_llibre)
				for (Dir d : dirs_mags){
					Pos new_pos = unit(id).pos + d;
					if (pos_ok(new_pos) and llibres[new_pos.i][new_pos.j] < l)
						move(id, d);
				}
			//moute buscant celes lliures (no va gaire be diria)
			for (Dir d : dirs_mags){
				Pos new_pos = unit(id).pos + d;
				if (pos_ok(new_pos) and cell(new_pos).type != Wall and cell(new_pos).owner == -1)
					move(id, d);
			}
			for (Dir d : dirs_mags)
				if (pos_ok(unit(id).pos + d) and cell(unit(id).pos + d).type != Wall)
					move(id, d);
			
		}
		//matriu que diu al fantasma d'on escaparse
		queue< pair<int, int> > c;
		VVI allunyat_de_tot = voldemort;
		int I = unit(ghost(me())).pos.i;
		int J = unit(ghost(me())).pos.j;
		for (int i = max(0, I - precaucio); i < min(board_rows(), I + precaucio); ++i)
			for (int j = max(0, J - precaucio); j < min(board_cols(), J + precaucio); ++j)
				if (cell(i, j).id != -1 and unit(cell(i, j).id).player != me() and unit(cell(i, j).id).type != Ghost){
					allunyat_de_tot[i][j] = 0;
					c.push({i, j});
				}
		
		int iter = 0;
		while (not c.empty() and iter < precaucio){
			pair<int, int> pos = c.front(); c.pop();
			int i = pos.first;
			int j = pos.second;
			iter = allunyat_de_tot[i][j];
			for (int k = max(0, i - 1); k < min(board_rows(), i + 2); ++k)
				for (int s = max(0, j - 1); s < min(board_cols(), j + 2); ++s)
					if (pos_ok(k, s) and allunyat_de_tot[k][s] > iter + 1){
						allunyat_de_tot[k][s] = iter + 1;
						c.push({k, s});
					}
		}
		
		//fer encanteri
		if (unit(ghost(me())).rounds_pending == 0 and (round() >= 80 or round() < 80)){
			VI solucio(15);
			VI contador(5, 0);
			VI sumes(5, 0);
			VI ingredients = spell_ingredients();
			int suma = 0;
			for (int i = 0; i < 15; ++i) suma += ingredients[i];
			bool no_importa = encanteri(0, suma / 5, contador, sumes, ingredients, solucio);
			spell(ghost(me()), solucio);
		}
		
		//moure fantasma
		int r = allunyat_de_tot[I][J];
		int L = voldemort[I][J];
		int l = llibres[I][J];
		if (round() < 150 and L < precaucio)
			for (Dir d : dirs_fantasmes){
				Pos new_pos = unit(ghost(me())).pos + d;
				if (pos_ok(new_pos) and cell(new_pos).type != Wall and voldemort[new_pos.i][new_pos.j] > L )
					move(ghost(me()), d);
			}
		else if (round() < 150 and r < precaucio){
			for (Dir d : dirs_fantasmes){
				Pos new_pos = unit(ghost(me())).pos + d;
				if (pos_ok(new_pos) and cell(new_pos).type != Wall and allunyat_de_tot[new_pos.i][new_pos.j] > r)
					move(ghost(me()), d);
			}
			for (Dir d : dirs_fantasmes){
				Pos new_pos = unit(ghost(me())).pos + d;
				if (pos_ok(new_pos) and cell(new_pos).type != Wall and allunyat_de_tot[new_pos.i][new_pos.j] == r)
					move(ghost(me()), d);
			}
			move(ghost(me()), dirs_fantasmes[0]);
		}
		else if (l < ganes_de_llibre)
				for (Dir d : dirs_fantasmes){
					Pos new_pos = unit(ghost(me())).pos + d;
					if (pos_ok(new_pos) and llibres[new_pos.i][new_pos.j] < l)
						move(ghost(me()), d);
				}
		
	}
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);