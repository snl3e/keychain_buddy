#include "../include/Game.h"

Game::Game(const char* title, uint32_t width, uint32_t height)
    : title(title), width(width), height(height) {}

void Game::begin(uint32_t nowMs, bool button1Down, bool button2Down) {
    button1_.reset(button1Down, nowMs);
    button2_.reset(button2Down, nowMs);
    phase_ = GamePhase::Start;
    lastUpdateMs_ = nowMs;
    gameOver_ = false;
}

void Game::tick(uint32_t nowMs, bool button1Down, bool button2Down) {
    const ButtonInput input1 = button1_.update(button1Down, nowMs);
    const ButtonInput input2 = button2_.update(button2Down, nowMs);
    const uint32_t deltaMs = nowMs - lastUpdateMs_;
    lastUpdateMs_ = nowMs;

    switch (phase_) {
        case GamePhase::Start:
            if (input1.click) {
                startRunning();
            }
            break;
        case GamePhase::Running:
            updateRunning(deltaMs, input1);
            if (gameOver_) {
                phase_ = GamePhase::End;
            }
            break;
        case GamePhase::End:
            if (input2.click) {
                exitToMenuRequested_ = true;
                phase_ = GamePhase::Start;
            }
            else if (input1.click) {
                startRunning();
            }
            break;
    }
}

void Game::render(U8G2& u8g2) {
    switch (phase_) {
        case GamePhase::Start:
            drawStart(u8g2);
            break;
        case GamePhase::Running:
            drawRunning(u8g2);
            break;
        case GamePhase::End:
            drawEnd(u8g2);
            break;
    }
}

bool Game::shouldExitToMenu() const {
    return exitToMenuRequested_;
}

void Game::clearExitRequest() {
    exitToMenuRequested_ = false;
}

GamePhase Game::phase() const {
    return phase_;
}

const char* Game::gameTitle() const {
    return title;
}

void Game::endGame() {
    gameOver_ = true;
}

void Game::onGameReset() {}

void Game::drawStart(U8G2& u8g2) {
    (void)u8g2;
}

void Game::drawEnd(U8G2& u8g2) {
    (void)u8g2;
}

void Game::startRunning() {
    gameOver_ = false;
    onGameReset();
    phase_ = GamePhase::Running;
}
