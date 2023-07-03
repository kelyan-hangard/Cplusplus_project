// fichier : noeud.cc
// auteurs : BIENAIMÉ Natacha n°315948 et HANGARD Kélyan n°312936
// version : 3.0
// architecture: Fig11 b1

#include "noeud.h"

using namespace std;
namespace
{
  double find_access_from_uid(const unsigned int& uid_, const vector<Quartier*>& tab);
  double compute_access(const vector<Quartier*>& tab, const unsigned int& n,
                        const unsigned int& lv);
}

Quartier::Quartier(std::istringstream& data, const char& type_quartier):
  type(type_quartier),infrastructure(false), current(false), 
  shortest_path_color_active(false)
{
  data >> uid;
  data >> cercle.centre.x;
  data >> cercle.centre.y;
  data >> nbp;
  cercle.rayon = sqrt(nbp);
}
  
Quartier::Quartier(const S2d& p, const char& type_quartier, 
				  const unsigned int& new_uid):
  type(type_quartier), uid(new_uid), infrastructure(false), current(false), 
  shortest_path_color_active(false)
{
  cercle.centre = p;
  nbp = min_capacity;
  cercle.rayon = sqrt(nbp);
}

bool Quartier::quartier_verif(const vector<Quartier*>& tab) const
{
  if(using_reserved_uid())
	return false;

  if(!capacity_ok())
    return false;

  if(uid_already_used(tab))
	return false;

  if(two_nodes_overlap(tab))
	return false;

  return true;
}

bool Quartier::connexion_verif(const Quartier& q2, const vector<Quartier*>& tab) const
{
  if(link_already_exists(q2))
	return false;

  if(max_link_reached() or q2.max_link_reached())
    return false;

  if(node_link_overlap(q2,tab))
	return false;

  if(refers_twice(q2))
	return false;

  return true;
}

void Quartier::check_liens_in(const vector<Quartier*>& tab, 
							  vector<unsigned int>& uids_tries)
{
  double alt;

  for (size_t i(0); i < link.size(); ++i) {
    if (link[i]->in) {
      alt = access + compute_access(tab,uid,link[i]->uid);
      if ((link[i]->access > alt) && !(type=='P')) {
        link[i]->set_access(alt);
        link[i]->set_parent(uid);
        noeud::tri_selon_access(uids_tries,tab);
      }
    }
  }
}

void Quartier::coloriage_noeuds ()
{
shortest_path_color_active = true;

  for (size_t i(0); i < link.size() ; ++i) {
    if (parent == (link[i]-> get_uid())) { 
	  link[i]->set_shortest_path_color_active(true);
	  set_shortest_parent(link[i]->get_uid());
	  link[i]->coloriage_noeuds();
    }
  }
}

void Quartier::draw_node()
{
  tools::Couleur color(tools::NOIR);
  
  if (current)
    color = tools::ROUGE;
  
  else if (shortest_path_color_active)
    color = tools::VERT;
 
  if (type=='L')
	tools::draw_logement(cercle, color);
		
  if (type=='P')
	tools::draw_production(cercle, color);
		
  if (type=='T')
	tools::draw_transport(cercle, color);	  
}

void Quartier::draw_link()
{
  for (size_t i(0); i < link.size() ; ++i) {
	if ((shortest_path_color_active) and (link[i]->get_shortest_path_color_active())
         and ((shortest_parent == (link[i]-> get_uid())) 
         or (uid == link[i]->get_shortest_parent())))  
      tools::draw_link(cercle, link[i]-> get_cercle(), tools::VERT);
    else 
      tools::draw_link(cercle, link[i]-> get_cercle(), tools::NOIR);
  }
}

void Quartier::destruct_link()
{
  for (size_t i(0); i < link.size() ; ++i) {
    link[i]->erase_link(uid);
  }
  link.clear();
}

void Quartier::erase_link(const unsigned int& uid1)
{
  for (size_t i(0); i < link.size() ; ++i) {
	if (link[i]->get_uid() == uid1)
	  link.erase(link.begin() + i);
  }
}
		
bool Quartier::connexion_exists(const unsigned int& uid2)
{
  for (size_t i(0); i < link.size() ; ++i) {
    if (link[i]->get_uid() == uid2)
	  return true;
  }
  return false;
}

double Quartier::compteur_ci(0.);

void Quartier::evaluation_connexions() 
{
  for(size_t i(0); i<link.size(); ++i) {
    double memo;
    if (!link[i]->infrastructure) {
      memo=tools::calcul_distance_entre_2_points(cercle.centre,link[i]->cercle.centre);
      if (nbp<=link[i]->nbp) {
        memo = memo*nbp;
      } else {
        memo = memo*(link[i]->nbp);
      }
      if ((type=='T')&&(link[i]->type=='T')) {
        memo = memo*fast_speed;
      } else {
        memo = memo*default_speed;
      }
      compteur_ci += memo;
    }
  }
}

bool Quartier::new_node_overlap(const std::vector<Quartier*>& tab)
{
  for (size_t i(0); i < tab.size(); ++i) {
    if (tab[i]->uid != uid) {
	  Cercle c2;
	  c2.centre = tab[i]->cercle.centre;
	  c2.rayon = tab[i]->cercle.rayon;

	  if (tools::intersection_2_cercles(cercle, c2, dist_min)) {
		return true;
	  }	  
	}
  }
  return false;	
}

void Quartier::moove_node(const S2d& p, std::vector<Quartier*>& tab)
{
  S2d centre_ref(cercle.centre);
  cercle.centre = p; 
	
  if (new_node_overlap(tab)){
	cercle.centre = centre_ref;
	cout << "Error moving node" << endl;
	cout << "Some links collide with some nodes" << endl;
  }
  if (!verif_superposition_links(tab)){
	cercle.centre = centre_ref;
	cout << "Error moving node" << endl;
	cout << "Some links collide with some nodes" << endl;
  }
  
  for (size_t i(0); i < link.size() ; ++i){
    if (node_link_overlap(*(link[i]), tab)){
	  cercle.centre = centre_ref;
	  cout << "Error moving node" << endl;
	  cout << "Some links collide with some nodes" << endl;
    }
  }	
}

void Quartier::verif_new_cap(const double& new_nbp, std::vector<Quartier*>& tab)
{
  if ((new_nbp > min_capacity) and (new_nbp < max_capacity)) {
    double nbp_ref(nbp);
	nbp = new_nbp;
	cercle.rayon = sqrt(nbp);
	  
	if (new_node_overlap(tab)) {
	  nbp = nbp_ref; 
	  cercle.rayon = sqrt(nbp);
	  cout << "Error changing size" << endl;
	  cout << "Some links collide with some nodes" << endl;
	}
	if (!verif_superposition_links(tab)) {
	  nbp = nbp_ref;
	  cercle.rayon = sqrt(nbp);
	  cout << "Error changing size" << endl;
	  cout << "Some links collide with some nodes" << endl;
	}
	for (size_t i(0); i < link.size() ; ++i) {
	  if (node_link_overlap(*(link[i]), tab)) {
	    nbp = nbp_ref;
	    cercle.rayon = sqrt(nbp);
	    cout << "Error changing size" << endl;
	    cout << "Some links collide with some nodes" << endl;
	  }
	}
  } else if (new_nbp < min_capacity) {
    nbp = min_capacity;
	cercle.rayon = sqrt(nbp);
  } else if (new_nbp > max_capacity){
    nbp = max_capacity;
	cercle.rayon = sqrt(nbp);
  }
}

bool Quartier::verif_superposition_links(const std::vector<Quartier*>& tab)
{
  for (size_t i(0); i < tab.size(); ++i) {
     if (((tab [i]-> get_uid()) != uid)) {
	   for (size_t j(0); j < tab[i]->get_link().size() ; ++j) {
	     if ((tab[i]->get_link()[j]->get_uid() != uid) and 
	         (tools::intersection_cercle_segment(tab[i]->get_cercle(),
	          tab[i]->get_link()[j]->get_cercle(), cercle))) { 			  
		   return false;
		 }
       }
     }
   }
  return true;
}

bool Quartier::neighbour_other_node(Quartier* neighbour)
{
  for (size_t i(0); i < link.size(); ++i) {
	if (link[i] == neighbour)
	  return true;
  }
  return false;	  	  	
}

bool Quartier::using_reserved_uid() const
{
  if (uid == no_link) {
	cout << error::reserved_uid();
	return true;
  }
  return false;
}

bool Quartier::capacity_ok() const
{
  if (min_capacity > nbp) {
    cout << error::too_little_capacity(nbp);
	return false;
  }
  if (max_capacity < nbp) {
    cout << error::too_much_capacity(nbp);
	return false;
  }
  return true;
}

bool Quartier::uid_already_used(const vector<Quartier*>& tab) const
{
  for (size_t i(0); i < tab.size(); ++i) {
    if (uid == tab[i]->uid) {
      cout << error::identical_uid(uid);
	  return true;
    }
  }
 return false;
}

bool Quartier::two_nodes_overlap(const vector<Quartier*>& tab) const
{
  for (size_t i(0); i < tab.size(); ++i) {
    Cercle c2;
    c2.centre = tab[i]->cercle.centre;
    c2.rayon = tab[i]->cercle.rayon;

    if (tools::intersection_2_cercles(cercle, c2, 0)) {
	  cout << error::node_node_superposition(uid, tab[i] -> uid);
	  return true;
    }
  }
  return false;
}

bool Quartier::link_already_exists(const Quartier& q2) const
{
  for (size_t i(0); i < link.size(); ++i) {
    if (link[i]->get_uid() == q2.get_uid() ) {
      cout << error::multiple_same_link(uid, q2.get_uid());
      return true;
	}
  }
  return false;
}

bool Quartier::max_link_reached() const
{
  if (((type == 'L')) and (link.size() >= max_link)) {
    cout << error::max_link(uid);
	return true;
  }
  return false;
}

bool Quartier::node_link_overlap(const Quartier& q2, const vector<Quartier*>& tab)const
{
  for (size_t i(0); i < tab.size(); ++i) {
    if (((tab [i]-> get_uid()) != uid) and ( (tab[i]->get_uid()) != q2.get_uid()) ) {
	  if (tools::intersection_cercle_segment(cercle, q2.get_cercle(),
                                             tab[i]->get_cercle())) {
        cout << error::node_link_superposition(tab[i]->get_uid());
        return true;
      }
     }
   }
  return false;
}

bool Quartier::refers_twice(const Quartier& q2) const
{
  if (uid == q2.get_uid()) {
    cout << error::self_link_node(uid);
    return true;
  }
  return false;
}

bool noeud::node_exists(const unsigned int& uid_, const vector<Quartier*>& tab)
{
  for (size_t i(0); i<tab.size(); ++i) {
    if (tab[i]->get_uid() == uid_)
      return true;
	}
  cout << error::link_vacuum(uid_);
  return false;
}

void noeud::tri_selon_access(vector<unsigned int>& uids_tries, 
							 const vector<Quartier*>& tab)
{
  unsigned int j, tmp;
  
  for (size_t i(0); i < uids_tries.size(); ++i) {
    tmp = uids_tries[i];
    j=i;
    while (j>0 && 
           (find_access_from_uid(uids_tries[j-1],tab)>find_access_from_uid(tmp,tab))) {
      uids_tries[j]=uids_tries[j-1];
      j=j-1;
    }
    uids_tries[j]= tmp;
  }
}

namespace
{
  double find_access_from_uid(const unsigned int& uid_, const vector<Quartier*>& tab)
  {
    for (size_t i(0); i < tab.size(); ++i) {
      if ((tab[i]->get_uid())==uid_)
        return tab[i]->get_access();
    }
    return infinite_time;  //instruction jamais atteinte
  }

  double compute_access(const vector<Quartier*>& tab, const unsigned int& n,
                        const unsigned int& lv)
  {
    for (size_t i(0); i < tab.size(); ++i) {
      for (size_t j(0); j < tab.size(); ++j) {
        if (((tab[i]->get_uid())==n) and ((tab[j]->get_uid())==lv)){
          double dist(tools::calcul_distance_entre_2_points((tab[i]->get_centre()),
                                                             tab[j]->get_centre()));
          if((tab[i]->get_type()=='T') and (tab[j]->get_type()=='T'))
            return dist/fast_speed;
            else
              return dist/default_speed;
        }
      }
    }
	return infinite_time; //instruction jamais atteinte
  }
}
