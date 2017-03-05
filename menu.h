#ifndef MENU_H
#define MENU_H

#include "display.h"
#include "dootmanager.h"
#include "utils.h"

class MenuState {
  protected:
    Menu* _menu;
  MenuState(Menu menu) {
    _menu = menu;
  }
  virtual void enterState() = 0;
  virtual void work() = 0;
  virtual void exitState() = 0;
}

class ScreenOffState {
  void enterState() {
    _menu->_display->off();
  }
  void work() {}
  void exitState() {
    _menu->_display->on();
  }
}

class 

class Menu {
  friend class MenuState;
  private:
    Display* _display;
    DoorManager* _doorManager;
    
  public:
    void work() {
      
      
    }
}


#endif
