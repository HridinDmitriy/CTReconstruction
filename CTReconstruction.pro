QT       += core gui concurrent opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DICOMImageReader.cpp \
    ImageResize.cpp \
    MarchingCubesAlgorithm.cpp \
    Mesh.cpp \
    OBJWriter.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    DICOMImageReader.h \
    ImageResize.h \
    MainWindow.h \
    MarchingCubesAlgorithm.h \
    MarchingCubesTables.h \
    Mesh.h \
    OBJWriter.h

FORMS += \
    MainWindow.ui

TRANSLATIONS += \
    CTReconstruction_uk_UA.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkCommonCore-9.1
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkCommonCore-9.1d

INCLUDEPATH += D:/Tools/VTK-9.1.0/install/include
DEPENDPATH += D:/Tools/VTK-9.1.0/install/include

INCLUDEPATH += D:/Tools/VTK-9.1.0/install/include/vtk-9.1
DEPENDPATH += D:/Tools/VTK-9.1.0/install/include/vtk-9.1

#win32:CONFIG(release, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkIOGeometry-9.1
#else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkIOGeometry-9.1d

win32:CONFIG(release, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkGUISupportQt-9.1
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkGUISupportQt-9.1d

win32:CONFIG(release, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkIOImage-9.1
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkIOImage-9.1d

win32:CONFIG(release, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkImagingCore-9.1
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkImagingCore-9.1d

#win32:CONFIG(release, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkIOCore-9.1
#else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkIOCore-9.1d

#win32:CONFIG(release, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkCommonDataModel-9.1
#else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkCommonDataModel-9.1d

win32:CONFIG(release, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkCommonExecutionModel-9.1
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkCommonExecutionModel-9.1d

win32:CONFIG(release, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkRenderingOpenGL2-9.1
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkRenderingOpenGL2-9.1d

#win32:CONFIG(release, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkRenderingCore-9.1
#else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkRenderingCore-9.1d

win32:CONFIG(release, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkInteractionImage-9.1
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkInteractionImage-9.1d

win32:CONFIG(release, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkInteractionStyle-9.1
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Tools/VTK-9.1.0/install/lib/ -lvtkInteractionStyle-9.1d

win32:CONFIG(release, debug|release): LIBS += -LD:/Tools/Qt/6.3.0/msvc2019_64/lib/ -lQt6OpenGLWidgets
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Tools/Qt/6.3.0/msvc2019_64/lib/ -lQt6OpenGLWidgetsd

INCLUDEPATH += D:/Tools/Qt/6.3.0/msvc2019_64/include
DEPENDPATH += D:/Tools/Qt/6.3.0/msvc2019_64/include
