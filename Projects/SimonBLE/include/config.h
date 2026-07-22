#ifndef SIMONBLE_CONFIG_H
#define SIMONBLE_CONFIG_H

// ============================================================
// Game Constants
// ============================================================

// Maximum sequence length before winning
#define MAX_SEQUENCE_LENGTH 15

// How long each color is shown (ms) — increased for easier play
#define SHOW_DELAY_MS 1200

// Gap between sequence items (ms) — more breathing room
#define SHOW_GAP_MS 500

// Time allowed for entire sequence input (ms) — more time to think
#define INPUT_TOTAL_TIMEOUT_MS 30000

// Delay after correct input before next sequence plays (ms)
#define CORRECT_DELAY_MS 2000

// Delay after game over before reset (ms)
#define GAME_OVER_DELAY_MS 5000

// Delay after winning before reset (ms)
#define WIN_DELAY_MS 8000

// ============================================================
// Gesture Detection (Accelerometer-based tilting)
// ============================================================

// Tilt angle threshold (degrees) — easier to trigger
#define TILT_ANGLE_THRESHOLD 25.0f

// Debounce time for gesture input (ms) — more tolerant
#define GESTURE_DEBOUNCE_MS 500

// ============================================================
// Onboard RGB LED pins (Nano 33 BLE Sense — active LOW)
// ============================================================
#define LED_PIN_R LED_RED   // D22
#define LED_PIN_G LED_GREEN // D23
#define LED_PIN_B LED_BLUE  // D24

// ============================================================
// BLE UUIDs
// ============================================================

#define BLE_SERVICE_UUID "19b20000-0001-537e-4f6c-d104768a1214"
#define BLE_CHAR_GAME_STATE_UUID "19b20001-0001-537e-4f6c-d104768a1214"
#define BLE_CHAR_PLAYER_INPUT_UUID "19b20002-0001-537e-4f6c-d104768a1214"
#define BLE_CHAR_GAME_CONTROL_UUID "19b20003-0001-537e-4f6c-d104768a1214"

#endif // SIMONBLE_CONFIG_H
