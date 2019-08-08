#include <M5Stack.h>
#include <M5StackUpdater.h>
#include "scroll.h"
#include "tinyexpr.h"

#define FACES_KEYBOARD_I2C_ADDR 0x08

TaskHandle_t taskHandle;

// Color Settings
const unsigned int TITLE     = M5.Lcd.color565(255, 255, 255);
const unsigned int TITLE_BG  = TFT_BLUE;
const unsigned int PROMPT    = TFT_WHITE;
const unsigned int PROMPT_BG = TFT_BLACK;

char promptBuffer[200];
int  promptNum = 0;

ScrollArea *scrollArea;
ScrollArea *answerArea;

unsigned long time1;
unsigned long time2;
boolean cursorFlag = true;

int inputMode = 1;

void cursorBlink(void *args) {
  boolean cursormode = true;
  for(;;)
  {
    if (cursormode) {
      M5.Lcd.setCursor(24 + promptNum * 12, 200);
      M5.Lcd.print('_');
      cursormode = false;
    } else {
      M5.Lcd.setCursor(24 + promptNum * 12, 200);
      M5.Lcd.print(' ');
      cursormode = true;
    }
    delay(500);
  }

}

void startTask()
{
  xTaskCreatePinnedToCore(
                          cursorBlink,
                          "cursorBlink",
                          2048,
                          NULL,
                          2,
                          &taskHandle,
                          1);
}

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
}

void changeMode() {
  M5.Lcd.setCursor(240, 220);
  M5.Lcd.setTextColor(PROMPT, PROMPT_BG);
  M5.Lcd.fillRect(0, 200, 320, 20, PROMPT_BG);

  if (inputMode == 0) {
    M5.Lcd.print("Scroll");
    vTaskDelete(taskHandle);
    inputMode = 1;
  } else {
    M5.Lcd.print("Calc  ");
    displayPrompt();
    startTask();
    inputMode = 0;
  }
}

char waitKeyInput() {
  while(true) {
    char c;
  
    if (digitalRead(5) == LOW)
    {
      Wire.requestFrom(FACES_KEYBOARD_I2C_ADDR, 1);
      while (Wire.available())
      {
        c = Wire.read(); // receive a byte as character
        Serial.print(c);         // print the character
        return c;
      }
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
    inputMode = 1;
    changeMode();
    clearPromptBuffer();
  }
  displaySummary();
  displayPrompt();
}

void pushButton(char c) {
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
      case '.': // BackSpace
        if (promptNum == 0) break;
        promptNum--;
        promptBuffer[promptNum] = '\0';
        break;
      case '%': // Line Clear
        clearPromptBuffer();
        break;
      default:
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
  displaySummary();
}

void testPrint() {
  scrollArea->println("Initial-------------");
  scrollArea->println("Line1");
  scrollArea->println("012345678901234567890123456789");
  scrollArea->println("Line3");
  scrollArea->println("012345678901234567890123456789");
  scrollArea->println("Line5");
  scrollArea->println("This is TextScroll Demo.Press key!!!!!!!!!!!!!!!!!!!!!");
  scrollArea->println("Line7");
  scrollArea->println("012345678901234567890123456789");
  scrollArea->println("Line9");
  scrollArea->println("012345678901234567890123456789");

  answerArea->println("ans1");
  answerArea->println("ans2");
  answerArea->println("ans3");
  answerArea->println("ans4");
  answerArea->println("ans5");
  answerArea->println("ans6");
  answerArea->println("ans7");
  answerArea->println("ans8");
  answerArea->println("ans9");
  answerArea->println("ans10");

}

void setup()
{
  M5.begin();
  Wire.begin();
  if(digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }

  pinMode(5, INPUT);
  digitalWrite(5,HIGH);
  M5.Lcd.setTextDatum(TC_DATUM);
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillRect(0, 0, 320, 32, TITLE_BG);
  M5.Lcd.setTextColor(TITLE, TITLE_BG);
  M5.Lcd.printf("M5Stack Faces\n   Integer Calculator\n");
  initScrollArea();
  changeMode();
  time1 = millis();
  //testPrint();
}
void loop()
{

  char c;
  
  if (digitalRead(5) == LOW)
  {
    Wire.requestFrom(FACES_KEYBOARD_I2C_ADDR, 1);
    while (Wire.available())
    {
      c = Wire.read(); // receive a byte as character
      Serial.print(c);         // print the character
      pushButton(c);
    }
  }
  delay(10);

}
