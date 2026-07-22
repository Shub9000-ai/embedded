#ifndef SIMONBLE_BLEGAMESERVICE_H
#define SIMONBLE_BLEGAMESERVICE_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "config.h"
#include "SimonGame.h"

// Represents the game state published over BLE
struct BLEGameState
{
    GameState state;
    int level;
    int score;
    int sequenceLength;
    char sequenceStr[64]; // comma-separated color names
    bool awaitingInput;   // true if we're waiting for player
};

class BLEGameService
{
public:
    BLEGameService();

    // Initialize BLE, create service, start advertising
    bool begin();

    // Poll BLE events and check for written characteristics
    // Returns:
    //   -1 if no input
    //    0–3 for color input
    //    4   for "start game" command
    //    5   for "reset" command
    int poll();

    // Publish current game state to connected clients
    void publishState(const BLEGameState &gameState);

    // Check if a central is connected
    bool isConnected() const;

private:
    BLEService service;
    BLEStringCharacteristic stateChar; // game state JSON (notify)
    BLEByteCharacteristic inputChar;   // player input (write)
    BLEByteCharacteristic controlChar; // game control (write)

    // Internal callbacks
    static void onInputWritten(BLEDevice central, BLECharacteristic characteristic);
    static void onControlWritten(BLEDevice central, BLECharacteristic characteristic);

    // Static pointers for callbacks
    static BLEGameService *instance;
    volatile int pendingInput;
    volatile int pendingCommand;
};

#endif // SIMONBLE_BLEGAMESERVICE_H
