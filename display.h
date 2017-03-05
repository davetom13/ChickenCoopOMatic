#ifndef DISPLAY_H
#define DISPLAY_H

enum LcdButton {
  ButtonUp,
  ButtonDown,
  ButtonLeft,
  ButtonRight,
  ButtonSelect,
  ButtonNone
};

const int statusX = 0;
const int statusY = 1;
const int progressX = 23;
const int progressY = 1;
const int progressCharCount = 4;
const char progressChars[progressCharCount] = {'-', '/', '|', '\\'}; // double \ is for escaping
const int buttonPin = 0;

class Display {
  private:   
    int currentPos = 0;
    unsigned long ticks = 0;
    String emptyLine = String("            ");

    LiquidCrystal* _lcd; 
  public:
    Display(LiquidCrystal* lcd) {
      _lcd = lcd;
    }

   LcdButton buttonDown() {
      int buttonValue = analogRead(buttonPin);

      if (buttonValue < 50)   return ButtonRight;  
      if (buttonValue < 250)  return ButtonUp; 
      if (buttonValue < 450)  return ButtonDown; 
      if (buttonValue < 650)  return ButtonLeft; 
      if (buttonValue < 850)  return ButtonSelect;  

      return ButtonNone;
   }

    void printLine(int line, String string) {
      _lcd->setCursor(0, line);
      _lcd->print(string);
      _lcd->print(emptyLine.substring(0, string.length() - 1));
    }
    
    void printStatus(String position, String status, bool inProgress) {
      printLine(0, position);
      printLine(1, status);
      progressSpinner(inProgress);
    }

    void progressSpinner(bool spin) {
      if (spin) {
        unsigned long newTicks = millis();
        if (newTicks - ticks > 1000) {
          _lcd->setCursor(progressX, progressY);
          _lcd->print(progressChars[currentPos]);
          currentPos++;
          if (currentPos > progressCharCount - 1) {
            currentPos = 0;
          }
          ticks = newTicks;
        }
      }
      else {
        _lcd->setCursor(progressX, progressY);
        _lcd->print(" ");
      }
    }

    void debug(int topSwitch, int botSwitch, int lightLevel) {
      _lcd->setCursor(0,0);
      _lcd->print(String(topSwitch, DEC));
      _lcd->setCursor(4, 0);
      _lcd->print(String(botSwitch, DEC));
    }
};


#endif
