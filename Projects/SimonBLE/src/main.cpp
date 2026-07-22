#include <Arduino.h>
#include "SimonGame.h"
#include "BLEGameService.h"
#include "config.h"

// Global game and BLE objects
SimonGame game;
BLEGameService bleService;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("============================");
  Serial.println("  SimonBLE — Gesture Edition ");
  Serial.println("  + Web Bluetooth Compatible ");
  Serial.println("============================");

  // Initialize game (starts IDLE, waits for tilt)
  game.begin();

  // Initialize BLE service
  bleService.begin();

  Serial.println("\n--- How to Play ---");
  Serial.println("  Standalone: Tilt the board!");
  Serial.println("  Tilt Forward  → Red");
  Serial.println("  Tilt Backward → Green");
  Serial.println("  Tilt Left     → Blue");
  Serial.println("  Tilt Right    → Yellow");
  Serial.println("");
  Serial.println("  Web: Connect via Bluetooth at");
  Serial.println("  simonble.rahul.live (or open index.html)");
  Serial.println("");
  Serial.println("SimonBLE ready!");
}

void loop()
{
  static bool lastConnected = false;

  // 1. Update game state machine
  game.update();

  // 2. Poll BLE for incoming commands/input
  int bleInput = bleService.poll();

  if (bleInput >= 0)
  {
    switch (bleInput)
    {
    case 0: // Red
    case 1: // Green
    case 2: // Blue
    case 3: // Yellow
      game.feedInput((SimonColor)bleInput);
      break;
    case 4: // Start command
      Serial.println("BLE: Start command received");
      game.startGame();
      break;
    case 5: // Reset command
      Serial.println("BLE: Reset command received");
      game.resetGame();
      break;
    }
  }

  // 3. Toggle gesture input based on BLE connection
  bool connected = bleService.isConnected();
  if (connected != lastConnected)
  {
    lastConnected = connected;
    if (connected)
    {
      Serial.println("📱 BLE Connected — disabling tilt gestures");
      game.setGesturesEnabled(false);
    }
    else
    {
      Serial.println("📱 BLE Disconnected — enabling tilt gestures");
      game.setGesturesEnabled(true);
    }
  }

  // 4. Publish game state over BLE if connected
  if (connected)
  {
    BLEGameState bleState;
    bleState.state = game.getState();
    bleState.level = game.getLevel();
    bleState.score = game.getScore();
    bleState.sequenceLength = game.getSequenceLength();

    char seqBuf[64];
    game.getSequenceString(seqBuf, sizeof(seqBuf));
    strncpy(bleState.sequenceStr, seqBuf, sizeof(bleState.sequenceStr) - 1);
    bleState.sequenceStr[sizeof(bleState.sequenceStr) - 1] = '\0';

    bleState.awaitingInput = (game.getState() == GameState::INPUT);

    bleService.publishState(bleState);
  }
}
