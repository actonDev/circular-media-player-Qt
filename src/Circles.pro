#-------------------------------------------------
#
# Project created by QtCreator 2012-07-27T15:45:51
#
#-------------------------------------------------
QT += network \
      xml \
      multimedia \
      widgets \
      sql \
      svg


TEMPLATE = app


###########
# platform dependent stuff
###########

#CONFIG +=enable_download

enable_download{
win32: TARGET = "CircularMediaPlayer DOWNLOAD_ENABLED"
macx: TARGET = "Circular Media Player DOWNLOAD_ENABLED"
DEFINES += DOWNLOAD_ENABLED
}

else{
win32: TARGET = CircularMediaPlayer
macx: TARGET = "Circular Media Player"
}

win32:{
    #taglib
    DEFINES += HAVE_TAGLIB=1

    LIBS += $$PWD/taglib/1_8/win-x86-msvc-release/tag.lib
    LIBS += $$PWD/taglib/1_8/win-x86-msvc-release/zlibstatic.lib
    INCLUDEPATH += $$PWD/taglib/1_8/Headers
    DEFINES += TAGLIB_STATIC=1

    #rc file, for the program icon
    RC_FILE=circles.rc
}
macx: {
    ICON = macx/icon.icns
    QMAKE_INFO_PLIST = macx/Info.plist

    HELP.files = macx/Resources/Help
    HELP.path = Contents/Resources
    QMAKE_BUNDLE_DATA += HELP

    THEMES.files = macx/Resources/Themes
    THEMES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += THEMES

    README.files = macx/Resources/README.txt
    README.path = Contents/Resources
    QMAKE_BUNDLE_DATA += README
}



###########
#   Sources
###########


SOURCES += main.cpp\
        mainwindow.cpp \
    edgeline.cpp \
    graph.cpp \
    myview.cpp \
    librarysearch.cpp \
    node.cpp \
    player.cpp \
    myscene.cpp \
    vkontakte.cpp \
    mynetworkreply.cpp \
    internetsearch.cpp \
    resultsmodel.cpp \
    customtableview.cpp \
    historysearch.cpp \
    lyrics.cpp \
    preferences.cpp \
    tools.cpp \
    editnodedialog.cpp \
    lastfmsearch.cpp \
    menusourceaction.cpp \
    menuthemeaction.cpp \
    objectmanager.cpp \
    menualbumtrackaction.cpp \
    dialogabout.cpp \
    menutagaction.cpp \
    settings.cpp \
    dialogshortcuts.cpp \
    widgetloading.cpp \
    menuactionloading.cpp \
    menualbummenu.cpp \
    menualbumaction.cpp \
    movieplayer.cpp \
    updatecheck.cpp \
    dialogupdate.cpp \
    dialoghelp.cpp \
    videowidget.cpp \
    lastfmview.cpp \
    tagwidgetitem.cpp \
    lastfmrelated.cpp \
    lastfm.cpp \
    dialogaddtags.cpp \
    downloader.cpp \
    mysplitter.cpp \
    treewidgettag.cpp \
    treewidgetplaylist.cpp
HEADERS  += mainwindow.h \
    edgeline.h \
    graph.h \
    librarysearch.h \
    myview.h \
    node.h \
    player.h \
    myscene.h \
    vkontakte.h \
    mynetworkreply.h \
    internetsearch.h \
    resultsmodel.h \
    customtableview.h \
    historysearch.h \
    lyrics.h \
    preferences.h \
    tools.h \
    editnodedialog.h \
    lastfmsearch.h \
    menusourceaction.h \
    menuthemeaction.h \
    objectmanager.h \
    menualbumtrackaction.h \
    dialogabout.h \
    menutagaction.h \
    settings.h \
    dialogshortcuts.h \
    widgetloading.h \
    menuactionloading.h \
    menualbummenu.h \
    menualbumaction.h \
    movieplayer.h \
    updatecheck.h \
    dialogupdate.h \
    dialoghelp.h \
    videowidget.h \
    lastfmview.h \
    tagwidgetitem.h \
    lastfmrelated.h \
    lastfm.h \
    dialogaddtags.h \
    downloader.h \
    mysplitter.h \
    treewidgettag.h \
    treewidgetplaylist.h

FORMS    += mainwindow.ui \
    searchform.ui \
    lyricsform.ui \
    preferences.ui \
    editnodedialog.ui \
    dialogabout.ui \
    dialogshortcuts.ui \
    searchformonline.ui \
    dialogupdate.ui \
    dialoghelp.ui \
    lastfmview.ui \
    dialogaddtags.ui


RESOURCES += \
    resources.qrc
