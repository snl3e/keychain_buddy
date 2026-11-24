#include <U8g2lib.h>
#include <Wire.h>

#include "../include/BuddyStateMachine.h"
#include "../include/Button.h"
#include "../include/Game.h"
#include "game/BreakoutGame/BreakoutGame.h"
#include "game/DefenderMiniGame/DefenderMiniGame.h"
#include "game/HeliCaveGame/HeliCaveGame.h"
#include "game/JumpGame/JumpGame.h"
#include "game/MicroRacerGame/MicroRacerGame.h"


U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 6, 5);

unsigned int startTime;
unsigned int elapsedTime;


constexpr uint8_t BUTTON1_PIN = 2;
constexpr uint8_t BUTTON2_PIN = 3;
constexpr uint8_t TOUCH_PIN = 10;

constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr uint8_t GAME_WIDTH = SCREEN_WIDTH;
constexpr uint8_t GAME_HEIGHT = SCREEN_HEIGHT;
constexpr uint8_t GAME_LEFT = 0;

BuddyStateMachine buddy;

BreakoutGame breakoutGame(GAME_WIDTH, GAME_HEIGHT, GAME_LEFT);
MicroRacerGame microRacerGame(GAME_WIDTH, GAME_HEIGHT, GAME_LEFT);
DefenderMiniGame defenderMiniGame(GAME_WIDTH, GAME_HEIGHT, GAME_LEFT);
JumpGame jumpGame(GAME_WIDTH, GAME_HEIGHT, GAME_LEFT);
HeliCaveGame heliCaveGame(GAME_WIDTH, GAME_HEIGHT, GAME_LEFT);

Game* games[] = {&breakoutGame, &microRacerGame, &defenderMiniGame, &jumpGame, &heliCaveGame};
constexpr uint8_t GAME_COUNT = sizeof(games) / sizeof(games[0]);

SingleButton button1;
SingleButton button2;
uint8_t menuIndex = 0;

Game* activeGame = nullptr;

void resetTimeCounter(){
  startTime = millis();
  elapsedTime = 0;
}

void drawMenu() {
  u8g2.drawFrame(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.drawStr(3, 9, "Select game");
  u8g2.drawStr(3, 19, games[menuIndex]->gameTitle());
  u8g2.drawStr(3, 61, "next");
  u8g2.drawStr(128 - u8g2.getStrWidth("start") - 3, 61, "start");
}

void drawGameOverlay(Game& game) {
  u8g2.drawFrame(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  u8g2.setFont(u8g2_font_5x8_tr);
  if (game.phase() == GamePhase::Start) {
    u8g2.drawStr(3, 10, game.gameTitle());
    u8g2.drawStr(128 - u8g2.getStrWidth("start") - 3, 61, "start");
  } else if (game.phase() == GamePhase::End) {
    u8g2.drawStr(3, 10, "Game Over");
    u8g2.drawStr(3, 61, "menu");
    u8g2.drawStr(128 - u8g2.getStrWidth("restart") - 3, 61, "restart");
  }
}

void setup(void) {
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  pinMode(TOUCH_PIN, INPUT);

  u8g2.begin();
  u8g2.setContrast(255);
  u8g2.setBusClock(400000);
  randomSeed(micros());
  button1.reset(readActiveLowButton(BUTTON1_PIN), millis());
  button2.reset(readActiveLowButton(BUTTON2_PIN), millis());

  startTime = millis();
  elapsedTime = 0;
}

void loop(void) {

  const uint32_t nowMs = millis();
  elapsedTime = nowMs - startTime;
  const bool button1Down = readActiveLowButton(BUTTON1_PIN);
  const bool button2Down = readActiveLowButton(BUTTON2_PIN);

  if (elapsedTime > 10000 && buddy.GetCurrentState() == BuddyStateMachine::ST_MENU) {
    u8g2.clearDisplay();
    u8g2.clearBuffer();
    u8g2.sendBuffer();

    buddy.enterIdle();
    return;
  }
  
  u8g2.clearBuffer();
  /*-------------IDLE STATE---------------*/
  if (buddy.GetCurrentState() == BuddyStateMachine::ST_IDLE){
    if (button1Down && button2Down) {
      resetTimeCounter();
      buddy.enterMenu();
      delay(500);
    }
  }


  /*-------------MENU STATE---------------*/
  else if (buddy.GetCurrentState() == BuddyStateMachine::ST_MENU) 
  {
    const ButtonInput input1 = button1.update(button1Down, nowMs);
    const ButtonInput input2 = button2.update(button2Down, nowMs);

    if (input2.click) {
      menuIndex = (menuIndex + 1) % GAME_COUNT;
    }
    if (input1.click) {
      activeGame = games[menuIndex];
      activeGame->begin(nowMs, button1Down, button2Down);
      buddy.enterGame();
    }
    drawMenu();
  } 


  /*-------------GAME STATE---------------*/
  else if (buddy.GetCurrentState() == BuddyStateMachine::ST_GAME)
  {
    activeGame->tick(nowMs, button1Down, button2Down);
    activeGame->render(u8g2);
    if (activeGame->phase() != GamePhase::Running) {
      drawGameOverlay(*activeGame);
    }
    if (activeGame->shouldExitToMenu()) {
      activeGame->clearExitRequest();
      button1.reset(button1Down, nowMs);
      button2.reset(button2Down, nowMs);
      buddy.enterMenu();
      resetTimeCounter();
    }
  }

  u8g2.sendBuffer();
}
