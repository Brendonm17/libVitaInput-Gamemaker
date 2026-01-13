# libVitaInput-Gamemaker

A high-performance native extension for the PS Vita and PSTV, providing 4-player support, hardware haptics, motion tracking, and system diagnostics.

## Full Macro Reference

Add these to **Resources > Macros** in GMS 1.4. These hex values correspond to the hardware bitmasks of the PS Vita.

| Macro Name | Value (Hex) | Description |
| :--- | :--- | :--- |
| **DPAD** | | |
| `VITA_UP` | `$000010` | D-Pad Up |
| `VITA_RIGHT` | `$000020` | D-Pad Right |
| `VITA_DOWN` | `$000040` | D-Pad Down |
| `VITA_LEFT` | `$000080` | D-Pad Left |
| **FACE BUTTONS** | | |
| `VITA_TRIANGLE` | `$001000` | Triangle |
| `VITA_CIRCLE` | `$002000` | Circle |
| `VITA_CROSS` | `$004000` | Cross / X |
| `VITA_SQUARE` | `$008000` | Square |
| **SHOULDERS** | | |
| `VITA_L1` | `$000400` | Left Shoulder |
| `VITA_R1` | `$000800` | Right Shoulder |
| `VITA_L2` | `$000100` | DS4 L2 / L-Trigger |
| `VITA_R2` | `$000200` | DS4 R2 / R-Trigger |
| **STICKS / SYSTEM** | | |
| `VITA_L3` | `$000002` | DS4 Left Stick Click |
| `VITA_R3` | `$000004` | DS4 Right Stick Click |
| `VITA_SELECT` | `$000001` | Select Button |
| `VITA_START` | `$000008` | Start Button |
| **INPUT STATES** | | |
| `INPUT_HELD` | `0` | Button is currently down |
| `INPUT_PRESSED` | `1` | Button was pressed this frame |
| `INPUT_RELEASED` | `2` | Button was released this frame |

---

## Function Reference

### Core Logic
#### `vita_init()`
- **Args:** None
- **Returns:** `Double` (1.0 on success)
- **Details:** Sets sampling mode to `ANALOG_WIDE` and starts Motion/Touch services. Call this **once** at the very start of the game (e.g., a splash screen or a persistent controller object).

#### `vita_update()`
- **Args:** None
- **Returns:** `Double` (1.0 on success)
- **Details:** Captures current hardware state for all 4 ports. **Must be called every frame** in the `Begin Step` event of a controller object.

---

### Port & Device Info
#### `vita_is_connected(port)`
- **Args:** `port` (Double: 0-3)
- **Returns:** `Double` (1.0 if connected, 0.0 if empty)
- **Details:** Prevents reading ghost inputs from empty controller slots.

#### `vita_get_type(port)`
- **Args:** `port` (Double: 0-3)
- **Returns:** `Double` (1: Vita Handheld, 2: DS3, 3: DS4, 0: Unknown)
- **Details:** Use this to swap UI prompts (e.g., show "Options" button for DS4, "Start" for Vita).

---

### Input Polling
#### `vita_check(port, mask, type)`
- **Args:** 
    - `port`: 0-3.
    - `mask`: The button macro (e.g., `VITA_CROSS`).
    - `type`: `INPUT_HELD` (0), `INPUT_PRESSED` (1), or `INPUT_RELEASED` (2).
- **Returns:** `Double` (1.0 if condition met, 0.0 otherwise).

#### `vita_axis(port, axis_idx)`
- **Args:** 
    - `port`: 0-3.
    - `axis_idx`: `0` (LX), `1` (LY), `2` (RX), `3` (RY).
- **Returns:** `Double` (-1.0 to 1.0).

---

### Haptics & LEDs
#### `vita_set_rumble(port, small, large)`
- **Args:** 
    - `port`: 0-3 (Requires PSTV or DS4 plugin).
    - `small`: 0 or 1 (High frequency motor).
    - `large`: 0 to 255 (Low frequency intensity).
- **Returns:** `Double`.

#### `vita_set_light(port, r, g, b)`
- **Args:** 
    - `port`: 0-3 (DS4 Only).
    - `r, g, b`: 0 to 255.
- **Returns:** `Double`.

---

### Complex Hardware Data
*Note: These return strings. Use `scr_vita_parse(string, index)` to extract values.*

#### `vita_get_touch(port)`
- **Args:** `port` (0: Front, 1: Back).
- **Returns:** `String` ("count,x1,y1").
- **Indices:** 0: Finger Count, 1: X (0-1.0), 2: Y (0-1.0).

#### `vita_get_quat()`
- **Args:** None (Uses Vita system gyro).
- **Returns:** `String` ("x,y,z,w").
- **Details:** Returns 3D orientation quaternion for tilt/motion controls.

#### `vita_get_battery()`
- **Args:** None.
- **Returns:** `String` ("percent,is_charging,temp").
- **Indices:** 0: Percentage, 1: Is Charging (0/1), 2: Temperature (Celsius).

---

## Build Instructions

1.  Place `main.c`, `exports.yml`, and `Makefile` in the same folder.
2.  Open your terminal and run `make`.
3.  Copy the resulting `vita_input.suprx` into your GMS 1.4 Extension.

## GML Implementation Example

**In `obj_player` Step Event:**
```gml
// 1. Get port (e.g., Player 1 is port 0)
var p = 0;

if (vita_is_connected(p)) {
    // 2. Check for jump (Just Pressed)
    if (vita_check(p, VITA_CROSS, INPUT_PRESSED)) {
        vspeed = -10;
    }
    
    // 3. Movement (Analog)
    var move_h = vita_axis(p, 0); // Left Stick X
    x += move_h * 4;
    
    // 4. Update DS4 Lightbar based on health
    if (vita_get_type(p) == 3) {
        vita_set_light(p, 255, 0, 0); // Set red
    }
}
```

## License
MIT - Developed for the Vita Homebrew Community.
