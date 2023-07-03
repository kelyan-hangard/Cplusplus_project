// fichier : projet.cc
// auteurs : BIENAIMÉ Natacha n°315948 et HANGARD Kélyan n°312936
// version : 3.0
// architecture: Fig11 b1

#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include "ville.h"
#include "gui.h"

using namespace std;

int main (int argc, char * argv[])
{
  if (argc==2)
    ville::lecture(argv[1]);
  
  auto app = Gtk::Application::create(argc=1, argv, "org.gtkmm.example");
  MyEvent eventWindow;
  return app->run(eventWindow);

  return 0;
}
