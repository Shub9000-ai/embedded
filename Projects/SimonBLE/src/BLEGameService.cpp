#include "BLEGameService.h"
#include "config.h"
#include <Arduino.h>
#include <ArduinoBLE.h>

// Static instance pointer for callbacks
BLEGameService *BLEGameService::instance = nullptr;

// Forward declaration
static const char *stateToString(GameState s);

// ============================================================
// Constructor
// ============================================================
BLEGameService::BLEGameService()
    : service(BLE_SERVICE_UUID), stateChar(BLE_CHAR_GAME_STATE_UUID, BLERead | BLENotify, 128), inputChar(BLE_CHAR_PLAYER_INPUT_UUID, BLERead | BLEWrite), controlChar(BLE_CHAR_GAME_CONTROL_UUID, BLERead | BLEWrite), pendingInput(-1), pendingCommand(-1)
{
    instance = this;
}

// ============================================================
// begin() — Initialize BLE, setup service, advertise
// ============================================================
bool BLEGameService::begin()
{
    if (!BLE.begin())
    {
        Serial.println("Failed to initialize BLE!");
        return false;
    }

    BLE.setLocalName("SimonBLE");
    BLE.setAdvertisedService(service);

    service.addCharacteristic(stateChar);
    service.addCharacteristic(inputChar);
    service.addCharacteristic(controlChar);

    stateChar.setValue("{\"state\":\"IDLE\",\"level\":0,\"score\":0,\"seq\":\"\",\"waiting\":false}");

    inputChar.setEventHandler(BLEWritten, onInputWritten);
    controlChar.setEventHandler(BLEWritten, onControlWritten);

    BLE.addService(service);
    BLE.advertise();

    Serial.println("BLE service started — advertising as 'SimonBLE'");
    return true;
}

// ============================================================
// poll() — Check BLE events and return pending input
// Returns: -1 = nothing, 0-3 = color, 4 = start, 5 = reset
// ============================================================
int BLEGameService::poll()
{
    BLE.poll();

    if (pendingInput >= 0)
    {
        int val = pendingInput;
        pendingInput = -1;
        return val;
    }

    if (pendingCommand >= 0)
    {
        int val = pendingCommand;
        pendingCommand = -1;
        return val;
    }

    return -1;
}

// ============================================================
// publishState() — Send JSON to connected clients
// ============================================================
void BLEGameService::publishState(const BLEGameState &gameState)
{
    char json[128];
    snprintf(json, sizeof(json),
             "{\"state\":\"%s\",\"level\":%d,\"score\":%d,\"seqlen\":%d,\"seq\":\"%s\",\"waiting\":%s}",
             stateToString(gameState.state),
             gameState.level,
             gameState.score,
             gameState.sequenceLength,
             gameState.sequenceStr,
             gameState.awaitingInput ? "true" : "false");

    stateChar.setValue(json);
}

// ============================================================
// isConnected() — Check for active BLE central
// ============================================================
bool BLEGameService::isConnected() const
{
    return BLE.connected();
}

// ============================================================
// Static callback: onInputWritten
// ============================================================
void BLEGameService::onInputWritten(BLEDevice central, BLECharacteristic characteristic)
{
    (void)central;
    if (instance && characteristic.written())
    {
        const uint8_t *val = (const uint8_t *)characteristic.value();
        if (val && *val <= 3)
        {
            instance->pendingInput = (int)(*val);
        }
    }
}

// ============================================================
// Static callback: onControlWritten
// ============================================================
void BLEGameService::onControlWritten(BLEDevice central, BLECharacteristic characteristic)
{
    (void)central;
    if (instance && characteristic.written())
    {
        const uint8_t *val = (const uint8_t *)characteristic.value();
        if (val)
        {
            if (*val == 0)
            {
                instance->pendingCommand = 4;
            }
            else if (*val == 1)
            {
                instance->pendingCommand = 5;
            }
        }
    }
}

// ============================================================
// Helper: state enum to string
// ============================================================
static const char *stateToString(GameState s)
{
    switch (s)
    {
    case GameState::IDLE:
        return "IDLE";
    case GameState::SHOWING:
        return "SHOWING";
    case GameState::INPUT:
        return "INPUT";
    case GameState::CORRECT:
        return "CORRECT";
    case GameState::WRONG:
        return "WRONG";
    case GameState::WIN:
        return "WIN";
    default:
        return "UNKNOWN";
    }
}
