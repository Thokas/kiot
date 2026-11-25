#!/bin/sh
# SPDX-FileCopyrightText: 2026 Thomas Kastenholz
# SPDX-License-Identifier: LGPL-2.1-or-later
#
# Extracts the translatable strings (Messages.sh) and merges them into every po/<language>/<catalog>.po,
# which keeps the existing translations and marks new or changed strings for the translator.
#   scripts/update-translations.sh        update all languages
#   scripts/update-translations.sh fr     also start po/fr/ for a new language
# Needs gettext (xgettext, msgmerge, msginit).
set -e
cd "$(dirname "$0")/.."
# msginit takes the charset of a new catalog from the locale: UTF-8, whatever the caller's
export LC_ALL=C.UTF-8
podir=$(mktemp -d)
trap 'rm -rf "$podir"' EXIT
# the keywords and flags KDE's scripty uses
XGETTEXT="xgettext --from-code=UTF-8 -C --kde --package-name=kiot
  --msgid-bugs-address=https://github.com/davidedmundson/kiot/issues
  -ci18n -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3
  -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3
  -kxi18n:1 -kxi18nc:1c,2 -kxi18np:1,2 -kxi18ncp:1c,2,3
  -kkxi18n:1 -kkxi18nc:1c,2 -kkxi18np:1,2 -kkxi18ncp:1c,2,3
  -kI18N_NOOP:1 -kI18NC_NOOP:1c,2"
export XGETTEXT podir
sh ./Messages.sh

for pot in "$podir"/*.pot; do
    catalog=$(basename "$pot" .pot)
    if [ -n "$1" ] && [ ! -e "po/$1/$catalog.po" ]; then
        mkdir -p "po/$1"
        msginit --no-translator --locale="$1" --input="$pot" --output-file="po/$1/$catalog.po"
    fi
    for po in po/*/"$catalog".po; do
        [ -e "$po" ] || continue
        msgmerge --quiet --update --backup=none "$po" "$pot"
        echo "$po: $(msgfmt --statistics -o /dev/null "$po" 2>&1)"
    done
done
