# Online Deployment from the Git Repository (Boards Manager)

Date: 2026-10-21
Status: Pipeline built and tested in the sandbox. **The first real publish needs your GitHub repo (Part B).**

---

## 1. How it works

```
 you push a tag  ──►  GitHub Actions "Release"
   v0.2.0             1. CI: compile every example for every board option   (stops here on failure)
                      2. build dist/vtlabs-esp32-0.2.0.zip                    (stops if platform.txt ≠ tag)
                      3. GitHub Release v0.2.0 with the zip attached          (immutable, versioned)
                      4. append 0.2.0 to package_vtlabs_index.json on gh-pages
                      5. download the published zip, check SHA-256 + size match the index

 students  ──►  Arduino IDE Preferences: https://<owner>.github.io/<repo>/package_vtlabs_index.json
                Boards Manager shows "VTLabs Training Kits", every version, with Update button
```

| Piece | Where it lives | Changes when |
|---|---|---|
| Source (boards.txt, variants, examples, docs) | `main` branch | every commit |
| Release archives `vtlabs-esp32-X.Y.Z.zip` | GitHub Releases | once per version, never edited |
| Index `package_vtlabs_index.json` + `index.html` | `gh-pages` branch → GitHub Pages | one entry appended per release |

The `main` branch never contains the index, and `gh-pages` never contains source. The workflow creates `gh-pages` the first time.

---

## 2. Files added

| File | Purpose |
|---|---|
| `.github/workflows/ci.yml` | On every push/PR: install Arduino CLI 1.5.0 + esp32 3.3.12, compile all sketches, test that the archive and index can be built |
| `.github/workflows/release.yml` | On tag `vX.Y.Z`: CI, then build, release, index, verify |
| `tools/release/build-archive.sh` | Builds the zip: one root folder, reproducible timestamps, refuses version mismatch or overwrite |
| `tools/release/make_index.py` | `add` / `deprecate` / `check` the index; SHA-256 and size computed from the real file; refuses to re-publish an existing version; adds `libraryDependencies` from `libraries.txt` |
| `tools/release/update-pages.sh` | Adds a version to the gh-pages checkout and writes a small install page (`index.html`) |

---

## 3. What was verified (sandbox, 2026-10-21)

| Test | Result |
|---|---|
| Archive layout = one root folder (`vtlabs-esp32-0.1.0/boards.txt …`) | PASS |
| Rebuilding an existing version / re-adding it to the index | Refused (immutability works) |
| Index served over HTTP → `arduino-cli core install vtlabs:esp32@0.1.0` with the **sketchbook copy removed** | Installed to `packages/vtlabs/hardware/esp32/0.1.0`; Blink compiled (271,640 B) |
| Upgrade 0.1.0 → 0.1.1 (`core upgrade`) | PASS |
| Rollback to 0.1.0 (`core install vtlabs:esp32@0.1.0`) | PASS |
| `deprecate 0.1.1` | Boards Manager lists it as `[DEPRECATED]` |
| `libraryDependencies` (2026-10-22): index served locally, `core install vtlabs:esp32@0.2.0` with Arduino CLI 1.5.0 and a local stand-in for the Library Manager index | PubSubClient 2.8 and MD_MAX72XX 3.5.1 downloaded and installed **before** the platform; 0.2.0 archive contains VTLabsKit + 64 lessons |
| Tampered archive | Install refused: "archive hash differs from hash in index" |
| Release job's git steps against a simulated remote, two releases | gh-pages holds `.nojekyll`, `index.html`, index with both versions; `main` untouched |
| Both workflows against GitHub's official workflow JSON schema | PASS |
| Bash syntax of every workflow step and script | PASS |

**Not yet verified:** an actual run on GitHub's servers (`gh release create`, pushing to gh-pages, Pages serving the file). The first real tag is that test — Part B, step 6.

The sandbox used `XI-TEK/vtlabs-arduino` only as an example name. The real URLs come from whatever repo you create; nothing is hard-coded.

---

## 4. Part B — first deployment, step by step

### Step 1 — Create the GitHub repository
1. github.com → **New repository**. Suggested name `vtlabs-arduino`, owner = your account or your organisation.
2. **Public.** Students' Arduino IDE downloads the index and zip without logging in. A private repo will not work for Boards Manager (see §6 for alternatives).
3. Do **not** add a README/licence on GitHub (the repo already has them).

### Step 2 — Fill in maintainer details
Edit `tools/release/make_index.py`, near the top: set `"maintainer"` (shown in Boards Manager, e.g. `"VTLabs (XI-TEK LTD)"`) and add a support email via the `VTLABS_EMAIL` line or directly. Commit.

### Step 3 — Push the code
From the unzipped repo folder:
```
git remote add origin https://github.com/<owner>/vtlabs-arduino.git
git push -u origin main
```

### Step 4 — Check CI is green
GitHub → **Actions** tab → "CI" should finish with a green tick (~5–10 min the first time, mostly downloading the ESP32 core). If it's red, open the failed step and send me the log.

### Step 5 — Allow the workflow to publish
Settings → **Actions → General → Workflow permissions** → "Read and write permissions" → Save.
(The workflow already asks for `contents: write`; organisation repos sometimes block that by default.)

### Step 6 — Publish the first release
1. In `CHANGELOG.md`, rename `## [Unreleased] 0.2.0` to `## [0.2.0] - <date>`. Commit and push.
2. Tag and push:
   ```
   git tag v0.2.0
   git push origin v0.2.0
   ```
3. Actions → "Release" runs. When green you have: a Release `v0.2.0` with `vtlabs-esp32-0.2.0.zip`, and a new `gh-pages` branch.

### Step 7 — Turn on GitHub Pages (once)
Settings → **Pages** → Source: "Deploy from a branch" → Branch: `gh-pages`, folder `/ (root)` → Save.
After 1–2 minutes, open `https://<owner>.github.io/vtlabs-arduino/`. You should see the install page, and `…/package_vtlabs_index.json` should show the JSON.
(Note: Pages hostnames are lowercase, e.g. owner `XI-TEK` → `xi-tek.github.io`.)

### Step 8 — Test as a student, on a clean PC
1. **Remove** your developer copy `Documents\Arduino\hardware\vtlabs` and restart the IDE (otherwise the IDE uses that copy and you're not testing the release).
2. File → Preferences → Additional boards manager URLs: add the Espressif URL **and**
   `https://<owner>.github.io/vtlabs-arduino/package_vtlabs_index.json` (one per line).
3. Boards Manager → install **esp32 by Espressif Systems 3.3.12**, then search **VTLabs** → install **VTLabs Training Kits 0.2.0**. Arduino IDE 2.4+ also installs the example libraries listed in `libraries.txt` (older IDEs: see [libraries.md](libraries.md)).
4. Select the board, upload Blink. This closes Stage 8 of the plan.

---

## 5. Everyday release procedure (after the first one)

1. Make changes on `main`; CI must be green.
2. Bump `version=` in `hardware/vtlabs/esp32/platform.txt` (semantic versioning, see below).
3. Add a `## [X.Y.Z] - date` section to `CHANGELOG.md`.
4. Commit, push, then `git tag vX.Y.Z && git push origin vX.Y.Z`.
5. Students see **Update** in Boards Manager.

| Change | Bump | Example |
|---|---|---|
| Fix a typo, add an example, add a menu option | PATCH | 0.1.0 → 0.1.1 |
| Add a new board / hardware revision, new library feature | MINOR | 0.1.1 → 0.2.0 |
| Remove a board, rename an FQBN, change an existing pin | MAJOR | 0.2.0 → 1.0.0 (avoid: add a new `_r2` board instead) |

If you tag the wrong version (tag ≠ `platform.txt`), the release stops at the build step and nothing is published. Delete the tag (`git push --delete origin vX.Y.Z`), fix, re-tag.

---

## 6. Rollback when a release is broken

Releases are never edited or replaced; a fix is always a **new, higher** version.

1. **Fast fix (preferred):** revert the bad commit on `main`, bump PATCH (e.g. 0.2.0 broken → 0.2.1 = 0.1.x content), tag. Boards Manager offers 0.2.1 as the update.
2. **Warn students meanwhile:** on a gh-pages checkout run
   ```
   python3 tools/release/make_index.py deprecate --index package_vtlabs_index.json --version 0.2.0
   ```
   commit and push gh-pages. Boards Manager shows `[DEPRECATED]` next to it.
3. **Students who already installed the bad version:** Boards Manager → VTLabs Training Kits → version dropdown → pick the last good version → Install. (Verified: downgrade works.)
4. Do **not** delete a release asset that's listed in the index — anyone selecting that version would get a download error.

---

## 7. Notes and limits

- **Private repo / own server:** if the code must stay private, publish only the zip and index somewhere public, e.g. `https://xitekltd.com/arduino/`. Build locally with `build-archive.sh`, then `make_index.py add --url-base https://xitekltd.com/arduino/` and upload both files. Students' URL becomes `https://xitekltd.com/arduino/package_vtlabs_index.json`.
- **The Espressif core is not auto-installed** (index format limitation, see Stage 1). The install page and guide always list it as step 1; the variant's version guard catches a wrong version.
- **Index caching:** the IDE refreshes indexes when Boards Manager opens; GitHub Pages can take a few minutes after a release to serve the new file.
- **Libraries:** once the VTLabs training library exists it goes in `hardware/vtlabs/esp32/libraries/` and ships inside the same archive (platform-bundled library) — no separate Library Manager submission needed.
