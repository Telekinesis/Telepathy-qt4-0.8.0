set(tubereceiver_SRCS
    tube-receiver.cpp)

set(tubereceiver_MOC_SRCS
    tube-receiver.h)

tpqt4_generate_mocs(${tubereceiver_MOC_SRCS})

add_executable(tubereceiver ${tubereceiver_SRCS} ${tubereceiver_MOC_SRCS})
target_link_libraries(tubereceiver
    ${QT_QTDBUS_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    telepathy-qt4)

set(tubeinitiator_SRCS
    tube-initiator.cpp)

set(tubeinitiator_MOC_SRCS
    tube-initiator.h)

tpqt4_generate_mocs(${tubeinitiator_MOC_SRCS})

add_executable(tubeinitiator ${tubeinitiator_SRCS} ${tubeinitiator_MOC_SRCS})
target_link_libraries(tubeinitiator
    ${QT_QTDBUS_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    telepathy-qt4)
