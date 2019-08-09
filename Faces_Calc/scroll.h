#pragma once
#include <M5Stack.h>
#include <SPIFFS.h>

class ScrollArea
{
private:
  boolean scrollmode;
  int textHeight;
  int textWidth;
  int startX;
  int startY;
  int areaWidth;
  int areaHeight;

  int maxChars;
  int maxLines;
  
  int endX;
  int endY;
  
  int textColor;
  int bgColor;
  
  String buffer[100];
  int col;
  int row;
  int currentCol;
  int currentRow;
  
  bool visibleRowNo;
  bool numOnly;
  
  void drawArea();
  void displayRowNo(int no);

public:
  ScrollArea();  //constructor
  ScrollArea(boolean scrollmode, boolean visibleRowNo,
             int textHeight, int textWidth,
             int startX, int startY, int areaWidth, int areaHeight,
             int textColor, int bgColor);
  void init();
  void println(String str);
  void up();
  void down();
  void left();
  void right();
  
  void setNumOnly(boolean paramNumOnly);
  double sum();
  String getCurrentValue();
  void writeBuffer2spiffs(String filename);
  void readSpiffs(String filename);

};
