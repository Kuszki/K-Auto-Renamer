QT += core gui widgets concurrent

CONFIG += c++20

SOURCES += \
    aboutdialog.cpp \
    common.cpp \
    logsdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    threadworker.cpp

HEADERS += \
    aboutdialog.hpp \
    common.hpp \
    logsdialog.hpp \
    mainwindow.hpp \
    settingsdialog.hpp \
    threadworker.hpp

FORMS += \
    aboutdialog.ui \
    logsdialog.ui \
    mainwindow.ui \
    settingsdialog.ui

TRANSLATIONS += \
    K-Auto-Renamer_pl_PL.ts
