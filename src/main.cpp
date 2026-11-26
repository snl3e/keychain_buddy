#include <U8g2lib.h>
#include <Wire.h>
#include <esp_heap_caps.h>

#include "../include/Animation.h"
#include "../include/BuddyStateMachine.h"
#include "../include/Button.h"
#include "../include/Game.h"
#include "game/BreakoutGame/BreakoutGame.h"
#include "game/DefenderMiniGame/DefenderMiniGame.h"
#include "game/HeliCaveGame/HeliCaveGame.h"
#include "game/JumpGame/JumpGame.h"
#include "game/MicroRacerGame/MicroRacerGame.h"
#include "animations/DefaultAnimation/DefaultAnimation.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 6, 5);

uint32_t startTime;

constexpr uint8_t BUTTON1_PIN = 2;
constexpr uint8_t BUTTON2_PIN = 3;
constexpr uint8_t TOUCH_PIN = 10;

constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr uint8_t GAME_WIDTH = SCREEN_WIDTH;
constexpr uint8_t GAME_HEIGHT = SCREEN_HEIGHT;
constexpr uint8_t GAME_LEFT = 0;

BuddyStateMachine buddy;

AnimationPlayer animationPlayer;

BreakoutGame breakoutGame(GAME_WIDTH, GAME_HEIGHT, GAME_LEFT);
MicroRacerGame microRacerGame(GAME_WIDTH, GAME_HEIGHT, GAME_LEFT);
DefenderMiniGame defenderMiniGame(GAME_WIDTH, GAME_HEIGHT, GAME_LEFT);
JumpGame jumpGame(GAME_WIDTH, GAME_HEIGHT, GAME_LEFT);
HeliCaveGame heliCaveGame(GAME_WIDTH, GAME_HEIGHT, GAME_LEFT);

Game* games[] = {&breakoutGame, &microRacerGame, &defenderMiniGame, &jumpGame, &heliCaveGame};
constexpr uint8_t GAME_COUNT = sizeof(games) / sizeof(games[0]);
constexpr uint32_t IDLE_TIMEOUT_MS = 10000;

SingleButton button1;
SingleButton button2;
uint8_t menuIndex = 0;

Game* activeGame = nullptr;

void resetTimeCounter() {
  startTime = millis();
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

bool shouldEnterIdle(uint32_t nowMs) {
  return nowMs - startTime > IDLE_TIMEOUT_MS &&
         buddy.GetCurrentState() == BuddyStateMachine::ST_MENU;
}

void enterIdle() {
  u8g2.clearDisplay();
  u8g2.clearBuffer();
  u8g2.sendBuffer();
  buddy.enterIdle();
}

void updateIdleState(bool button1Down, bool button2Down) {
  if (button1Down && button2Down) {
    resetTimeCounter();
    buddy.enterMenu();
    delay(500);
  }
}

void updateMenuState(uint32_t nowMs, bool button1Down, bool button2Down) {
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

void updateGameState(uint32_t nowMs, bool button1Down, bool button2Down) {
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

void setup(void) {
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  pinMode(TOUCH_PIN, INPUT);

  Serial.begin(115200);

  u8g2.begin();
  u8g2.setContrast(255);
  u8g2.setBusClock(400000);

  randomSeed(micros());

  button1.reset(readActiveLowButton(BUTTON1_PIN), millis());
  button2.reset(readActiveLowButton(BUTTON2_PIN), millis());

  startTime = millis();
}

void printRamUsage() {
  Serial.printf("Free heap: %lu bytes\n", (unsigned long)ESP.getFreeHeap());
  Serial.printf("Min free heap (lowest ever): %lu bytes\n", (unsigned long)ESP.getMinFreeHeap());
  Serial.printf("Largest free block: %lu bytes\n", (unsigned long)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

  if (psramFound()) {
    Serial.printf("Free PSRAM: %lu bytes\n", (unsigned long)ESP.getFreePsram());
  }
}

void loop(void) {
  const uint32_t nowMs = millis();
  const bool button1Down = readActiveLowButton(BUTTON1_PIN);
  const bool button2Down = readActiveLowButton(BUTTON2_PIN);
  const auto currentState = buddy.GetCurrentState();

  static unsigned long lastRamPrint = 0;
  if (nowMs - lastRamPrint >= 1000) {
    lastRamPrint = nowMs;
    printRamUsage();
  }


  if (shouldEnterIdle(nowMs) && currentState != BuddyStateMachine::ST_IDLE) {
    animationPlayer.play(DEFAULT_ANIMATION);
    enterIdle();
    return;
  }

  u8g2.clearBuffer();


  switch(currentState){
    case (BuddyStateMachine::ST_IDLE):
      updateIdleState(button1Down, button2Down);
      animationPlayer.update(u8g2);
      break;
    case (BuddyStateMachine::ST_MENU):
      updateMenuState(nowMs, button1Down, button2Down);
      break;
    case (BuddyStateMachine::ST_GAME):
      updateGameState(nowMs, button1Down, button2Down);
      break;
  }
 
  if (currentState != BuddyStateMachine::ST_IDLE) {
    u8g2.sendBuffer();
  }
}
