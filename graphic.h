// fichier : graphic.h
// auteurs : BIENAIMÉ Natacha n°315948 et HANGARD Kélyan n°312936
// version : 1.0
// architecture: Fig11 b1

#ifndef GRAPHIC_HEADER_H
#define GRAPHIC_HEADER_H

#include <iostream>
#include <cmath>
#include <cairomm/context.h>
#include <gtkmm.h>
#include "graphic_draw.h"

namespace graphic
{
  void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr);
  void set_clean();      
}

#endif
