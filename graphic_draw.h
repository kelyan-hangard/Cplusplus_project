// fichier : graphic_draw.h
// auteurs : BIENAIMÉ Natacha n°315948 et HANGARD Kélyan n°312936
// version : 2.0
// architecture: Fig11 b1

#include <iostream>
#include <gtkmm.h>

struct Code_rgb
{
  double r;
  double g;
  double b;
};

namespace graphic_draw
{
  void draw_segment(const double& x1, const double& y1, const double& r1, 
					const double& x2, const double& y2, const double& r2, 
					const Code_rgb& rgb);
  void draw_circle(const double& x, const double& y, const double& r, 
				   const Code_rgb& rgb);
  void draw_rectangle(const double& x, const double& y, const double& r, 
					  const Code_rgb& rgb);                
  void draw_star_circle(const double& x, const double& y, const double& r, 
					    const Code_rgb& rgb);
}
