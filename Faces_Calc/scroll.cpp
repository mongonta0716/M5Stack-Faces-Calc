#include "scroll.h"


ScrollArea::ScrollArea(void)
{

}

ScrollArea::ScrollArea(boolean scrollmode, boolean visibleRowNo,
                       int textHeight, int textWidth,
                       int startX, int startY, int areaWidth, int areaHeight,
                       int textColor, int bgColor)
{
  this->scrollmode   = scrollmode;
  this->textHeight   = textHeight;
  this->textWidth    = textWidth;
  this->startX       = startX;
  this->startY       = startY;
  this->areaWidth    = areaWidth;
  this->areaHeight   = areaHeight;
  
  this->textColor    = textColor;
  this->bgColor      = bgColor;

  this->maxChars     = areaWidth  / textWidth;
  this->maxLines     = areaHeight / textHeight;

  this->endX       = startX + maxChars * textWidth;
  this->endY       = startY + maxLines * textHeight;

  this->col    = 0;
  this->row    = 0;
  
  this->currentCol = 0;
  this->currentRow = 3;
  
  this->init();
  
  this->visibleRowNo = visibleRowNo;
  this->numOnly      = false;
  
  if (visibleRowNo) {
    this->maxChars = this->maxChars - 3;
  }
}

void ScrollArea::init()
{

  M5.Lcd.fillRect(startX, startY, areaWidth, areaHeight, bgColor);
  M5.Lcd.setTextColor(textColor, bgColor);
  
  for (int i = 0; i < 100; i++) {
    buffer[i] = "";
  }
  col = 0;
  row = 0;

}

void ScrollArea::println(String str)
{

  Serial.println("ScrollArea::println:" + str);
  Serial.printf("row:%d", row);
  buffer[row] = str;
  Serial.printf("Buffer:%s\n" , buffer[row]);
  
  Serial.println(str);
  row += 1;
  drawArea();
}

void ScrollArea::up() {
  row -= 1;
  Serial.printf("Up  :%d\n", row);
  if (row < 0) {
    row = 0;
    return;
  }
  drawArea();
}

void ScrollArea::down() {
  row += 1;
  Serial.printf("Down:%d\n", row);
  if (row > 100) {
    row = 99;
    return;
  }
  drawArea();
}

void ScrollArea::right() {
  col += 1;
  drawArea();
}

void ScrollArea::left() {
  col -= 1;
  if (col < 0) {
    col = 0;
    return;
  }
  drawArea();
}

void ScrollArea::setNumOnly(boolean paramNumOnly) {
  numOnly = paramNumOnly;
}

double ScrollArea::sum() {
  
  double sum = 0;
  if (!numOnly) {
    sum = sqrt(-1);
  }
  double val;
  for (int i = 0; i < 100; i++) {
    val = buffer[i].toDouble();
    sum = sum + val;
  }
  return sum;
  
}

String ScrollArea::getCurrentValue() {
  return buffer[row];
}

void ScrollArea::drawArea() 
{
  M5.Lcd.fillRect(startX, startY, areaWidth, areaHeight, bgColor);
  M5.Lcd.setTextColor(textColor, bgColor);

  int blank    = currentRow - row;

  // normal scroll
  int x = startX;
  int y = startY;
  if (blank >= 0) {
    for (int i = 0; i < (maxLines - blank); i++) {
      M5.Lcd.setCursor(x, y + (i + blank) * textHeight);
      displayRowNo(i);
      M5.Lcd.print(buffer[i].substring(col, col + maxChars));
    }
  } else {
    for (int i = 0; i < maxLines; i++) {
      if ((row + i - currentRow) > 100) {
        break;
      }
      M5.Lcd.setCursor(x, y + i * textHeight);
      displayRowNo( row + i - currentRow);
      M5.Lcd.print(buffer[row + i - currentRow].substring(col, col + maxChars));
    }
  }
  
  M5.Lcd.drawRect( startX, startY + currentRow * textHeight - 1, areaWidth, textHeight, TFT_RED);
  
}

void ScrollArea::displayRowNo(int no)
{
  if (visibleRowNo) {
    M5.Lcd.printf("%2d:", no + 1);
  }
}

void ScrollArea::writeBuffer2spiffs(String filename)
{
  File file;
  file = SPIFFS.open(filename, FILE_WRITE);
  
  for (int i = 0; i < 100; i++) {
    file.println(buffer[i]);
  }
  file.println(row);
  file.close();
}

void ScrollArea::readSpiffs(String filename)
{
  File file;
  file = SPIFFS.open(filename);
  unsigned long filesize = file.size();
  if(filesize == 0) {
    file.close();
    writeBuffer2spiffs(filename);
    return;
  }
  String tmpstr;
  for (int i = 0; i < 100; i++) {
    tmpstr = file.readStringUntil('\n');
    Serial.println("ReadBuf:" + tmpstr + ":");
    if (!numOnly) {
      tmpstr.trim();
    }
    buffer[i] = tmpstr;
  }
  String rowstr = file.readStringUntil('\n');
  Serial.println("Row:" + rowstr + ":");
  row = rowstr.toInt();
  col = 0;
  file.close();
  drawArea();
}
