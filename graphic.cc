// fichier : graphic.cc
// auteurs : BIENAIMÉ Natacha n°315948 et HANGARD Kélyan n°312936
// version : 2.0
// architecture: Fig11 b1

#include "graphic.h"

using namespace std;

static const Cairo::RefPtr<Cairo::Context>* ptcr(nullptr);

void graphic::graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr)
{
  static bool init(false);
	
  if(!init) {
    ptcr = &cr;
	init = true;
  }
  (*ptcr)->set_line_width(5.0);
} 

void graphic::set_clean()
{
  (*ptcr)->set_source_rgb(1.,1.,1.);
  (*ptcr)->paint();
} 

void graphic_draw::draw_segment(const double& x1, const double& y1, const double& r1, 
								const double& x2, const double& y2, const double& r2, 
								const Code_rgb& rgb)
{
  (*ptcr)->set_source_rgb(rgb.r,rgb.g,rgb.b);
  (*ptcr)->move_to(x1, y1);
  (*ptcr)->line_to(x2, y2);
  (*ptcr)->stroke();
  
  (*ptcr)->set_source_rgb(1.,1.,1.);
  (*ptcr)->arc(x1, y1, r1, 0, M_PI * 2);
  (*ptcr)->fill();
  (*ptcr)->arc(x2, y2, r2, 0, M_PI * 2);
  (*ptcr)->stroke();
}

void graphic_draw::draw_circle(const double& x, const double& y, const double& r, 
							   const Code_rgb& rgb)
{
  (*ptcr)->set_source_rgb(rgb.r,rgb.g,rgb.b);
  (*ptcr)->move_to(x,y);
  (*ptcr)->arc(x, y, r, 0, M_PI * 2);
  (*ptcr)->set_source_rgb(1,1,1);
  (*ptcr)->fill();
  (*ptcr)->set_source_rgb(rgb.r,rgb.g,rgb.b);
  (*ptcr)->arc(x, y, r, 0, M_PI * 2);
  
  (*ptcr)->stroke(); 	
}
  
void graphic_draw::draw_rectangle(const double& x, const double& y, const double& r, 
								  const Code_rgb& rgb)
{
  (*ptcr)->set_source_rgb(rgb.r,rgb.g,rgb.b);
  (*ptcr)->move_to(x-(0.75*r), y+(0.125*r));
  (*ptcr)->line_to(x+(0.75*r), y+(0.125*r));
  (*ptcr)->line_to(x+(0.75*r), y-(0.125*r));
  (*ptcr)->line_to(x-(0.75*r), y-(0.125*r));
  (*ptcr)->line_to(x-(0.75*r), y+(0.125*r));
	
  (*ptcr)->stroke();	
}
  
void graphic_draw::draw_star_circle(const double& x, const double& y, const double& r, 
									const Code_rgb& rgb)
{
  (*ptcr)->set_source_rgb(rgb.r,rgb.g,rgb.b);
  
  (*ptcr)->move_to(x+r, y);
  (*ptcr)->line_to(x-r, y);
  (*ptcr)->stroke();
	
  (*ptcr)->move_to(x, y+r);
  (*ptcr)->line_to(x, y-r);
  (*ptcr)->stroke();
	
  (*ptcr)->move_to(x+((sqrt(2)/2)*r), y+((sqrt(2)/2)*r));
  (*ptcr)->line_to(x-((sqrt(2)/2)*r), y-((sqrt(2)/2)*r));
  (*ptcr)->stroke();
	
  (*ptcr)->move_to(x-((sqrt(2)/2)*r), y+((sqrt(2)/2)*r));
  (*ptcr)->line_to(x+((sqrt(2)/2)*r), y-((sqrt(2)/2)*r));
  (*ptcr)->stroke();
}
