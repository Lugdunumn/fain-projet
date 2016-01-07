TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    src/imgui_impl_glut.cpp \
    include/imgui/imgui.cpp \
    include/imgui/imgui_demo.cpp \
    include/imgui/imgui_draw.cpp \
    src/Image.c \
    src/Ppm.c \
    src/main.cpp \
    src/drawing.cpp

HEADERS += \
    src/Image.h \
    src/Ppm.h \
    src/imgui_impl_glut.h \
    include/imgui/imconfig.h \
    include/imgui/imgui.h \
    include/imgui/imgui_internal.h \
    include/imgui/stb_rect_pack.h \
    include/imgui/stb_textedit.h \
    include/imgui/stb_truetype.h \
    src/drawing.h

win32 {
LIBS += libfreeglut -lopengl32 -lglu32 -luser32 -lGL
}

unix {
LIBS += -lglut -lGL
}

