# VTLabs Arduino Board Support

Arduino board support for VTLabs training kits. Boards reuse the official
Espressif ESP32 core; this repo adds VTLabs board definitions, pin variants,
examples and (later) the VTLabs training library.

| Component | Version |
|---|---|
| VTLabs platform (`vtlabs:esp32`) | 0.1.0 (unreleased) |
| Required Espressif core (`esp32:esp32`) | 3.3.x, tested 3.3.12 |
| Arduino IDE | 2.3.3 or newer (1.8.x not supported) |
| Arduino CLI | 1.0.4 or newer |

Boards:
- `vtlabs:esp32:vtlabs_iot_esp32_r1` — VTLabs IoT Training Kit ESP32 (Rev 1)

Install and test steps: `docs/02-stage2-board-definition.md`.
Compile everything: `scripts/compile-check.sh`.
