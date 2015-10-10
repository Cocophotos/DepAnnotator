#-------------------------------------------------
#
# Project created by QtCreator 2012-08-17T11:11:11
#
#-------------------------------------------------
cache()

QT += core gui xml svg

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets printsupport
}

#QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
#QMAKE_CFLAGS_X86_64 -= -mmacosx-version-min=10.5
#QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64

INCLUDEPATH += .

TARGET = DepAnnotator
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    ui/projectwindow.cpp \
    project/projectmanager.cpp \
    ui/originalparsewidget.cpp \
    project/annotationitem.cpp \
    project/projectmodel.cpp \
    ui/projectmanagerwidget.cpp \
    scene/scenecreator.cpp \
    project/conllproject.cpp \
    scene/graphicsitem.cpp \
    ui/relationswidget.cpp \
    ui/wikigraphdialog.cpp \
    utils/config.cpp \
    project/scheme/schemereader.cpp \
    scene/commands.cpp \
    ui/nodeeditor.cpp \
    exporter/svgexporter.cpp \
    exporter/pdfexporter.cpp \
    ui/commentplainedit.cpp \
    exporter/pngexporter.cpp \
    ui/edgeeditor.cpp \
    utils/logconsole.cpp \
    ui/logconsolewidget.cpp \
    ui/preferencesdlg.cpp \
    ui/annotationtimer.cpp

HEADERS  += mainwindow.h \
    ui/projectwindow.h \
    project/projectmanager.h \
    ui/originalparsewidget.h \
    project/project.h \
    project/annotationitem.h \
    project/revision.h \
    project/projectmodel.h \
    ui/projectmanagerwidget.h \
    scene/scenecreator.h \
    graph/graph.h \
    graph/conllgraph.h \
    project/completeannotationitem.h \
    project/conllproject.h \
    scene/graphicsitem.h \
    ui/relationswidget.h \
    ui/wikigraphdialog.h \
    utils/config.h \
    project/indexer/index.h \
    project/scheme/schemereader.h \
    scene/commands.h \
    ui/nodeeditor.h \
    exporter/svgexporter.h \
    exporter/pdfexporter.h \
    exporter/exporter.h \
    ui/commentplainedit.h \
    exporter/pngexporter.h \
    ui/edgeeditor.h \
    utils/logconsole.h \
    ui/logconsolewidget.h \
    ui/preferencesdlg.h \
    ui/annotationtimer.h

UI_DIR = tmp/ui
MOC_DIR = tmp/moc
OBJECTS_DIR = tmp/obj
RCC_DIR = tmp/qrc

FORMS +=

RESOURCES += \
    DepAnnotator.qrc

ICON = img/app-icon.icns
RC_FILE = icon.rc
