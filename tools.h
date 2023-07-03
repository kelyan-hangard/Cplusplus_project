// fichier : tools.h
// auteurs : BIENAIMÉ Natacha n°315948 et HANGARD Kélyan n°312936
// version : 3.0
// architecture: Fig11 b1

#ifndef TOOLS_HEADER_H
#define TOOLS_HEADER_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <sstream>
#include "constantes.h"
#include "error.h"

struct S2d
{
  double x;
  double y;
};

struct Segment
{
  S2d point1;
  S2d point2;
};

struct Cercle
{
  S2d centre;
  double rayon;
};

namespace tools
{
  enum Couleur {NOIR,ROUGE,VERT,BLEU};
  bool intersection_2_cercles(const Cercle& c1, const Cercle& c2, 
							  const double& dist_security);
  bool intersection_cercle_segment(const Cercle& c1, const Cercle& c2, 
								   const Cercle& c3);
  void draw_link(const Cercle& c1, const Cercle& c2, const Couleur& color);
  void draw_logement(const Cercle& c, const Couleur& color);
  void draw_production(const Cercle& c, const Couleur& color);
  void draw_transport(const Cercle& c, const Couleur& color);
  double calcul_distance_entre_2_points(const S2d& a, const S2d& b);
  bool point_dans_cercle(const S2d& p, const Cercle& c);
}

#endif
