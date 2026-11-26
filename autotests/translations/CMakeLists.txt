# SPDX-FileCopyrightText: 2026 Thomas Kastenholz
# SPDX-License-Identifier: LGPL-2.1-or-later

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Qml)
find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS I18n)

kiot_add_test(translationstest translationstest.cpp LINK_LIBRARIES KF6::I18n KF6::I18nQml Qt6::Qml)
# where ki18n_install() (top-level CMakeLists.txt) puts the compiled catalogs in the build
target_compile_definitions(translationstest PRIVATE KIOT_LOCALE_DIR="${CMAKE_BINARY_DIR}/locale")
