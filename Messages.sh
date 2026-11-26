#! /usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Thomas Kastenholz
# SPDX-License-Identifier: LGPL-2.1-or-later
#
# Which sources go into which translation catalog, in the form KDE's translation tooling (scripty) reads;
# scripts/update-translations.sh runs it with the same $XGETTEXT. kiot: the daemon's tray, settings window
# and notifications. kcm_kiot: its System Settings module, which looks its strings up under its plugin id.
$XGETTEXT $(find core -name '*.cpp' -o -name '*.qml' | sort) -o $podir/kiot.pot
$XGETTEXT $(find kcm -name '*.cpp' -o -name '*.qml' | sort) -o $podir/kcm_kiot.pot
