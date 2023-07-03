// fichier : ville.h
// auteurs : BIENAIMÉ Natacha n°315948 et HANGARD Kélyan n°312936
// version : 3.0
// architecture: Fig11 b1

#ifndef VILLE_HEADER_H
#define VILLE_HEADER_H

#include <fstream>
#include "noeud.h"

namespace ville
{
  void lecture (std::string nom_fichier);
  void refresh_criteres();
  void draw_city();
  void shortest_path_city();
  double export_enj();
  double export_ci();
  double export_mta();
  bool export_success();
  void restart ();
  void save(std::string nom_fichier);
  void change_current_node(S2d p);
  void click_right(S2d p);
  void add_node(S2d p, char T);	
  void change_node_size(S2d p1, S2d p2);
  bool click_on_a_node(S2d p);
  bool current_node_active();
  bool current_node_active_logement();
  void reset_current_node();
  void edit_link(S2d p);
  void reset_path();
}

class Ville
{
public:
  void decodage_ligne(const std::string& line) const;
  void calcul_criteres();
  unsigned int algo_dijkstra (const unsigned int& d, const char& type_noeud_final);
private:
  void ajouter_noeud(std::istringstream& data, const char& type_quartier);
  void ajouter_connexion(std::istringstream& data);
  double critere_enj() const;
  double critere_ci();
  double critere_mta(const unsigned int& d,const char& type_noeud_final) const;
  void initialiser_tab_selon_d (const unsigned int& d);
  unsigned int find_min_access (const std::vector<unsigned int>& uids_tries) const;
};

#endif
