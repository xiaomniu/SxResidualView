#-------------------------------------------------
#
# Project created by QtCreator 2021-09-28T09:12:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ResidualView
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
CONFIG += console

QMAKE_CXXFLAGS_RELEASE += -O

INCLUDEPATH += \
        . \
        ./Src_Include \

SOURCES += \
        SOIL2/SOIL2.cpp \
        SOIL2/image_DXT.cpp \
        SOIL2/image_helper.cpp \
        SOIL2/wfETC.cpp \
        Src_Attribute/CChunkAttribute.cpp \
        Src_Attribute/CLayerAttribute.cpp \
        Src_Core/CGeoMath.cpp \
        Src_Core/CGeoRelation.cpp \
        Src_Core/CGlobal.cpp \
        Src_Core/CThreadPool.cpp \
        Src_Core/Misc.cpp \
        Src_Geometry/CBaseMap.cpp \
        Src_Geometry/CDrawGround.cpp \
        Src_Geometry/CLayerDraw.cpp \
        Src_Geometry/CLayerGeoDraw.cpp \
        Src_Geometry/CLayerLines.cpp \
        Src_Geometry/CLayerPoints.cpp \
        Src_Geometry/CLayerPolygons.cpp \
        Src_Geometry/CLayerPolygonsEdit.cpp \
        Src_Geometry/CLayerResidual.cpp \
        Src_Geometry/CLayerVector.cpp \
        Src_Geometry/CRasterDraw.cpp \
        Src_Geometry/CRasterThumbnail.cpp \
        Src_Geometry/CReadRaster.cpp \
        Src_Geometry/CTifAvailBound.cpp \
        Src_GlWnd/CCamera.cpp \
        Src_GlWnd/CMainViewWidget.cpp \
        Src_GlWnd/CMainViewWidgetSub.cpp \
        Src_GlWnd/COpenGLCore.cpp \
        Src_GlWnd/COpenGLCoreSub.cpp \
        Src_GlWnd/FontRenderer.cpp \
        Src_LayerManager/CLayerManagerWidget.cpp \
        Src_LayerManager/CTreeWidget.cpp \
        Src_LayerManager/CTreeWidgetItem.cpp \
        Src_MatchPoints/Class_RPCs.cpp \
        Src_MatchPoints/ReadResidualFile.cpp \
        Src_MenuBar/CMenuBar.cpp \
        Src_MenuBar/CPicTextLabel.cpp \
        Src_MenuBar/CToolMenuHandler.cpp \
        main.cpp \
        CMainWindow.cpp

HEADERS += \
        CMainWindow.h \
        SOIL2/SOIL2.h \
        SOIL2/image_DXT.h \
        SOIL2/image_helper.h \
        SOIL2/jo_jpeg.h \
        SOIL2/pkm_helper.h \
        SOIL2/pvr_helper.h \
        SOIL2/stb_image.h \
        SOIL2/stb_image_write.h \
        SOIL2/stbi_DDS.h \
        SOIL2/stbi_DDS_c.h \
        SOIL2/stbi_ext.h \
        SOIL2/stbi_ext_c.h \
        SOIL2/stbi_pkm.h \
        SOIL2/stbi_pkm_c.h \
        SOIL2/stbi_pvr.h \
        SOIL2/stbi_pvr_c.h \
        SOIL2/wfETC.h \
        Src_Attribute/CChunkAttribute.h \
        Src_Attribute/CLayerAttribute.h \
        Src_Core/BaseVectorStruct.h \
        Src_Core/CGeoMath.h \
        Src_Core/CGeoRelation.h \
        Src_Core/CGlobal.h \
        Src_Core/CThreadPool.h \
        Src_Core/DefType.h \
        Src_Core/Misc.h \
        Src_Geometry/CBaseMap.h \
        Src_Geometry/CDrawGround.h \
        Src_Geometry/CLayerDraw.h \
        Src_Geometry/CLayerGeoDraw.h \
        Src_Geometry/CLayerLines.h \
        Src_Geometry/CLayerPoints.h \
        Src_Geometry/CLayerPolygons.h \
        Src_Geometry/CLayerPolygonsEdit.h \
        Src_Geometry/CLayerResidual.h \
        Src_Geometry/CLayerVector.h \
        Src_Geometry/CRasterDraw.h \
        Src_Geometry/CRasterThumbnail.h \
        Src_Geometry/CReadRaster.h \
        Src_Geometry/CTifAvailBound.h \
        Src_GlWnd/CCamera.h \
        Src_GlWnd/CMainViewWidget.h \
        Src_GlWnd/CMainViewWidgetSub.h \
        Src_GlWnd/COpenGLCore.h \
        Src_GlWnd/COpenGLCoreSub.h \
        Src_GlWnd/FontRenderer.h \
        Src_LayerManager/CLayerManagerWidget.h \
        Src_LayerManager/CTreeWidget.h \
        Src_LayerManager/CTreeWidgetItem.h \
        Src_MatchPoints/Class_RPCs.h \
        Src_MatchPoints/ReadResidualFile.h \
        Src_MenuBar/CMenuBar.h \
        Src_MenuBar/CMenuItemWidget.h \
        Src_MenuBar/CPicTextLabel.h \
        Src_MenuBar/CToolMenuHandler.h

FORMS += \
        CMainWindow.ui \
        Src_Attribute/CChunkAttribute.ui \
        Src_Attribute/CLayerAttribute.ui \
        Src_GlWnd/CMainViewWidget.ui \
        Src_GlWnd/CMainViewWidgetSub.ui \
        Src_LayerManager/CLayerManagerWidget.ui \
        Src_MenuBar/CMenuBar.ui \
        Src_MenuBar/CPicTextLabel.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


LIBS += -LD:/QT_CODE_PRJ/SuperXView02/lib -lgdal_i
LIBS += -LD:/QT_CODE_PRJ/SuperXView02/lib -lgeos
LIBS += -LD:/QT_CODE_PRJ/SuperXView02/lib -lgeos_i
LIBS += -LD:/QT_CODE_PRJ/SuperXView02/lib -lgeos_c
LIBS += -LD:/QT_CODE_PRJ/SuperXView02/lib -lgeos_c_i
LIBS += -LD:/QT_CODE_PRJ/SuperXView02/lib -llibgeos

LIBS += -LD:/QT_CODE_PRJ/SuperXView02/lib -lfreetype
