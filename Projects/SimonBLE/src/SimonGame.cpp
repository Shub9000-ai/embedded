#include "SimonGame.h"
#include "config.h"
#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include <math.h>

// ============================================================
// Constructor
// ============================================================
SimonGame::SimonGame()
    : state(GameState::IDLE), sequenceLength(0), level(0), score(0), showIndex(0), inputIndex(0), stateStartMillis(0), lastShowMillis(0), lastGesture(SimonColor::NONE), lastGestureMillis(0), enableGestures(true)
{
    memset(sequence, 0, sizeof(sequence));
}

// ============================================================
// begin() — Initialize pins, IMU
// ============================================================
void SimonGame::begin()
{
    // LED pins
    pinMode(LED_PIN_R, OUTPUT);
    pinMode(LED_PIN_G, OUTPUT);
    pinMode(LED_PIN_B, OUTPUT);
    ledAllOff();

    // Initialize IMU for gesture detection
    if (!IMU.begin())
    {
        Serial.println("ERROR: Failed to initialize IMU!");
        while (1)
            ; // halt
    }
    Serial.println("IMU initialized for gesture control");

    // Seed random
    randomSeed(analogRead(A0));

    // Startup blink
    ledSetColor(SimonColor::BLUE);
    delay(400);
    ledAllOff();
    delay(200);
    ledSetColor(SimonColor::RED);
    delay(200);
    ledAllOff();
    delay(200);
    ledSetColor(SimonColor::GREEN);
    delay(200);
    ledAllOff();
    delay(200);
    ledSetColor(SimonColor::YELLOW);
    delay(200);
    ledAllOff();

    Serial.println("SimonBLE ready — tilt to start!");
}

// ============================================================
// update() — Main game loop (call every loop())
// ============================================================
void SimonGame::update()
{
    unsigned long now = millis();

    switch (state)
    {

    // ========================================
    case GameState::IDLE:
        // Idle breathing: pulse blue slowly
        {
            float phase = (sin(now * 0.003f) + 1.0f) * 0.5f; // 0–1
            uint8_t b = (uint8_t)(phase * 80.0f);
            ledSetRGB(0, 0, b);
        }

        // Detect tilt to start game (only if gestures enabled)
        if (enableGestures && detectGesture() != SimonColor::NONE)
        {
            startGame();
        }
        break;

    // ========================================
    case GameState::SHOWING:
    {
        // Show items one by one with timing
        if (showIndex < sequenceLength)
        {
            if (now - lastShowMillis >= SHOW_DELAY_MS + SHOW_GAP_MS)
            {
                // Show next item
                SimonColor c = sequence[showIndex];
                ledSetColor(c);
                lastShowMillis = now;
                showIndex++;
            }
            else if (now - lastShowMillis >= SHOW_DELAY_MS)
            {
                // Between items: turn off
                ledAllOff();
            }
        }
        else
        {
            // Done showing — switch to input mode
            ledAllOff();
            inputIndex = 0;
            setState(GameState::INPUT);
            Serial.println("Your turn — tilt to match the sequence!");
        }
    }
    break;

    // ========================================
    case GameState::INPUT:
    {
        // Check for timeout
        if (now - stateStartMillis > INPUT_TOTAL_TIMEOUT_MS)
        {
            Serial.println("Timeout!");
            setState(GameState::WRONG);
            break;
        }

        // Detect gesture (only if gestures enabled)
        if (enableGestures)
        {
            SimonColor gesture = detectGesture();
            if (gesture != SimonColor::NONE && gesture != lastGesture)
            {
                lastGesture = gesture;
                lastGestureMillis = now;

                // Visual feedback for the detected color
                ledSetColor(gesture);
                delay(200);
                ledAllOff();

                // Evaluate
                evaluateInput(gesture);
            }
        }

        // Visual cue: blink the current expected color faintly
        if (inputIndex < sequenceLength)
        {
            SimonColor expected = sequence[inputIndex];
            float phase = (sin(now * 0.005f) + 1.0f) * 0.5f;
            uint8_t dim = (uint8_t)(phase * 30.0f);
            switch (expected)
            {
            case SimonColor::RED:
                ledSetRGB(dim, 0, 0);
                break;
            case SimonColor::GREEN:
                ledSetRGB(0, dim, 0);
                break;
            case SimonColor::BLUE:
                ledSetRGB(0, 0, dim);
                break;
            case SimonColor::YELLOW:
                ledSetRGB(dim, dim, 0);
                break;
            default:
                break;
            }
        }
    }
    break;

    // ========================================
    case GameState::CORRECT:
        // Brief success feedback, then show next sequence
        if (now - stateStartMillis > CORRECT_DELAY_MS)
        {
            setState(GameState::SHOWING);
        }
        break;

    // ========================================
    case GameState::WRONG:
        // Red flashing + fail tone, then back to idle
        {
            float phase = (sin(now * 0.008f) + 1.0f) * 0.5f;
            uint8_t r = (uint8_t)(phase * 255.0f);
            ledSetRGB(r, 0, 0);
        }
        if (now - stateStartMillis > GAME_OVER_DELAY_MS)
        {
            resetGame();
        }
        break;

    // ========================================
    case GameState::WIN:
        // Rainbow celebration
        {
            int cycle = (now / 120) % 4;
            switch (cycle)
            {
            case 0:
                ledSetColor(SimonColor::RED);
                break;
            case 1:
                ledSetColor(SimonColor::GREEN);
                break;
            case 2:
                ledSetColor(SimonColor::BLUE);
                break;
            case 3:
                ledSetColor(SimonColor::YELLOW);
                break;
            }
        }
        if (now - stateStartMillis > WIN_DELAY_MS)
        {
            resetGame();
        }
        break;
    }
}

// ============================================================
// feedInput() — External input from BLE
// ============================================================
void SimonGame::feedInput(SimonColor color)
{
    if (state == GameState::INPUT)
    {
        evaluateInput(color);
    }
}

// ============================================================
// startGame() — Begin a new game
// ============================================================
void SimonGame::startGame()
{
    resetGame();
    sequenceLength = 1;
    level = 1;
    score = 0;

    // Generate first color
    generateNextColor();

    // Start showing sequence
    showIndex = 0;
    lastShowMillis = 0;
    setState(GameState::SHOWING);

    Serial.print("Game started! Level ");
    Serial.println(level);
    Serial.println("Watch the colors...");
}

// ============================================================
// resetGame() — Return to idle
// ============================================================
void SimonGame::resetGame()
{
    state = GameState::IDLE;
    stateStartMillis = millis();
    sequenceLength = 0;
    level = 0;
    score = 0;
    showIndex = 0;
    inputIndex = 0;
    memset(sequence, 0, sizeof(sequence));
    ledAllOff();
    Serial.println("Game reset — tilt to start a new game!");
}

// ============================================================
// getExpectedColor() — For BLE display
// ============================================================
SimonColor SimonGame::getExpectedColor(int index) const
{
    if (index >= 0 && index < sequenceLength)
    {
        return sequence[index];
    }
    return SimonColor::NONE;
}

// ============================================================
// getSequenceString() — Build comma-separated color string
// ============================================================
void SimonGame::getSequenceString(char *buf, size_t len) const
{
    int pos = 0;
    for (int i = 0; i < sequenceLength && pos < (int)len - 1; i++)
    {
        const char *name;
        switch (sequence[i])
        {
        case SimonColor::RED:
            name = "R";
            break;
        case SimonColor::GREEN:
            name = "G";
            break;
        case SimonColor::BLUE:
            name = "B";
            break;
        case SimonColor::YELLOW:
            name = "Y";
            break;
        default:
            name = "?";
            break;
        }
        pos += snprintf(buf + pos, len - pos, "%s%s", (i > 0 ? "," : ""), name);
    }
}

// ============================================================
// generateNextColor() — Add random color to sequence
// ============================================================
void SimonGame::generateNextColor()
{
    if (sequenceLength < MAX_SEQUENCE_LENGTH)
    {
        sequence[sequenceLength] = (SimonColor)(random(0, 4));
        sequenceLength++;
    }
}

// ============================================================
// setState() — Transition to a new state
// ============================================================
void SimonGame::setState(GameState newState)
{
    state = newState;
    stateStartMillis = millis();
}

// ============================================================
// evaluateInput() — Check player's color against expected
// ============================================================
void SimonGame::evaluateInput(SimonColor color)
{
    if (inputIndex >= sequenceLength)
        return; // shouldn't happen

    SimonColor expected = sequence[inputIndex];

    if (color == expected)
    {
        inputIndex++;

        if (inputIndex >= sequenceLength)
        {
            // Entire sequence matched!
            score += level;
            Serial.print("Level ");
            Serial.print(level);
            Serial.println(" complete! ✓");

            // Success blink
            ledSetColor(SimonColor::GREEN);
            delay(200);
            ledAllOff();
            delay(100);
            ledSetColor(SimonColor::GREEN);
            delay(200);
            ledAllOff();

            if (level >= MAX_SEQUENCE_LENGTH)
            {
                // Player won!
                Serial.println("🎉 YOU WIN! 🎉");
                setState(GameState::WIN);
            }
            else
            {
                // Next level
                level++;
                generateNextColor();
                showIndex = 0;
                lastShowMillis = 0;
                setState(GameState::SHOWING);
            }
        }
        // else: still waiting for more input — stay in INPUT state
    }
    else
    {
        // Wrong!
        Serial.print("Wrong! Expected ");
        Serial.print((int)expected);
        Serial.print(", got ");
        Serial.println((int)color);
        setState(GameState::WRONG);
    }
}

// ============================================================
// detectGesture() — Returns detected color from IMU tilt
// ============================================================
SimonColor SimonGame::detectGesture()
{
    // Read accelerometer
    float ax = 0, ay = 0, az = 0;
    if (!IMU.accelerationAvailable())
    {
        return SimonColor::NONE;
    }
    IMU.readAcceleration(ax, ay, az);

    // Debounce
    unsigned long now = millis();
    if (now - lastGestureMillis < GESTURE_DEBOUNCE_MS)
    {
        return SimonColor::NONE;
    }

    // Determine tilt direction based on dominant axis
    // On Nano 33 BLE:
    //   Flat on table: X≈0, Y≈0, Z≈ -9.8 (gravity pointing down)
    //   Tilt forward: Z becomes less negative, X becomes positive
    //   Tilt backward: Z becomes less negative, X becomes negative
    //   Tilt left: Z becomes less negative, Y becomes positive
    //   Tilt right: Z becomes less negative, Y becomes negative

    // Normalize: remove gravity
    float gravMag = sqrt(ax * ax + ay * ay + az * az);
    if (gravMag < 0.1f)
        return SimonColor::NONE; // avoid div by zero

    float nx = ax / gravMag;
    float ny = ay / gravMag;
    float nz = az / gravMag; // now ~ -1 when flat, ~0 when vertical

    // Tilt angle from horizontal
    float tiltAngle = acos(fabs(nz)) * 180.0f / M_PI; // 0° = flat, 90° = vertical

    if (tiltAngle < TILT_ANGLE_THRESHOLD)
    {
        return SimonColor::NONE; // not tilted enough
    }

    // Which axis has largest absolute value?
    float absX = fabs(nx);
    float absY = fabs(ny);

    if (absX > absY)
    {
        // Left or Right tilt
        if (nx > 0)
        {
            lastGestureMillis = now;
            return SimonColor::BLUE; // Tilt left → Blue
        }
        else
        {
            lastGestureMillis = now;
            return SimonColor::YELLOW; // Tilt right → Yellow
        }
    }
    else
    {
        // Forward or Backward tilt
        if (ny > 0)
        {
            lastGestureMillis = now;
            return SimonColor::RED; // Tilt forward → Red
        }
        else
        {
            lastGestureMillis = now;
            return SimonColor::GREEN; // Tilt backward → Green
        }
    }
}

// ============================================================
// LED control (active LOW on Nano 33 BLE Sense)
// ============================================================
void SimonGame::ledAllOff()
{
    digitalWrite(LED_PIN_R, HIGH);
    digitalWrite(LED_PIN_G, HIGH);
    digitalWrite(LED_PIN_B, HIGH);
}

void SimonGame::ledSetColor(SimonColor color)
{
    switch (color)
    {
    case SimonColor::RED:
        digitalWrite(LED_PIN_R, LOW);
        digitalWrite(LED_PIN_G, HIGH);
        digitalWrite(LED_PIN_B, HIGH);
        break;
    case SimonColor::GREEN:
        digitalWrite(LED_PIN_R, HIGH);
        digitalWrite(LED_PIN_G, LOW);
        digitalWrite(LED_PIN_B, HIGH);
        break;
    case SimonColor::BLUE:
        digitalWrite(LED_PIN_R, HIGH);
        digitalWrite(LED_PIN_G, HIGH);
        digitalWrite(LED_PIN_B, LOW);
        break;
    case SimonColor::YELLOW:
        digitalWrite(LED_PIN_R, LOW);
        digitalWrite(LED_PIN_G, LOW);
        digitalWrite(LED_PIN_B, HIGH);
        break;
    default:
        ledAllOff();
        break;
    }
}

// ledSetRGB: 0 = fully on, 255 = fully off (active LOW)
void SimonGame::ledSetRGB(uint8_t r, uint8_t g, uint8_t b)
{
    analogWrite(LED_PIN_R, 255 - r);
    analogWrite(LED_PIN_G, 255 - g);
    analogWrite(LED_PIN_B, 255 - b);
}
