#ifndef SIMONBLE_GAME_H
#define SIMONBLE_GAME_H

#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include "config.h"

// Color indices (also used for BLE input: 0–3)
enum class SimonColor : uint8_t
{
    RED = 0,
    GREEN = 1,
    BLUE = 2,
    YELLOW = 3,
    NONE = 4 // no input / timeout
};

// Game states
enum class GameState : uint8_t
{
    IDLE = 0,
    SHOWING = 1, // device is displaying sequence
    INPUT = 2,   // waiting for player gesture input
    CORRECT = 3, // player matched correctly (brief)
    WRONG = 4,   // player made a mistake (brief)
    WIN = 5      // player completed all levels (brief)
};

class SimonGame
{
public:
    SimonGame();

    // Initialize pins, IMU, start in IDLE
    void begin();

    // Main update — call every loop()
    void update();

    // Force-feed a color input from BLE (0–3)
    void feedInput(SimonColor color);

    // Start a new game (from BLE control or tilt-to-start)
    void startGame();

    // Reset to idle
    void resetGame();

    // Enable/disable gesture (IMU) input — disable when BLE controls
    void setGesturesEnabled(bool enabled) { enableGestures = enabled; }
    bool getGesturesEnabled() const { return enableGestures; }

    // Getters
    GameState getState() const { return state; }
    int getLevel() const { return level; }
    int getScore() const { return score; }
    int getSequenceLength() const { return sequenceLength; }
    SimonColor getExpectedColor(int index) const;

    // Get the full sequence as a string for BLE JSON
    void getSequenceString(char *buf, size_t len) const;

private:
    // --- Game State ---
    GameState state;
    SimonColor sequence[MAX_SEQUENCE_LENGTH];
    int sequenceLength; // how many items currently in sequence
    int level;
    int score;
    int showIndex;  // which item we're showing
    int inputIndex; // which item player is currently inputting

    // --- Timing ---
    unsigned long stateStartMillis;
    unsigned long lastShowMillis;

    // --- Gesture ---
    SimonColor lastGesture;
    unsigned long lastGestureMillis;
    bool enableGestures; // true = use IMU tilt, false = BLE controls

    // --- Internal methods ---
    void generateNextColor();
    void showNextItem();
    void evaluateInput(SimonColor color);
    void setState(GameState newState);

    // --- LED control (active LOW on Nano 33 BLE) ---
    void ledAllOff();
    void ledSetColor(SimonColor color);
    void ledSetRGB(uint8_t r, uint8_t g, uint8_t b); // 0=off, 255=full on (will invert)

    // --- Gesture detection from IMU ---
    SimonColor detectGesture();
};

#endif // SIMONBLE_GAME_H
