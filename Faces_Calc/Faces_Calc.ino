#include <SD.h>
#include <SPIFFS.h>
#include <M5Unified.h>
#include <M5StackUpdater.h>
#include "scroll.h"
#include "tinyexpr.h"

#define FACES_KEYBOARD_I2C_ADDR 0x08
#define FACES_KEYBOARD_I2C_FREQ 100000

TaskHandle_t taskHandle;

// Color Settings
const uint16_t TITLE     = M5.Lcd.color565(255, 255, 255);
const uint16_t TITLE_BG  = TFT_BLUE;
const uint16_t PROMPT    = TFT_WHITE;
const uint16_t PROMPT_BG = TFT_BLACK;

char promptBuffer[200];
int  promptNum = 0;

ScrollArea *scrollArea;
ScrollArea *answerArea;
String filenameSA = "/scrollarea.tmp";
String filenameAA = "/answerarea.tmp";

unsigned long time1;
unsigned long time2;
boolean cursorFlag = true;

int inputMode = 1;


void initScrollArea()
{
  scrollArea = new ScrollArea(true, true,
                              16, 12, 0, 38, 160, 129,
                              M5.Lcd.color565(0xff, 0xff, 0xff),
                              M5.Lcd.color565(0x66, 0xb7, 0xec));
  answerArea = new ScrollArea(true, false,
                              16, 12, 160, 38, 160, 129,
                              M5.Lcd.color565(0xff, 0xff, 0xff),
                              M5.Lcd.color565(0xaf, 0xd0, 0xef));
  answerArea->setNumOnly(true);

  M5.update();
  if (M5.BtnB.isPressed()) {
    SPIFFS.remove(filenameSA);
    SPIFFS.remove(filenameAA);
  }

  scrollArea->readSpiffs(filenameSA);
  answerArea->readSpiffs(filenameAA);
  
}

void cursorBlink()
{
  if (inputMode == 1) {
    // Don't display cursor when ScrollMode
    M5.Lcd.fillRect(0, 200, 320, 20, PROMPT_BG);
    return;
  }
  time2 = millis();
  unsigned long tmp = time2 - time1;
  if (tmp > 500) {
    if (cursorFlag) {
      M5.Lcd.setCursor(24 + promptNum * 12, 200);
      M5.Lcd.print('_');
      cursorFlag = false;
    } else {
      M5.Lcd.setCursor(24 + promptNum * 12, 200);
      M5.Lcd.print(' ');
      cursorFlag = true;
    }
    time1 = time2;
  }
}

void changeMode() {
  M5.Lcd.setCursor(240, 220);
  M5.Lcd.setTextColor(PROMPT, PROMPT_BG);
  M5.Lcd.fillRect(0, 200, 320, 20, PROMPT_BG);

  if (inputMode == 0) {
    M5.Lcd.print("Scroll");
    inputMode = 1;
  } else {
    M5.Lcd.print("Calc  ");
    displayPrompt();
    inputMode = 0;
  }
}

// Read 1 byte from Faces keyboard by polling. Returns false if no key was read.
// Faces is connected to the internal I2C bus (SDA:21, SCL:22) on M5Stack Basic.
bool readFacesKey(char *c) {
  uint8_t data = 0;
  bool result = false;
  if (M5.In_I2C.start(FACES_KEYBOARD_I2C_ADDR, true, FACES_KEYBOARD_I2C_FREQ)) {
    result = M5.In_I2C.read(&data, 1, true);
  }
  M5.In_I2C.stop();
  if (!result || data == 0x00) return false;
  *c = (char)data;
  Serial.print(*c);         // print the character
  return true;
}

char waitKeyInput() {
  while(true) {
    char c;
    if (readFacesKey(&c)) {
      return c;
    }
    delay(10);
  }
}

void displayPrompt() {
  M5.Lcd.fillRect(0, 200, 320, 20, PROMPT_BG);
  M5.Lcd.setTextColor(PROMPT, PROMPT_BG);
  M5.Lcd.setCursor(0, 200);
  M5.Lcd.print("> ");
  M5.Lcd.print(promptBuffer);
}

void displaySummary() {
  double sum = answerArea->sum();
  M5.Lcd.fillRect(0, 180, 320, 20, PROMPT_BG);
  M5.Lcd.setTextColor(PROMPT, PROMPT_BG);
  M5.Lcd.setCursor(110, 181);
  M5.Lcd.printf("SUM:%12.0f", sum);
  
}

void allClear() {
  M5.Lcd.setCursor(0, 200);
  M5.Lcd.setTextColor(TFT_BLACK, TFT_RED);
  M5.Lcd.print("All clear? yes(=)");
  char c = waitKeyInput();
  if (c == '=') {
    scrollArea->init();
    answerArea->init();
    scrollArea->writeBuffer2spiffs(filenameSA);
    answerArea->writeBuffer2spiffs(filenameAA);
    inputMode = 1;
    changeMode();
    clearPromptBuffer();
  }
  displaySummary();
  displayPrompt();
}

void pushButton(char c) {
  // Faces Calculator3 long press codes
  if (c == 0x08) c = '`';  // Long press A : BackSpace
  if (c == 0x0D) c = '=';  // Long press = : Calc Execute
  if (c < 0x20 || c > 0x7E) return;  // Ignore no key / unknown codes

  if (c == 'M') {
    changeMode();
    return;
  }
  if (c == 'A') {
    allClear();
    return;
  }

  if (inputMode == 1) {
    // Scroll Mode
    switch (c) {
      case '8':
        scrollArea->up();
        answerArea->up();
        break;
      case '2':
        scrollArea->down();
        answerArea->down();
        break;
      case '4':
        scrollArea->left();
        break;
      case '6': 
        scrollArea->right();
        break;
      case '5': // Select line
        String str = scrollArea->getCurrentValue();
        promptNum = str.length();
        str.toCharArray(promptBuffer, promptNum + 1);
        changeMode();
        displayPrompt();
        break;
    }
  } else if (inputMode == 0) {
    switch (c) {
      case '=': // Exec
        calc();
        clearPromptBuffer();
        break;
      case '`': // BackSpace
        if (promptNum == 0) break;
        promptNum--;
        promptBuffer[promptNum] = '\0';
        break;
      case '%': // Line Clear
        clearPromptBuffer();
        break;
      default:
        if (promptNum >= (int)sizeof(promptBuffer) - 1) break;
        promptBuffer[promptNum] = c;
        promptNum++;
        break;
    }
    displayPrompt();
  }
}
void clearPromptBuffer() {
  memset(promptBuffer, '\0', sizeof(promptBuffer));;
  promptNum = 0;
}
void calc() {
  double answer;
  const char *expression = promptBuffer;
  answer = te_interp(expression, 0);
  M5.Lcd.setCursor(0, 220);
  M5.Lcd.fillRect(0, 220, 239, 20, PROMPT_BG);
  if ((answer > 999999999999) || (answer < -99999999999)) {
    M5.Lcd.print("OverFlow!        ");
    return;
  } else if (isnan(answer)) {
    M5.Lcd.print("Wrong expression!");
    return;
  }
  M5.Lcd.printf("%12.0f", answer);
  Serial.printf("answer:%12.0f", answer);
  char buf[13];
  snprintf(buf, 13, "%12.0f", answer);
  scrollArea->println(promptBuffer);
  answerArea->println(buf);
  scrollArea->writeBuffer2spiffs(filenameSA);
  answerArea->writeBuffer2spiffs(filenameAA);
  displaySummary();
}

void setup()
{
  auto cfg = M5.config();
  M5.begin(cfg);
  if (M5.BtnA.isPressed()) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD, MENU_BIN, TFCARD_CS_PIN);
    ESP.restart();
  }
  SPIFFS.begin(true);
  M5.Lcd.setTextDatum(TC_DATUM);
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillRect(0, 0, 320, 32, TITLE_BG);
  M5.Lcd.setTextColor(TITLE, TITLE_BG);
  M5.Lcd.printf("M5Stack Faces\n   Integer Calculator\n");
  if (!M5.In_I2C.scanID(FACES_KEYBOARD_I2C_ADDR, FACES_KEYBOARD_I2C_FREQ)) {
    Serial.println("Faces Calculator not found.");
  } else {
    Serial.printf("Faces model:0x%02X fw:0x%02X\n",
                  M5.In_I2C.readRegister8(FACES_KEYBOARD_I2C_ADDR, 0xD0, FACES_KEYBOARD_I2C_FREQ),
                  M5.In_I2C.readRegister8(FACES_KEYBOARD_I2C_ADDR, 0xFE, FACES_KEYBOARD_I2C_FREQ));
  }
  initScrollArea();
  displaySummary();
  changeMode();
  time1 = millis();
}

void loop()
{

  char c;
  
  if (readFacesKey(&c))
  {
    pushButton(c);
  }
  cursorBlink();
  delay(10);

}
