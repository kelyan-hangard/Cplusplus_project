// fichier : noeud.h
// auteurs : BIENAIMÉ Natacha n°315948 et HANGARD Kélyan n°312936
// version : 3.0
// architecture: Fig11 b1

#ifndef NOEUD_HEADER_H
#define NOEUD_HEADER_H

#include "tools.h"

class Quartier
{
public :
	Quartier(std::istringstream& data, const char& type_quartier);
	Quartier(const S2d& p, const char& type_quartier, const unsigned int& new_uid);
	Cercle get_cercle() const {return cercle;}
	S2d get_centre() const{return cercle.centre;}
	unsigned int get_uid() const {return uid;}
	char get_type() const {return type;}
	unsigned int get_nbp() const {return nbp;}
	static double get_compteur_ci() {return compteur_ci;}
    bool get_in() const {return in;}
    double get_access() const {return access;}
    unsigned int get_parent() const {return parent;}
    bool get_current() const {return current;}
    std::vector<Quartier*> get_link() {return link;}
    void set_link(Quartier* pt_quartier) {link.push_back(pt_quartier);}
    void set_infrastructure(bool infrastructure_) {infrastructure=infrastructure_;}
    void reset_compteur_ci(){compteur_ci = 0.;}
    void set_in (bool in_) {in=in_;}
    void set_access (double access_) {access=access_;}
    void set_parent (unsigned int uid_) {parent=uid_;}
    void set_cap(double new_cap) {nbp=new_cap;}
    void set_current(bool cur) {current = cur;}
	bool quartier_verif(const std::vector<Quartier*>& tab) const;
	bool connexion_verif(const Quartier& q2, const std::vector<Quartier*>& tab) const;
    void check_liens_in(const std::vector<Quartier*>& tab,
						std::vector<unsigned int>& uids_tries);
    void coloriage_noeuds ();
	void draw_node();
	void draw_link();
	void draw_link_shortest_path(const unsigned int& uid_);
	void destruct_link();
	void erase_link(const unsigned int& uid1);
	bool connexion_exists(const unsigned int& uid2);
   //alimente compteur_ci avec chaque lien ayant le booléen infrastructure encore false
    void evaluation_connexions(); 
    bool new_node_overlap(const std::vector<Quartier*>& tab);
    void moove_node(const S2d& p, std::vector<Quartier*>& tab);
    void verif_new_cap(const double& new_nbp, std::vector<Quartier*>& tab);
    bool verif_superposition_links(const std::vector<Quartier*>& tab);
    bool neighbour_other_node(Quartier* neighbour);
    void set_shortest_path_color_active(bool set) {shortest_path_color_active = set;}
    bool get_shortest_path_color_active() const {return shortest_path_color_active;}
    double get_shortest_parent() const {return shortest_parent;}
    void set_shortest_parent(double new_value){shortest_parent = new_value;}
private :
	char type;
	unsigned int uid;
	Cercle cercle;
	unsigned int nbp;
	std::vector<Quartier*> link;
	bool infrastructure; 
    static double compteur_ci;
    bool in;
    double access;
    unsigned int parent;
    bool current;
    bool shortest_path_color_active;
	double shortest_parent;
	bool using_reserved_uid() const;
	bool capacity_ok() const;
	bool uid_already_used(const std::vector<Quartier*>& tab) const;
	bool two_nodes_overlap(const std::vector<Quartier*>& tab) const;
	bool link_already_exists(const Quartier& q2) const;
	bool max_link_reached() const;
	bool node_link_overlap(const Quartier& q2,const std::vector<Quartier*>& tab)const;
	bool refers_twice(const Quartier& q2) const;
};

namespace noeud
{
  bool node_exists(const unsigned int& uid_, const std::vector<Quartier*>& tab);
				   std::ostream& operator<<(std::ostream& os, const Quartier& node);
  void tri_selon_access(std::vector<unsigned int>& uids_tries,
			            const std::vector<Quartier*>& tab);
}

#endif
