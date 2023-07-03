// fichier : gui.h
// auteurs : BIENAIMÉ Natacha n°315948 et HANGARD Kélyan n°312936
// version : 2.0
// architecture: Fig11 b1

#ifndef GUI_HEADER_H
#define GUI_HEADER_H

#include <gtkmm.h>
#include "tools.h"

class MyArea : public Gtk::DrawingArea
{
public:
  MyArea();
  virtual ~MyArea();
  void clear();
  void draw();
  void click_left(S2d p, const double& width, const double& height, 
				  const char& T);
  void click_right(S2d p, const double& width, const double& height);
  void zoom_change(const double& change);
  void zoom_reset();
  void stay_click(S2d p, const double& width, const double& height);
  void shortest_path_active();
  void shortest_path_non_active();
  bool edit_link_active;
protected:
  void orthographic_projection(const Cairo::RefPtr<Cairo::Context>& cr, 
							   const int height, const int width);
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
private:
  void change_co(S2d& p, const double& width, const double& height);
  void refresh();
  bool empty;
  double zoom;
  S2d p1;
  bool path;
};

class MyEvent : public Gtk::Window
{
public:
  MyEvent();
  virtual ~MyEvent();

protected:
  void on_button_clicked_exit();
  void on_button_clicked_new();
  void on_button_clicked_open();
  void on_button_clicked_save();
  void on_button_clicked_shortest_path();
  void on_button_clicked_zoom_in();
  void on_button_clicked_zoom_out();
  void on_button_clicked_zoom_reset();
  void on_button_clicked_edit_link();
  void on_button_clicked_housing();
  void on_button_clicked_transport();
  void on_button_clicked_production();
  void set_criteres();
  bool on_button_press_event(GdkEventButton * event);
  bool on_key_press_event(GdkEventKey * key_event);
  bool on_button_release_event(GdkEventButton * event);

  Gtk::Box m_Box, m_Box_Left, m_Box_Right;
  MyArea              m_Area;
  Gtk::Button         m_Button_Exit;
  Gtk::Button         m_Button_New;
  Gtk::Button         m_Button_Open;
  Gtk::Button         m_Button_Save;
  Gtk::ToggleButton   m_Button_Shortest_path;
  Gtk::Button         m_Button_Zoom_in;
  Gtk::Button         m_Button_Zoom_out;
  Gtk::Button         m_Button_Zoom_reset;
  Gtk::ToggleButton   m_Button_Edit_link;
  Gtk::Label          m_Label_ENJ;
  Gtk::Label          m_Label_CI;
  Gtk::Label          m_Label_MTA;
  Gtk::RadioButton    m_rb_housing;
  Gtk::RadioButton    m_rb_transport;
  Gtk::RadioButton    m_rb_production;
};

#endif

