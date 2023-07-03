// fichier : tools.cc
// auteurs : BIENAIMÉ Natacha n°315948 et HANGARD Kélyan n°312936
// version : 3.0
// architecture: Fig11 b1

#include "tools.h"
#include "graphic_draw.h"

using namespace std;

namespace
{
  S2d projetee_ortho (const Segment& segment, const S2d& point);
  Code_rgb set_rgb (tools::Couleur color);
}

bool tools::intersection_2_cercles(const Cercle& c1, const Cercle& c2, 
								   const double& dist_security )
{
  double dist (calcul_distance_entre_2_points (c1.centre ,c2.centre));
  double srayons (c1.rayon + c2.rayon);

  if (dist > srayons + dist_security)
    return false;

  return true;
}

bool tools::intersection_cercle_segment(const Cercle& c1, const Cercle& c2, 
										const Cercle& c3)
{
  Segment segment = {c1.centre, c2.centre};
  S2d proj(projetee_ortho (segment, c3.centre));
  double dist (calcul_distance_entre_2_points(proj, c3.centre));
  
  if (dist <= c3.rayon) {
    if((proj.x > segment.point1.x and proj.x < segment.point2.x) or
	   (proj.x > segment.point2.x and proj.x < segment.point1.x) or
	   (proj.y > segment.point1.y and proj.y < segment.point2.y) or
	   (proj.y > segment.point2.y and proj.y < segment.point1.y))
	  return true;
  }
  return false;
}

void tools::draw_link(const Cercle& c1, const Cercle& c2, const tools::Couleur& color) 
{
  graphic_draw::draw_segment(c1.centre.x, c1.centre.y, c1.rayon, c2.centre.x, 
							 c2.centre.y, c2.rayon, set_rgb(color));  		
}

void tools::draw_logement(const Cercle& c, const tools::Couleur& color)
{
  graphic_draw::draw_circle(c.centre.x, c.centre.y, c.rayon, set_rgb(color));	
}

void tools::draw_production(const Cercle& c, const tools::Couleur& color)
{
  graphic_draw::draw_circle(c.centre.x, c.centre.y, c.rayon, set_rgb(color));
  graphic_draw::draw_rectangle(c.centre.x, c.centre.y, c.rayon, set_rgb(color));
}

void tools::draw_transport(const Cercle& c, const tools::Couleur& color)
{
  graphic_draw::draw_circle(c.centre.x, c.centre.y, c.rayon, set_rgb(color));
  graphic_draw::draw_star_circle(c.centre.x, c.centre.y, c.rayon, set_rgb(color));
}

double tools::calcul_distance_entre_2_points(const S2d& a, const S2d& b)
{
  return sqrt ( pow((b.x - a.x),2) + pow((b.y - a.y),2) );
}

bool tools::point_dans_cercle(const S2d& p, const Cercle& c)
{
  if (calcul_distance_entre_2_points(p, c.centre) < c.rayon)
    return true;
  
  return false;
}

namespace
{
  S2d projetee_ortho (const Segment& segment, const S2d& point)
  {
    /* création d'une fonction affine d'équation y1=mx+p représentant la droite
    entre les deux points du segment : */
    double m ((segment.point2.y - segment.point1.y) /
              (segment.point2.x - segment.point1.x));
    double p (segment.point1.y - m * segment.point1.x);
   
    /*calcul de la valeur c correspondant à la constante de la droite d'équation -x-m*y2+c=0
    orthogonale au segment et passant pas le point à projeter*/
    double c ((m * point.y) + point.x);
    
    //résultat des solutions de l'équation y1(x) = y2(x) 
    S2d res;
    res.x = (c-m*p)/(1+(m*m)) ;
    res.y = m*res.x+p;
    
    return res; 
  }
  
  Code_rgb set_rgb(tools::Couleur color)
  {
    Code_rgb rgb;
	  
	switch (color) 
    {
	case tools::NOIR:
      rgb.r = 0;
      rgb.g = 0;
      rgb.b = 0;
	  break;
	  	
    case tools::ROUGE:
      rgb.r = 1;
      rgb.g = 0;
      rgb.b = 0;
	  break;

    case tools::VERT:
      rgb.r = 0;
      rgb.g = 1;
      rgb.b = 0;
	  break;
	
    case tools::BLEU:
      rgb.r = 0;
      rgb.g = 0;
      rgb.b = 1;
	  break;
    }
    return rgb;               	  
  }
}
