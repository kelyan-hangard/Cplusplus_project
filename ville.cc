// fichier : ville.cc
// auteurs : BIENAIMÉ Natacha n°315948 et HANGARD Kélyan n°312936
// version : 3.0
// architecture: Fig11 b1

#include "ville.h"

using namespace std;

namespace
{
  enum Etat_lecture {NBO,LOGEMENT,NB1,TRANSPORT,NB2,PRODUCTION,NB3,LINKS};
  Ville city;
  double nb_noeud(const char& t);
  void save_noeud(ofstream& monFlux, const char& t);
  double nb_link();
  void save_lien(ofstream& monFlux);
  unsigned int choose_new_uid();
  void draw_shortest_path(unsigned int noeudfinal);
  vector <Quartier*> tab;
  Quartier* current_node(nullptr);
  bool success(true);
  double equilibreNuitJour(0.);
  double coutInfrastructure(0.);
  double meilleurTempsAcces;
}

void ville::lecture(string nom_fichier)
{
  string line;
  ifstream fichier(nom_fichier);

  if(!fichier.fail()) {
    while(getline(fichier >> ws,line)) {
      istringstream data(line);

      if(line[0]=='#')
        continue;

      if(line.size()<=max_line)
        city.decodage_ligne(line);
    }
    fichier.close();
  }
}

void ville::refresh_criteres()
{
  city.calcul_criteres();
}

void ville::draw_city()
{
//success représente le fait que la ville ne dispose d'aucune erreur	
  if (success) {
    for (size_t i(0); i < tab.size() ; ++i) {
	  tab[i]->draw_link();
	 }
    for (size_t i(0); i < tab.size() ; ++i) {
	  tab[i]->draw_node();
	}
  }
}

void ville::shortest_path_city()
{
  for (size_t i(0); i < tab.size() ; ++i) {
    if (tab[i]->get_current() and ((tab[i]->get_type()) == 'L' )) {
      draw_shortest_path(city.algo_dijkstra(tab[i]->get_uid(),'P'));
      draw_shortest_path(city.algo_dijkstra(tab[i]->get_uid(),'T')); 
	}   
  }  
}
	
void ville::reset_path()
{
  for (size_t i(0); i < tab.size() ; ++i) {
	tab[i]->set_shortest_path_color_active(false);
  }
} 
	
double ville::export_enj()
{
  return equilibreNuitJour;
}

double ville::export_ci()
{
  return coutInfrastructure;
}

double ville::export_mta()
{
  return meilleurTempsAcces;
}

bool ville::export_success()
{	
  if (success)
    return true;
  else 
    return false;
}

void ville::restart()
{
  for (size_t i(0); i < tab.size() ; ++i) {
    delete tab[i];
  }
  tab.clear();
  success=true;
}

void ville::save(string nom_fichier)
{
  ofstream monFlux(nom_fichier.c_str());
  
  if (monFlux) {
    monFlux << "# nb housing" << endl;
	double compte(nb_noeud('L'));//renvoie le nombre de noeuds du type renseigné
	monFlux << compte << endl;
	monFlux << "# housing:" << endl;
	save_noeud(monFlux, 'L');//sauvegarde les données propres 
	                        //à chaque noeud du type renseigné
	monFlux << endl;

	monFlux << "# nb transport" << endl;
	compte = nb_noeud('T');
	monFlux << compte << endl;
	monFlux << "# transport:" << endl;
	save_noeud(monFlux, 'T');
	monFlux << endl;

	monFlux << "# nb production" << endl;
	compte = nb_noeud('P');
	monFlux << compte << endl;
	monFlux << "# production:" << endl;
	save_noeud(monFlux, 'P');
	monFlux << endl;

	monFlux << "# nb link" << endl;
	monFlux << nb_link() << endl;
	save_lien(monFlux);
  }
}

void ville::change_current_node(S2d p)
{
//met à jour un nouveau noeud courant ou détruit celui déja existant
  for (size_t i(0); i < tab.size() ; ++i) {
    if (tools::point_dans_cercle (p, tab[i]->get_cercle())) {
	  if (!(tab[i]->get_current())) {
	     if (current_node != nullptr)
            current_node->set_current(false);
		 tab[i]-> set_current(true);
		 current_node = tab[i];
	  } else {
		tab[i]-> destruct_link();	  
		delete tab[i];
		tab.erase(tab.begin() +i);
		current_node = nullptr;   
	  }
	}
  }
}

void ville::click_right(S2d p)
{
  if (current_node != nullptr)
    current_node->moove_node(p, tab);
}

void ville::add_node(S2d p, char T)
{
  Quartier quartier(p, T, choose_new_uid());
  
  if ((!(quartier.new_node_overlap(tab))) and 
     (quartier.verif_superposition_links(tab)))
    tab.push_back(new Quartier(quartier));
    
  else if (quartier.new_node_overlap(tab)){
    cout << "Error adding node" << endl;
	cout << "Collision with existing node" << endl;
  }
    
  if (!(quartier.verif_superposition_links(tab))){
  cout << "Error adding node" << endl;
  cout << "Collision with existing link" << endl;	
  }
}

void ville::change_node_size(S2d p1, S2d p2)
{
  double r_debut(tools::
				calcul_distance_entre_2_points(current_node->get_cercle().centre, p1));
  double r_fin(tools::
               calcul_distance_entre_2_points(current_node->get_cercle().centre, p2));
  double new_cap (pow((current_node->get_cercle().rayon + (r_fin - r_debut)), 2));
  
  //compare les deux rayons des cercles correspondants au point cliqué et
  //au point relaché pour agrandir ou rétrécir le noeud en conséquence 
  if ((!((r_fin < r_debut) and (new_cap > current_node->get_nbp())))
       and (!((r_fin > r_debut) and (new_cap < current_node->get_nbp()))))
    current_node-> verif_new_cap(new_cap,tab);
}

bool ville::click_on_a_node(S2d p)
{
  for (size_t i(0); i < tab.size() ; ++i) {
    if (tools::point_dans_cercle(p, tab[i]->get_cercle()))
	  return true;  	
   }
  return false;
}
 
bool ville::current_node_active()
{
  if (current_node == nullptr)
	return false;
  else 
    return true;
}

bool ville::current_node_active_logement()
{
  if (ville::current_node_active() and (current_node->get_type() == 'L') ) 
    return true;
  else 
    return false;
}
 
void ville::reset_current_node()
{
  current_node->set_current(false);
  current_node = nullptr;	
}

void ville::edit_link(S2d p)
{
//soit on veut créer, soit on veut détruire un lien
  for (size_t i(0); i < tab.size() ; ++i) {
    if ((tools::point_dans_cercle(p, tab[i]->get_cercle()))
         and (!(current_node->neighbour_other_node(tab[i])))) {
	  if (current_node->connexion_verif(*tab[i], tab)) {
	    tab[i]->set_link(current_node);
		current_node->set_link(tab[i]);
	  }
	  else 
	    cout << "Error creating link" << endl;
	} else if ((tools::point_dans_cercle(p, tab[i]->get_cercle()))
                and ((current_node->neighbour_other_node(tab[i])))) {		
      current_node->erase_link(tab[i]->get_uid());
      tab[i]->erase_link(current_node->get_uid());
	}
  }
}

void Ville::decodage_ligne(const string& line) const
{
  istringstream data(line);
   
  static int etat(NBO), i(0), total(0);
  char type;
  switch (etat) 
  {
  case NBO:
    data >> total;
    i=0;
    if (total == 0)
	  etat = NB1;
    else
      etat = LOGEMENT;
    break;

  case LOGEMENT:
    ++i;
    if(i==total)
      etat = NB1;
    type = 'L';
    if (success)
      city.ajouter_noeud(data, type);
    break;

  case NB1:
    data >> total;
    i=0;
    if (total == 0)
      etat = NB2;
    else
      etat = TRANSPORT;
    break;

  case TRANSPORT:
    ++i;
    if (i==total)
      etat = NB2;
    type = 'T';
    if (success)
      city.ajouter_noeud(data, type);
    break;

  case NB2:
    data >> total;
    i=0;
    if (total == 0)
      etat = NB3;
    else
      etat = PRODUCTION;
    break;

  case PRODUCTION:
    ++i;
    if (i==total)
       etat = NB3;
    type = 'P';
    if (success)
      city.ajouter_noeud(data, type);
    break;

  case NB3:
    data >> total;
    i=0;
    if (total == 0)
      etat=NBO;
    else
      etat = LINKS;
    break;

  case LINKS:
    ++i;
    if(i==total) {
      etat = NBO;
	  i=0;
      total = 0;
    }
    if (success)
      city.ajouter_connexion(data);
    break;
  }
}

void Ville::calcul_criteres()
{
  meilleurTempsAcces = 0.;
  
  if (not tab.empty()) {
    coutInfrastructure = city.critere_ci();
    equilibreNuitJour = city.critere_enj();
    
    //reset pour un prochain calcul du CI 
    tab[0]->reset_compteur_ci();
    for (size_t i(0); i < tab.size(); ++i) {
    tab[i]->set_infrastructure(false);
    }
    
    bool existence_noeudL(false); 
    for(size_t i(0); i < tab.size(); ++i) {
      if((tab[i]->get_type())=='L')
		  existence_noeudL=true;
    }
    if (existence_noeudL) {
      unsigned int compteur_noeudsL(0);
      for(size_t i(0); i < tab.size(); ++i) {
        if((tab[i]->get_type())=='L') {
          ++compteur_noeudsL;
          meilleurTempsAcces +=city.critere_mta(tab[i]->get_uid(),'P');
          meilleurTempsAcces +=city.critere_mta(tab[i]->get_uid(),'T');
        }
      }
      meilleurTempsAcces=meilleurTempsAcces/compteur_noeudsL;
    } else 
      meilleurTempsAcces=0.; // si aucun noeud logement dans tab
  } else {
	coutInfrastructure = 0.;
    equilibreNuitJour = 0.;
  }
}

unsigned int Ville::algo_dijkstra (const unsigned int& d, const char& type_noeud_final)
{
  city.initialiser_tab_selon_d (d); // d est l'uid du noeud de départ

  vector<unsigned int> uids_tries;
  
  for (size_t i(0); i < tab.size(); ++i) {
    uids_tries.push_back(tab[i]->get_uid());
  }
  noeud::tri_selon_access(uids_tries,tab);
  
  unsigned int n;
  bool still_in(false);
  
  for (size_t i(0); i < tab.size(); ++i) {
    if (tab[i]->get_in())
      still_in=true;
  }
  while (still_in) {
    n = city.find_min_access(uids_tries);
    for (size_t i(0); i < tab.size(); ++i) {
      if ((tab[i]->get_uid()==n) && (tab[i]->get_type()==type_noeud_final))
        return n;
    }
    for (size_t i(0); i < tab.size(); ++i) {
      if ((tab[i]->get_uid())==n) {
        tab[i]->set_in(false);
        tab[i]->check_liens_in(tab, uids_tries);
      }
    }
    still_in=false;
    for (size_t i(0); i < tab.size(); ++i) {
      if (tab[i]->get_in()) 
        still_in=true;
    }
  }
  return no_link;
}

void Ville::ajouter_noeud(std::istringstream& data, const char& type_quartier)
{
  Quartier quartier(data, type_quartier);

  if(quartier.quartier_verif(tab))
  	tab.push_back(new Quartier(quartier));
  else
	success=false;
}

void Ville::ajouter_connexion(istringstream& data)
{
  unsigned int uid1;
  data >> uid1;
  unsigned int uid2;
  data >> uid2;

  if ((noeud::node_exists(uid1, tab)) and (noeud::node_exists(uid2, tab))) {
    for (size_t i(0); i < tab.size(); ++i) {
      for (size_t j(0); (j < tab.size()); ++j) {
      	if ((tab[i]->get_uid() == uid1) and (tab[j]->get_uid() == uid2)) {
		  if ((*tab[i]).connexion_verif(*tab[j], tab)){
		    tab[i]->set_link(tab[j]);
			tab[j]->set_link(tab[i]);
		  } else
			  success=false;
    	}
  	  }
	}
  } else
	  success=false;
}

double Ville::critere_enj() const
{
  double nbpL(0.), nbpP(0.), nbpT(0.);
  
  for (size_t i(0); i < tab.size(); ++i) {
    if (tab[i]->get_type()=='L')
      nbpL+=tab[i]->get_nbp();
    if (tab[i]->get_type()=='P')
      nbpP+=tab[i]->get_nbp();
    if (tab[i]->get_type()=='T')
      nbpT+=tab[i]->get_nbp();
  }
  return (nbpL - (nbpT + nbpP)) / (nbpL + nbpT + nbpP);
}

double Ville::critere_ci()
{
  for (size_t i(0); i < tab.size(); ++i) {
    tab[i]->evaluation_connexions();
    tab[i]->set_infrastructure(true);
  }
  return Quartier::get_compteur_ci();
}

double Ville::critere_mta(const unsigned int& d,const char& type_noeud_final) const
{
  double critere(0.);

  unsigned int noeudFinal (city.algo_dijkstra(d,type_noeud_final));

  if (noeudFinal==no_link)
    return infinite_time;

  for (size_t i(0); i < tab.size(); ++i) {
    if (noeudFinal==(tab[i]->get_uid()))
      critere += tab[i]->get_access();
  }
  return critere;
}

void Ville::initialiser_tab_selon_d (const unsigned int& d)
{
  for (size_t i(0); i < tab.size(); ++i) {
    tab[i]->set_in(true);
    tab[i]->set_access(infinite_time);
    tab[i]->set_parent(no_link);
    
    if (tab[i]->get_uid()==d) 
      tab[i]->set_access(0.);
  }
}

unsigned int Ville::find_min_access(const vector<unsigned int>& uids_tries) const
{
  for (size_t i (0); i < uids_tries.size(); ++i) {
    for (size_t j(0); j < tab.size(); ++j) {
      if ((tab[j]->get_uid()==uids_tries[i]) && (tab[j]->get_in()))
        return tab[j]->get_uid();
    }
  }
  return no_link; //instruction jamais atteinte
}
 
namespace
{	
  double nb_noeud(const char& t)
  {
	int c(0);
	
	for (size_t i(0); i < tab.size() ; ++i) {
	  if (tab[i]-> get_type() == t)
		++c;
	 }
	return c;
  }

  void save_noeud(ofstream& monFlux, const char& t)
  {
	for (size_t i(0); i < tab.size() ; ++i) {
	  if (tab[i]-> get_type() == t) {
	    monFlux << "	" << tab[i]->get_uid()
		<< " " << tab[i]->get_cercle().centre.x
		<< " " << tab[i]->get_cercle().centre.y
		<< " " << tab[i]->get_nbp() << endl;
	  }
	}
  }

  double nb_link()
  {
	int c(0);

	for (size_t i(0); i < tab.size() ; ++i) {
	  for (size_t j(i); j < tab.size() ; ++j) {
	    if (tab[i]-> connexion_exists(tab[j]->get_uid()))
		  ++c;
	  }
	}
	return c;
  }

  void save_lien(ofstream& monFlux)
  {
	for (size_t i(0); i < tab.size() ; ++i) {
	  for (size_t j(i); j < tab.size() ; ++j) {
		if (tab[i]-> connexion_exists(tab[j]->get_uid())){
		  monFlux << "	" << tab[i]->get_uid()
		  << " " << tab[j]->get_uid() << endl;
		}
	  }
	}
  }
  
  unsigned int choose_new_uid() //permet de trouver un uid unique libre
  {
	unsigned int new_uid(0);
	bool used(false);
	do {
      used = false;
	  for (size_t i(0); i < tab.size() ; ++i) {
	    if (tab[i]->get_uid() == new_uid) {
	      used = true;
		  ++new_uid;
	    }
      }
    } while (used or (new_uid == no_link));
    
    return new_uid;
  }
    
  void draw_shortest_path(unsigned int noeudfinal)
  {
    for (size_t i(0); i < tab.size() ; ++i) {
      if((tab[i]->get_uid()==noeudfinal) and (tab[i]->get_access() != infinite_time)){
	    tab[i]->coloriage_noeuds(); 
	    if (tab[i]->get_type() == 'P') 
	      tab[i]->set_shortest_path_color_active(true);
	    if (tab[i]->get_type() == 'T')
	      tab[i]->set_shortest_path_color_active(true);
      }
    }
  }
}
