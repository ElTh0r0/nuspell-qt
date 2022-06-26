TEMPLATE = app

TARGET   = nuspellchecker-qt

QT      += core gui widgets
CONFIG  += c++17

SOURCES += main.cpp \
           dialog.cpp \
           nuspellcheck.cpp \
           nuspellcheckdialog.cpp

HEADERS += dialog.h \
           nuspellcheck.h \
           nuspellcheckdialog.h

FORMS   += dialog.ui \
           nuspellcheckdialog.ui

unix {
   LIBS += -lnuspell -licuuc -licudata
}

win32 {
   INCLUDEPATH += C:/path/to/nuspell/include
   LIBS += C:/path/to/nuspell/nuspell.lib
}
