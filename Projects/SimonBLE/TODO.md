# SimonBLE — Gesture-Controlled Simon Game

## Board: Arduino Nano 33 BLE Sense (ONLY onboard components)

### Components Used (all onboard — no external wiring needed)

| Component       | Pin                                            | Purpose                                  |
| --------------- | ---------------------------------------------- | ---------------------------------------- |
| Onboard RGB LED | LED_RED (D22), LED_GREEN (D23), LED_BLUE (D24) | Display colors (active LOW)              |
| LSM9DS1 IMU     | I2C (onboard)                                  | Accelerometer for tilt gesture detection |
| BLE (nRF52840)  | Built-in                                       | Game state broadcast + remote input      |
| USB Serial      | USB                                            | Debug output                             |

### Build: ✅ SUCCESS — Zero warnings, Zero errors

- RAM: 26.4% (69,288 / 262,144 bytes)
- Flash: 34.5% (339,076 / 983,040 bytes)

### How to use

1. Upload: `pio run -t upload`
2. Open Serial Monitor at 115200 baud
3. Tilt the board to play!

### Gesture Controls

| Tilt Direction | Color  | LED |
| -------------- | ------ | --- |
| Forward        | Red    | 🔴  |
| Backward       | Green  | 🟢  |
| Left           | Blue   | 🔵  |
| Right          | Yellow | 🟡  |
