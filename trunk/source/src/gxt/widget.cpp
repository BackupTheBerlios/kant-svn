#include "widget.h"

namespace gxt {

  Widget::Widget(): menu(0) 
  { } 

  Widget::~Widget() 
  { } 

  void Widget::connect_menu( gxt::Menu &menu_ ) 
  { menu = &menu_; }

  void Widget::disconnect_menu() 
  { menu = 0; }

  void Widget::disconnect_menu( gxt::Menu &menu_ ) { 
    if( menu == &menu_ )
      menu = 0; 
  }

  void Widget::menu_popup() 
  { menu_popup(0,0); }

  void Widget::menu_popup( guint button, guint32 activate_time ) {
    if( menu )
      menu->popup(button, activate_time);
  }

}
