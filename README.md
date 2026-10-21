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

Docs:
- `docs/01-stage1-dependency-assessment.md` — verified platform research
- `docs/02-stage2-board-definition.md` — developer install, compile, upload
- `docs/03-online-deployment.md` — publish to Boards Manager from GitHub

Compile everything: `scripts/compile-check.sh`.
Release: bump `version=` in `hardware/vtlabs/esp32/platform.txt`, then `git tag vX.Y.Z && git push origin vX.Y.Z`.

Student install URL (after first release + GitHub Pages):
`https://<owner>.github.io/<repo>/package_vtlabs_index.json`
