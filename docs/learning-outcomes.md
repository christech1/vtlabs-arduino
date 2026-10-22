# VTLabs IoT Training Kit - Learning Outcomes

Audience: TVET / university students with basic computer skills; no prior programming required for
Level 1. Total guided time: about **90 hours** (lessons + exercises), plus a final project.

Each lesson's sketch (File > Examples > VTLabsKit) contains *Experiments* - small changes students make
and explain. Those experiments are the main evidence for the outcomes below.

---

## 1. Programme outcomes

On completing the course, the student can:

| ID | Programme outcome |
|---|---|
| P1 | Set up the Arduino IDE for a third-party board, install platforms and libraries, and upload firmware to an ESP32. |
| P2 | Write structured Arduino C++ programs (variables, functions, conditions, loops, state machines, non-blocking timing with `millis()`). |
| P3 | Read digital and analog sensors, convert raw values into engineering units, and filter noisy data. |
| P4 | Drive actuators (LEDs, PWM, DAC, buzzer, relay) safely, with defined start-up and fail-safe states. |
| P5 | Use serial buses (UART, I2C, SPI, RS485) to exchange data with modules and other controllers, and diagnose bus faults. |
| P6 | Build human-machine interfaces with a matrix keypad, OLED display and LED matrix: input validation, menus, settings, status display. |
| P7 | Connect devices to a network: Wi-Fi, local web server, JSON API, data logging and MQTT. |
| P8 | Design, document and test an integrated IoT system against a written requirement, including a hardware test record. |
| P9 | Work safely: low-voltage practice, power budgeting, ESD and protection of GPIO pins. |

---

## 2. Outcomes by level

### Level 1 - Digital input and output (8 h) - lessons L1_01 to L1_08
Students can:
1. Install the VTLabs board package and upload a sketch; recover from a failed upload (BOOT/EN).
2. Configure pins with `pinMode()` and use `digitalRead()` / `digitalWrite()`.
3. Explain pull-up resistors and "active LOW" inputs; draw the button circuit.
4. Distinguish level and edge detection; implement a toggle.
5. Explain contact bounce and implement software debouncing with `millis()`.
6. Read a digital sensor (PIR) and describe its warm-up and hold time.
7. Switch a relay and buzzer through transistor drivers with a safe default state and timeout.
8. Implement a state machine (traffic light) that stays responsive to inputs.

*Evidence:* working traffic light with pedestrian button; written answer on why `delay()` blocks inputs.

### Level 2 - Analog input, PWM and DAC (8 h) - L2_01 to L2_08
Students can:
1. Read a voltage with the 12-bit ADC and state the input range and resolution.
2. Convert counts to millivolts using factory calibration, and calibrate a sensor with two points.
3. Apply and compare moving-average and exponential filters using the Serial Plotter.
4. Generate PWM with `ledcAttach()` / `ledcWrite()` and explain duty cycle and frequency.
5. Explain the difference between ADC (input), PWM (switched output) and DAC (true analog output), and
   measure each with a multimeter / oscilloscope.
6. Use `map()` / `constrain()` and dead-bands; generate tones with a passive buzzer.

*Evidence:* table of measured vs. expected voltages for PWM and DAC at 25/50/75 %.

### Level 3 - Communication (12 h) - L3_01 to L3_09
Students can:
1. Use the USB serial port for structured diagnostics and log levels.
2. Exchange data between two UART devices, crossing TX/RX and sharing GND; explain baud-rate mismatch.
3. Parse text commands robustly and return OK/ERROR responses.
4. Scan an I2C bus, identify devices by address, and diagnose missing pull-ups or wrong wiring.
5. Read a single-wire digital sensor (DHT22) and handle failed readings.
6. Read an I2C sensor (BME280) and present calibrated values.
7. Draw text and graphics on an I2C OLED.
8. Use SPI with chip-select to write and read files on an SD card.
9. Explain differential signalling, half-duplex direction control and termination in RS485.

*Evidence:* I2C scanner output for the student's kit; two-kit UART or RS485 exchange demonstrated.

### Level 4 - IoT networking (12 h) - L4_01 to L4_06
Students can:
1. Connect to Wi-Fi with timeout and reconnection; interpret RSSI and status codes.
2. Host a web server with routes, parameters and correct HTTP status codes.
3. Separate data (JSON API) from presentation (HTML/JavaScript dashboard).
4. Control outputs from a web page using POST requests, a local override and a timeout (safe remote control).
5. Log measurements to the flash file system and download them as CSV.
6. Publish and subscribe with MQTT; explain topics, broker and why public test brokers are not private.

*Evidence:* dashboard screenshot from a phone; downloaded CSV plotted in a spreadsheet.

### Level 5 - Integrated projects (12 h) - L5_01 to L5_06
Students can:
1. Combine sensors and actuators with hysteresis to avoid oscillation (lighting, fan control).
2. Choose and justify a fail-safe state for a controller.
3. Build multi-page local displays with min/max statistics and alarm levels.
4. Time-stamp data with NTP and keep a persistent log.
5. Run the ESP32 as an access point for control without infrastructure.
6. Build an end-to-end sensor-to-dashboard system with history and alarm limit.

*Evidence:* one project demonstrated with a short written description (inputs, outputs, logic, limits).

### Level 6 - Matrix keypad (10 h) - L6_01 to L6_08
Students can:
1. Explain how a matrix keypad is scanned (rows driven, columns read) and why ghosting occurs.
2. Identify keypad dimensions, row/column wiring and an I2C expander, and configure them.
3. Read individual keys with debouncing, and handle press, hold and release events.
4. Implement numeric input with editing and range validation.
5. Implement PIN entry with masking, attempt limit, lock-out and a stored, changeable PIN.
6. Build a menu system (navigate, select, back) on a display.
7. Store user-configurable settings in non-volatile memory with validation and factory reset.
8. Use the keypad as a control panel for LEDs, sensors and actuators.

*Evidence:* PIN lock with lock-out demonstrated; settings survive a power cycle.

### Level 7 - LED matrix drivers (8 h) - L7_01 to L7_05
Students can:
1. Identify an LED matrix's driver (MAX7219 or WS2812) from the hardware.
2. Explain the MAX7219 serial interface (DIN/CLK/CS, daisy chain, registers) and configure module type,
   device count and intensity.
3. Explain WS2812 one-wire timing, strip order, colour order and pixel mapping, and determine them experimentally.
4. Calculate the current of a WS2812 panel and choose a power supply and brightness limit.
5. Explain why a 3.3 V -> 5 V level shifter is needed for reliable WS2812 data.

*Evidence:* completed mapping sheet (pixel 0 position, layout, colour order) and power calculation.

### Level 8 - LED matrix, portable API (8 h) - L8_01 to L8_07
Students can:
1. Address individual pixels by (x, y) and correct orientation.
2. Display numbers, symbols and custom icons (bitmap design in hex).
3. Generate patterns mathematically and create frame-based and rule-based animations.
4. Scroll text without blocking the program.
5. Present sensor measurements as number, bar graph or text and justify the choice.
6. Design a priority-based status/warning display.

*Evidence:* own icon designed and displayed; status display with at least three priorities.

### Level 9 - Integrated exercises (12 h) - L9_01 to L9_07
Students can:
1. Combine keypad, LED matrix, sensors, actuators and network in one program structured as a state machine.
2. Handle optional hardware at compile time (`#if VTL_HAS_MATRIX`).
3. Implement access control, threshold configuration and alarm acknowledgement workflows.
4. Keep local indication consistent with remote (web) status.

*Evidence:* each exercise's *Task* extension completed and demonstrated.

---

## 3. Assessment rubric (per level)

| Criterion | Beginning (1) | Developing (2) | Proficient (3) | Advanced (4) |
|---|---|---|---|---|
| Functionality | Example uploaded but not working as expected | Works with help | Works; experiments completed | Extended beyond the experiments |
| Understanding | Cannot explain the code | Explains parts | Explains all lines and the hardware | Explains trade-offs and alternatives |
| Debugging | Needs step-by-step help | Finds errors with hints | Uses Serial/scanner/multimeter independently | Diagnoses unfamiliar faults systematically |
| Safety & quality | Unsafe wiring or blocking code | Safe with reminders | Safe defaults, no blocking delays where it matters | Adds fail-safes and validation unprompted |
| Documentation | None | Partial notes | Clear notes with measurements | Test record others can repeat |

Pass = average 2.5 or higher, and "Safety & quality" at least 3 for Levels 1, 4, 5, 9.

---

## 4. Final project (20 h)

Students write a one-page requirement (inputs, outputs, user interface, network, safety), build it on
the kit using at least: one analog sensor, one digital/bus sensor, the keypad **or** LED matrix,
one actuator with a fail-safe, and a network interface. They submit the sketch, a wiring/pin table
(using the `VTL_PIN_` names), and a hardware test record in the format of
[hardware-validation-checklist.md](hardware-validation-checklist.md).

## 5. Safety outcomes (all levels)

- Only low-voltage loads (<= 24 V DC) on the relay in class; never mains.
- Check the power budget before connecting LED panels or motors.
- Never connect 5 V signals directly to ESP32 GPIOs.
- Disconnect power before changing wiring.
