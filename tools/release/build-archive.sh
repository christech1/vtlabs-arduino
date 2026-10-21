#!/usr/bin/env bash
# Build the Boards Manager archive for one VTLabs platform version.
# Usage: tools/release/build-archive.sh 0.1.0      -> dist/vtlabs-esp32-0.1.0.zip
#
# Archive rule (package index spec): exactly ONE folder at the root, containing
# the contents of the architecture folder (boards.txt, platform.txt, variants/...).
set -euo pipefail

VERSION="${1:?usage: build-archive.sh X.Y.Z}"
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
SRC="$ROOT/hardware/vtlabs/esp32"
NAME="vtlabs-esp32-$VERSION"
DIST="$ROOT/dist"

# The version in platform.txt must match the release being built.
PT_VERSION="$(sed -n 's/^version=//p' "$SRC/platform.txt" | tr -d '\r')"
if [ "$PT_VERSION" != "$VERSION" ]; then
  echo "platform.txt says version=$PT_VERSION but you are releasing $VERSION. Update platform.txt first." >&2
  exit 1
fi

mkdir -p "$DIST"
if [ -e "$DIST/$NAME.zip" ]; then
  echo "$DIST/$NAME.zip already exists; refusing to overwrite a release archive." >&2
  exit 1
fi

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT
mkdir "$STAGE/$NAME"
# Copy platform contents; leave out local overrides and editor junk.
(cd "$SRC" && tar --exclude='*.local.txt' --exclude='.DS_Store' --exclude='*.bak' -cf - .) | (cd "$STAGE/$NAME" && tar -xf -)
cp "$ROOT/CHANGELOG.md" "$STAGE/$NAME/" 2>/dev/null || true

# Reproducible archive: fixed timestamps, sorted entries, no extra attributes.
find "$STAGE/$NAME" -exec touch -h -d '2000-01-01T00:00:00Z' {} +
(cd "$STAGE" && find "$NAME" -print | LC_ALL=C sort | zip -q -X -D "$DIST/$NAME.zip" -@)

echo "$DIST/$NAME.zip"
