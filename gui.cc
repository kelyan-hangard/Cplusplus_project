// fichier : gui.cc
// auteurs : BIENAIMÉ Natacha n°315948 et HANGARD Kélyan n°312936
// version : 2.0
// architecture: Fig11 b1

#include <cairomm/context.h>
#include <iomanip>
#include "gui.h"
#include "ville.h"
#include "graphic.h"

using namespace std;

static bool city_read(true);

MyArea::MyArea(): edit_link_active(false), empty(false), zoom(1), path(false) 
{}

MyArea::~MyArea() {}

void MyArea::clear()
{
  empty = true; 
  refresh();
  empty = false;
}

void MyArea::draw()
{
  empty = false;
  refresh();
}
void MyArea::change_co(S2d& p, const double& width, const double& height)
{
	double newModelScale(width/height);
		
  if (width < height) {
	newModelScale=(2*dim_max)/width;
  } else {
	newModelScale=(2*dim_max)/height;
  }
  p.x  = (newModelScale * p.x - dim_max)*(1/zoom) ;
  p.y = (- newModelScale * p.y + dim_max)*(1/zoom);
}

void MyArea::click_left(S2d p, const double& width, const double& height, 
						const char& T)
{
  //changement des coordonées du point p de la fenêtre vers l'espace du modèle
  change_co(p, width, height);
  /*p1 retient la position du point cliqué 
  (pour la comparer plus tard à celle du point relaché)*/
  p1 = p;
  
  if ((!(edit_link_active)) and (!(ville::click_on_a_node(p))) 
	   and (!(ville::current_node_active())))
    ville::add_node(p, T);
  else if ((!(edit_link_active)) and (ville::click_on_a_node(p))){
	ville::change_current_node(p);
	if (path){ 
	  ville::reset_path();		
	  ville::shortest_path_city();
    }
  }
  if ((edit_link_active) and (ville::click_on_a_node(p)))
	ville::edit_link(p);

  refresh();
}

void MyArea::click_right(S2d p, const double& width, const double& height)
{
  change_co(p, width, height);
  ville::click_right(p);
  refresh();
}

void MyArea::zoom_change(const double& change)
{
  if ((zoom + change < max_zoom+epsil_zero) 
	   and (zoom +change > min_zoom-epsil_zero))
	 zoom = zoom + change ;
  refresh();
}

void MyArea::zoom_reset()
{
  zoom = 1;
  refresh();
}

void MyArea::stay_click(S2d p, const double& width, const double& height)
{
  change_co(p, width, height);
  
  //initialisation de la distance parcourue entre les deux cliques
  double mouse_distance(tools::calcul_distance_entre_2_points(p1,p));
  
  if ((mouse_distance > epsil_zero) and (!(ville::click_on_a_node(p1)))
      and (ville::current_node_active())) {
    ville::change_node_size(p1, p);
    refresh();
  }  
  if ((ville::current_node_active()) and (mouse_distance < epsil_zero)
     and (!(ville::click_on_a_node(p1)))){
    ville::reset_current_node();
    ville::reset_path();
    refresh();
  }
}

void MyArea::shortest_path_active()
{
  path = true;
  if (ville::current_node_active_logement()){
	ville::reset_path();  
    ville::shortest_path_city();
  }
  refresh();
}

void MyArea::shortest_path_non_active()
{
  path = false;
  ville::reset_path();
  refresh();
}

void MyArea::orthographic_projection(const Cairo::RefPtr<Cairo::Context>& cr, 
									  const int height, const int width)
{
  double newModelScale(width/height);
	
  if (width < height) {
	newModelScale=width/(2*dim_max);
	cr->translate(width/2, width/2);
  } else {
	newModelScale=height/(2*dim_max);
	cr->translate(height/2, height/2);
  }
  cr->scale(newModelScale*(zoom), (-1)*newModelScale*(zoom));
}

bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  if(not empty) { 
    Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	
    //changement de coordonées du modèle vers la fenêtre
	orthographic_projection(cr, height, width); 
	graphic::graphic_set_context(cr);
	
	if (ville::export_success() and city_read){
	  graphic::set_clean();
	  ville::draw_city();
	} 
	if (!ville::export_success() and city_read)
	  graphic::set_clean();  
  } else
      graphic::set_clean();

  return true;
}

void MyArea::refresh()
{
  auto win = get_window();
  
  if(win) {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(), 
					get_allocation().get_height());
	win->invalidate_rect(r,false);
  }
}

MyEvent::MyEvent() :
  m_Box(Gtk::ORIENTATION_HORIZONTAL,10),
  m_Box_Left(Gtk::ORIENTATION_VERTICAL, Gtk::PACK_SHRINK),
  m_Box_Right(Gtk::ORIENTATION_HORIZONTAL, Gtk::PACK_SHRINK),
  m_Button_Exit("exit"),
  m_Button_New("new"),
  m_Button_Open("open"),
  m_Button_Save("save"),
  m_Button_Shortest_path("shortest path"),
  m_Button_Zoom_in("zoom in"),
  m_Button_Zoom_out("zoom out"),
  m_Button_Zoom_reset("zoom reset"),
  m_Button_Edit_link("edit link"),
  m_Label_ENJ("Enj: 0.0000"),
  m_Label_CI("CI: 0"),
  m_Label_MTA("MTA: 0"),
  m_rb_housing("housing"),
  m_rb_transport("transport"),
  m_rb_production("production")
{
  add(m_Box);
  
  m_rb_transport.join_group(m_rb_housing);
  m_rb_production.join_group(m_rb_housing);
  
  m_Box.pack_start(m_Box_Left);
  m_Box.pack_start(m_Box_Right);
  
  m_Area.set_size_request(800,800);
   
  m_Box_Right.pack_start(m_Area);
  
  m_Box_Left.pack_start(m_Button_Exit,Gtk::PACK_SHRINK);
  m_Box_Left.pack_start(m_Button_New,Gtk::PACK_SHRINK);
  m_Box_Left.pack_start(m_Button_Open,Gtk::PACK_SHRINK);
  m_Box_Left.pack_start(m_Button_Save,Gtk::PACK_SHRINK);
  m_Box_Left.pack_start(m_Button_Shortest_path,Gtk::PACK_SHRINK);
  m_Box_Left.pack_start(m_Button_Zoom_in,Gtk::PACK_SHRINK);
  m_Box_Left.pack_start(m_Button_Zoom_out,Gtk::PACK_SHRINK);
  m_Box_Left.pack_start(m_Button_Zoom_reset,Gtk::PACK_SHRINK);
  m_Box_Left.pack_start(m_Button_Edit_link,Gtk::PACK_SHRINK);
  m_Box_Left.pack_start(m_Label_ENJ, Gtk::PACK_SHRINK);
  m_Box_Left.pack_start(m_Label_CI, Gtk::PACK_SHRINK);
  m_Box_Left.pack_start(m_Label_MTA, Gtk::PACK_SHRINK);
  m_Box_Left.pack_start(m_rb_housing, Gtk::PACK_SHRINK);
  m_Box_Left.pack_start(m_rb_transport, Gtk::PACK_SHRINK);
  m_Box_Left.pack_start(m_rb_production, Gtk::PACK_SHRINK);
  
  //connexion des boutons et de leurs fonctions associées 
              
  m_Button_Exit.signal_clicked().connect(sigc::mem_fun(*this,
										  &MyEvent::on_button_clicked_exit));
  
  m_Button_New.signal_clicked().connect(sigc::mem_fun(*this,
                                          &MyEvent::on_button_clicked_new));
  
  m_Button_Open.signal_clicked().connect(sigc::mem_fun(*this,
									      &MyEvent::on_button_clicked_open));
  
  m_Button_Save.signal_clicked().connect(sigc::mem_fun(*this,
									   	  &MyEvent::on_button_clicked_save));
              
  m_Button_Shortest_path.signal_clicked().connect(sigc::mem_fun(*this,
				  				          &MyEvent::on_button_clicked_shortest_path)); 
  
  m_Button_Zoom_in.signal_clicked().connect(sigc::mem_fun(*this,
										  &MyEvent::on_button_clicked_zoom_in));
  
  m_Button_Zoom_out.signal_clicked().connect(sigc::mem_fun(*this,
										  &MyEvent::on_button_clicked_zoom_out));
  
  m_Button_Zoom_reset.signal_clicked().connect(sigc::mem_fun(*this,
										  &MyEvent::on_button_clicked_zoom_reset));
  
  m_Button_Edit_link.signal_clicked().connect(sigc::mem_fun(*this,
										  &MyEvent::on_button_clicked_edit_link));
  
  m_rb_housing.signal_clicked().connect(sigc::mem_fun(*this,
										  &MyEvent::on_button_clicked_housing)); 
   
  m_rb_transport.signal_clicked().connect(sigc::mem_fun(*this,
										  &MyEvent::on_button_clicked_transport)); 
              
  m_rb_production.signal_clicked().connect(sigc::mem_fun(*this,
										  &MyEvent::on_button_clicked_production)); 
  set_criteres();        
  show_all_children();
}

MyEvent::~MyEvent() {}

void MyEvent::on_button_clicked_exit()
{
  exit(0);
}

void MyEvent::on_button_clicked_new()
{ 	
  ville::restart();
  m_Area.clear();
  m_Area.zoom_reset();
  city_read=false;
  set_criteres();
  city_read=true;
}

void MyEvent::on_button_clicked_open()
{
  Gtk::FileChooserDialog dialog("Please choose a file",
								Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Open", Gtk::RESPONSE_OK);
  
  city_read= false;
  int result = dialog.run();
  city_read = true;
  
  switch(result)
  {
    case(Gtk::RESPONSE_OK):
	{
      std::string filename = dialog.get_filename();
      ville::restart();
      ville::lecture(filename);
      set_criteres();
      m_Area.zoom_reset();
      m_Area.draw();
           
      break;
    } 
    case(Gtk::RESPONSE_CANCEL):
    {
      break;
    }
    default:
    {
      break;
    }
  } 
}

void MyEvent::on_button_clicked_save()
{
  Gtk::FileChooserDialog dialog("Please choose a file",
								Gtk::FILE_CHOOSER_ACTION_SAVE);
      
  dialog.set_transient_for(*this);
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Save", Gtk::RESPONSE_OK);
  
  city_read = false;
  int result = dialog.run();
  city_read = true;

  switch(result)
  {
    case(Gtk::RESPONSE_OK):
    {
      std::string filename = dialog.get_filename();
      
      ville::save(filename);
      
      break;
    }
    case(Gtk::RESPONSE_CANCEL):
    {
      break;
    } 
    default:
    {
     break;
    }
  }
}

void MyEvent::on_button_clicked_shortest_path()
{
   
  if (m_Button_Shortest_path.get_active()) 
	  m_Area.shortest_path_active();
  else
	m_Area.shortest_path_non_active();
}

void MyEvent::on_button_clicked_zoom_in()
{
  m_Area.zoom_change(delta_zoom);
}

void MyEvent::on_button_clicked_zoom_out()
{
  m_Area.zoom_change(-delta_zoom);
}

void MyEvent::on_button_clicked_zoom_reset()
{
  m_Area.zoom_reset();
}

void MyEvent::on_button_clicked_edit_link()
{
  if (m_Button_Edit_link.get_active() and (!ville::current_node_active())) {
    cout << "Error edit link " << endl;
    cout << "Please select first a node to edit " << endl;
    m_Button_Edit_link.set_active(false);
  }
  m_Area.edit_link_active = m_Button_Edit_link.get_active();
}

void MyEvent::on_button_clicked_housing(){}

void MyEvent::on_button_clicked_transport(){}

void MyEvent::on_button_clicked_production(){}

void MyEvent::set_criteres()
{
  string equilibreNuitJour ("ENJ: ");
  string coutInfrastructure ("CI: ");
  string meilleurTempsAcces ("MTA: ");
  ville::refresh_criteres();
	
  if (city_read and ville::export_success()) { 
	ostringstream streamObj1;
	streamObj1 << fixed;
	streamObj1 << setprecision(4);
	streamObj1 << ville::export_enj() ;
	string crit1 = streamObj1.str();	
	m_Label_ENJ.set_text(equilibreNuitJour + crit1);
	
	ostringstream streamObj2;
	streamObj2 << ville::export_ci() ;
	string crit2 = streamObj2.str();	
	m_Label_CI.set_text(coutInfrastructure + crit2);
	
	ostringstream streamObj3;
	streamObj3 << ville::export_mta() ;
	string crit3 = streamObj3.str();	
	m_Label_MTA.set_text(meilleurTempsAcces + crit3);
  } else {
    m_Label_ENJ.set_text(equilibreNuitJour + "0.0000");
	m_Label_CI.set_text(coutInfrastructure + "0");
	m_Label_MTA.set_text(meilleurTempsAcces + "0");
  }	
}

bool MyEvent::on_button_press_event(GdkEventButton * event)
{
  if (event->type == GDK_BUTTON_PRESS) {
    double clic_x = event->x ;
	double clic_y = event->y ;
	     
    double origin_x = m_Area.get_allocation().get_x();
	double origin_y = m_Area.get_allocation().get_y();
	  
	double width = m_Area.get_allocation().get_width();
	double height= m_Area.get_allocation().get_height();
	
	// évenement de la souris se situant dans la zone de dessin  
	if (clic_x >= origin_x && clic_x <= origin_x + width &&
		clic_y >= origin_y && clic_y <= origin_y + height) { 
		S2d p({clic_x - origin_x, clic_y -origin_y}); //point cliqué
			
	  if(event->button == 1) { // Bouton gauche de la souris
	    if (m_rb_housing.get_active()) 			
	      m_Area.click_left(p, width, height, 'L');
	    if (m_rb_transport.get_active())
	      m_Area.click_left(p, width, height, 'T'); 
	    if (m_rb_production.get_active())  
	     m_Area.click_left(p, width, height, 'P');	
	  } else if (event->button == 3) { // Bouton droit de la souris
	    m_Area.click_right(p, width, height);
		set_criteres();
	  }
    }
  }
  return true;
}
	
bool MyEvent::on_key_press_event(GdkEventKey * key_event)
{
  if(key_event->type == GDK_KEY_PRESS) {
    switch(gdk_keyval_to_unicode(key_event->keyval))
    {
    case 'i':
      m_Area.zoom_change(delta_zoom);
	  break;
    case 'o':
	  m_Area.zoom_change(-delta_zoom);
	  break;
    case 'r':
	  m_Area.zoom_reset();
	  break;
    }
  }
  return Gtk::Window::on_key_press_event(key_event);
}

bool MyEvent::on_button_release_event(GdkEventButton * event)
{
  if(event->type == GDK_BUTTON_RELEASE) {
	
	double clic_x = event->x ;
	double clic_y = event->y ;
		
	
	double origin_x = m_Area.get_allocation().get_x();
	double origin_y = m_Area.get_allocation().get_y();
			
	
	double width = m_Area.get_allocation().get_width();	
	double height= m_Area.get_allocation().get_height();
	
	// évenement de la souris se situant dans la zone de dessin
	if (clic_x >= origin_x && clic_x <= origin_x + width &&
	    clic_y >= origin_y && clic_y <= origin_y + height) { 
	  
	  S2d p({clic_x - origin_x, clic_y -origin_y}); //point cliqué

	  if (event->button == 1) { // Relache du bouton gauche de la souris
	    m_Area.stay_click(p, width, height);
		set_criteres();
	  }
    }
  }
  return true;
}
