QT += core gui xml

TARGET = LEM
target.path = /usr/bin

TEMPLATE = app

SOURCES += src/gui/aboutdialog.cpp \
           src/main.cpp \
           src/srm/fileeraser.cpp \
           src/srm/owr/zerooverwriter.cpp \
           src/srm/owr/vsitroverwriter.cpp \
           src/srm/owr/usdodoverwriter.cpp \
           src/srm/owr/usdodeoverwriter.cpp \
           src/srm/owr/usdodeceoverwriter.cpp \
           src/srm/owr/usarmyoverwriter.cpp \
           src/srm/owr/oneoverwriter.cpp \
           src/srm/owr/navsorlloverwriter.cpp \
           src/srm/owr/navsomfmoverwriter.cpp \
           src/srm/owr/ismoverwriter.cpp \
           src/srm/owr/schneieroverwriter.cpp \
           src/srm/owr/rcmpoverwriter.cpp \
           src/srm/owr/randomoverwriter.cpp \
           src/srm/owr/overwriter.cpp \
           src/srm/owr/hmgenhancedoverwriter.cpp \
           src/srm/owr/hmgbaseoverwriter.cpp \
           src/srm/owr/gutmannoverwriter.cpp \
           src/srm/owr/gostoverwriter.cpp \
           src/srm/owr/afssioverwriter.cpp \
           src/srm/owr/pfitzneroverwriter.cpp \
           src/srm/owr/bsioverwriter.cpp \
           src/srm/memoryeraser.cpp \
           src/srm/overwriters.cpp \
           src/gui/memerasedialog.cpp \
           src/srm/freespaceeraser.cpp \
           src/gui/freespaceerasedialog.cpp \
           src/gui/fileerasedialog.cpp \
           src/gui/mainwindow.cpp \
           src/gui/settingsdialog.cpp \
           src/gui/promptdialog.cpp \
           src/gui/descriptionwidget.cpp \
           src/srm/systeminfo.cpp \
           src/gui/overwriterstab.cpp \
           src/gui/generaltab.cpp \
           src/gui/shutdowndialog.cpp \
           src/gui/reportdialog.cpp

HEADERS += src/gui/mainwindow.hpp \
           src/gui/aboutdialog.hpp \
           src/srm/fileeraser.hpp \
           src/srm/owr/zerooverwriter.hpp \
           src/srm/owr/vsitroverwriter.hpp \
           src/srm/owr/usdodoverwriter.hpp \
           src/srm/owr/usdodeoverwriter.hpp \
           src/srm/owr/usdodeceoverwriter.hpp \
           src/srm/owr/usarmyoverwriter.hpp \
           src/srm/owr/navsorlloverwriter.hpp \
           src/srm/owr/navsomfmoverwriter.hpp \
           src/srm/owr/ismoverwriter.hpp \
           src/srm/owr/hmgenhancedoverwriter.hpp \
           src/srm/owr/schneieroverwriter.hpp \
           src/srm/owr/rcmpoverwriter.hpp \
           src/srm/owr/randomoverwriter.hpp \
           src/srm/owr/overwriter.hpp \
           src/srm/owr/oneoverwriter.hpp \
           src/srm/owr/hmgbaseoverwriter.hpp \
           src/srm/owr/gutmannoverwriter.hpp \
           src/srm/owr/gostoverwriter.hpp \
           src/srm/owr/afssioverwriter.hpp \
           src/srm/overwriters.hpp \
           src/srm/owr/pfitzneroverwriter.hpp \
           src/srm/owr/bsioverwriter.hpp \
           src/srm/memoryeraser.hpp \
           src/gui/memerasedialog.hpp \
           src/srm/freespaceeraser.hpp \
           src/gui/freespaceerasedialog.hpp \
           src/gui/fileerasedialog.hpp \
           src/gui/settingsdialog.hpp \
           src/gui/promptdialog.hpp \
           src/gui/descriptionwidget.hpp \
           src/gui/taskinfo.hpp \
           src/srm/fileinfo.hpp \
           src/srm/systeminfo.hpp \
           src/gui/overwriterstab.hpp \
           src/gui/generaltab.hpp \
           src/gui/shutdowndialog.hpp \
           src/gui/reportdialog.hpp

RESOURCES += res/LEM.qrc

DEFINES += _FILE_OFFSET_BITS=64 \
           _XOPEN_SOURCE=500 \
           URANDOM_FILE=\\\"/dev/urandom\\\" \
           EXIT_ERROR=2 \
           ERASE_BLOCK_SIZE=307200 \
           PROGRESS_UPDATE_FREQ_MS=1000

CODECFORTR = UTF-8

TRANSLATIONS += i18n/LEM_ru.ts

data.path = /usr/share/LEM
data.files = share/doc share/icons share/profiles share/translations

desktop.path = /usr/share/applications
desktop.files = share/applications/LEM.desktop

icon.path = /usr/share/pixmaps
icon.files = share/pixmaps/LEM.svg

TS_DIR = share/translations

QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease

TSQM.name = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm $$TS_DIR/${QMAKE_FILE_BASE}.qm
TSQM.input = TRANSLATIONS
TSQM.output = $$TS_DIR/${QMAKE_FILE_BASE}.qm
TSQM.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm $$TS_DIR/${QMAKE_FILE_BASE}.qm
TSQM.CONFIG = no_link

QMAKE_EXTRA_COMPILERS += TSQM

PRE_TARGETDEPS += compiler_TSQM_make_all

INSTALLS += target data desktop icon
