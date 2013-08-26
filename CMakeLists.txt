cmake_minimum_required (VERSION 2.6)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(FORM_DIR ${PROJECT_SOURCE_DIR}/forms)

find_package(Qt4 REQUIRED)
find_package(OpenCV 2.3 REQUIRED)

include(${QT_USE_FILE})

add_definitions(${QT_DEFINITIONS})

add_definitions(-Wall -O2 -ffast-math -funroll-loops -ftree-vectorize -fomit-frame-pointer -pipe -mfpmath=sse -mmmx -msse -mtune=core2 -march=core2 -msse2 -msse3 -mssse3 -msse4)

set(MOC_HEADERS
    ${SRC_DIR}/OpenCvWidget.h
    ${SRC_DIR}/ThermoCamThread.h
    )

set(FORMS
    ${FORM_DIR}/MainWidget.ui
    )

QT4_WRAP_CPP(HEADERS_MOC ${MOC_HEADERS})
QT4_WRAP_UI(FORMS_MOC ${FORMS})

include_directories(${PROJECT_SOURCE_DIR}/build
                    ${SRC_DIR}
                    ${OpenCV_INCLUDE_DIRS}
                    )

link_directories(${OpenCV_LIBRARY_DIRS}
                 ${PROJECT_SOURCE_DIR}/lib
                 )

add_executable(optris-calibration-tool
               ${SRC_DIR}/main.cpp
               ${SRC_DIR}/OpenCvWidget.cpp
               ${SRC_DIR}/MainWidget.cpp
               ${SRC_DIR}/ThermoCamThread.cpp
               ${SRC_DIR}/ThermoCam.cpp
               ${SRC_DIR}/ImageBuilder.cpp
               ${SRC_DIR}/ImagerUVC.cpp
               ${SRC_DIR}/VideoDevice.cpp
               ${HEADERS_MOC}
               ${FORMS_MOC}
               )

target_link_libraries(optris-calibration-tool
                      ${QT_LIBRARIES}
                      ${OpenCV_LIBS}
                      Imager
                      ImageProcessing
                      udev
                      )
