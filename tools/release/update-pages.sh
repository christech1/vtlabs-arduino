#!/usr/bin/env bash
# Add a built release to the Boards Manager index kept on the gh-pages branch.
# Used by .github/workflows/release.yml; can also be run by hand.
#
# Usage: tools/release/update-pages.sh <version> <owner/repo> <site-dir>
#   <site-dir> must be a checkout of the gh-pages branch (the workflow makes one).
# The release archive must already be in dist/. The commit is made but NOT pushed.
set -euo pipefail

VERSION="${1:?version}"
REPO="${2:?owner/repo}"
SITE="${3:?site dir}"
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
ARCHIVE="$ROOT/dist/vtlabs-esp32-$VERSION.zip"
OWNER="${REPO%%/*}"
NAME="${REPO#*/}"
PAGES_URL="https://${OWNER,,}.github.io/$NAME"
INDEX_URL="$PAGES_URL/package_vtlabs_index.json"

[ -f "$ARCHIVE" ] || { echo "missing $ARCHIVE - run build-archive.sh first" >&2; exit 1; }

VTLABS_WEBSITE="https://github.com/$REPO" VTLABS_HELP="https://github.com/$REPO#readme" \
python3 -I "$ROOT/tools/release/make_index.py" add \
  --index "$SITE/package_vtlabs_index.json" \
  --archive "$ARCHIVE" \
  --version "$VERSION" \
  --boards-txt "$ROOT/hardware/vtlabs/esp32/boards.txt" \
  --url-base "https://github.com/$REPO/releases/download/v$VERSION"

touch "$SITE/.nojekyll"
cat > "$SITE/index.html" <<EOF
<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>VTLabs Boards Manager</title>
<style>body{font:16px/1.5 system-ui,sans-serif;max-width:720px;margin:2rem auto;padding:0 16px}code{background:#eee;padding:2px 6px;border-radius:4px;word-break:break-all}</style>
</head><body>
<h1>VTLabs Training Kits &mdash; Arduino Boards Manager</h1>
<ol>
<li>Install <b>Arduino IDE 2.3.3 or newer</b>.</li>
<li>File &rarr; Preferences &rarr; Additional boards manager URLs, add:<br><code>$INDEX_URL</code></li>
<li>Boards Manager: install <b>esp32 by Espressif Systems 3.3.x</b>, then <b>VTLabs Training Kits</b>.</li>
<li>Tools &rarr; Board &rarr; VTLabs Training Kits &rarr; your kit.</li>
</ol>
<p>Latest version: <b>$VERSION</b>. Source and releases: <a href="https://github.com/$REPO">github.com/$REPO</a></p>
</body></html>
EOF

git -C "$SITE" add -A
git -C "$SITE" -c user.name="${GIT_AUTHOR_NAME:-vtlabs-release-bot}" -c user.email="${GIT_AUTHOR_EMAIL:-vtlabs-release-bot@users.noreply.github.com}" \
  commit -q -m "Publish vtlabs:esp32 $VERSION to Boards Manager index"
echo "index updated; students use: $INDEX_URL"
