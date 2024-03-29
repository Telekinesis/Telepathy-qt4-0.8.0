file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/_gen")

# Set the required flags found in TelepathyDefaults
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VISIBILITY_HIDDEN_FLAGS} ${COMPILER_COVERAGE_FLAGS} ${DEPRECATED_DECLARATIONS_FLAGS}")
set(LD_FLAGS "${LD_FLAGS} ${VISIBILITY_HIDDEN_FLAGS} ${COMPILER_COVERAGE_FLAGS} ${DEPRECATED_DECLARATIONS_FLAGS}")

# We are building Telepathy-Qt4
add_definitions(-DBUILDING_TELEPATHY_QT4)

# Sources for Tp-Qt4
set(telepathy_qt4_SRCS
    abstract-client.cpp
    abstract-interface.cpp
    account.cpp
    account-factory.cpp
    account-manager.cpp
    account-property-filter.cpp
    account-set.cpp
    account-set-internal.h
    avatar.cpp
    capabilities-base.cpp
    channel.cpp
    channel-class-spec.cpp
    channel-dispatcher.cpp
    channel-dispatch-operation.cpp
    channel-factory.cpp
    channel-internal.h
    channel-request.cpp
    client.cpp
    client-registrar.cpp
    client-registrar-internal.h
    connection.cpp
    connection-capabilities.cpp
    connection-factory.cpp
    connection-internal.h
    connection-manager.cpp
    connection-manager-internal.h
    contact.cpp
    contact-capabilities.cpp
    contact-factory.cpp
    contact-manager.cpp
    contact-manager-roster.cpp
    contact-messenger.cpp
    contact-search-channel.cpp
    dbus.cpp
    dbus-proxy.cpp
    dbus-proxy-factory.cpp
    dbus-proxy-factory-internal.h
    debug.cpp
    debug-internal.h
    fake-handler-manager-internal.cpp
    fake-handler-manager-internal.h
    feature.cpp
    file-transfer-channel.cpp
    file-transfer-channel-creation-properties.cpp
    fixed-feature-factory.cpp
    future.cpp
    future-internal.h
    handled-channel-notifier.cpp
    incoming-file-transfer-channel.cpp
    incoming-stream-tube-channel.cpp
    key-file.cpp
    location-info.cpp
    manager-file.cpp
    media-session-handler.cpp
    media-stream-handler.cpp
    message.cpp
    message-content-part.cpp
    object.cpp
    optional-interface-factory.cpp
    outgoing-file-transfer-channel.cpp
    outgoing-stream-tube-channel.cpp
    pending-account.cpp
    pending-channel.cpp
    pending-channel-request.cpp
    pending-channel-request-internal.h
    pending-connection.cpp
    pending-contact-attributes.cpp
    pending-contact-info.cpp
    pending-contacts.cpp
    pending-handles.cpp
    pending-operation.cpp
    pending-ready.cpp
    pending-send-message.cpp
    pending-string-list.cpp
    pending-stream-tube-connection.cpp
    pending-variant.cpp
    presence.cpp
    pending-variant-map.cpp
    profile.cpp
    profile-manager.cpp
    properties.cpp
    protocol-info.cpp
    protocol-parameter.cpp
    readiness-helper.cpp
    requestable-channel-class-spec.cpp
    ready-object.cpp
    referenced-handles.cpp
    request-temporary-handler-internal.cpp
    request-temporary-handler-internal.h
    room-list-channel.cpp
    simple-call-observer.cpp
    simple-observer.cpp
    simple-observer-internal.h
    simple-stream-tube-handler.cpp
    simple-text-observer.cpp
    simple-text-observer-internal.h
    stream-tube-channel.cpp
    stream-tube-client.cpp
    stream-tube-client-internal.h
    stream-tube-server.cpp
    stream-tube-server-internal.h
    streamed-media-channel.cpp
    test-backdoors.cpp
    test-backdoors.h
    text-channel.cpp
    tls-certificate.cpp
    tube-channel.cpp
    types.cpp
    types-internal.h
    utils.cpp)

# Exported headers for Tp-Qt4
set(telepathy_qt4_HEADERS
    AbstractClient
    AbstractClientApprover
    abstract-client.h
    AbstractClientHandler
    AbstractClientObserver
    AbstractInterface
    abstract-interface.h
    Account
    account.h
    AccountCapabilityFilter
    account-capability-filter.h
    AccountFactory
    account-factory.h
    AccountFilter
    account-filter.h
    AccountInterface
    AccountInterfaceAddressingInterface
    AccountInterfaceAvatarInterface
    AccountManager
    account-manager.h
    AccountManagerInterface
    account-property-filter.h
    AccountPropertyFilter
    AccountSet
    account-set.h
    AndFilter
    and-filter.h
    AuthenticationTLSCertificateInterface
    AvatarData
    AvatarSpec
    avatar.h
    CapabilitiesBase
    capabilities-base.h
    Channel
    channel.h
    ChannelClassFeatures
    channel-class-features.h
    ChannelClassSpec
    ChannelClassSpecList
    channel-class-spec.h
    ChannelDispatcher
    ChannelDispatcherInterface
    channel-dispatcher.h
    ChannelDispatchOperation
    channel-dispatch-operation.h
    ChannelDispatchOperationInterface
    ChannelFactory
    channel-factory.h
    ChannelInterface
    ChannelInterfaceAnonymityInterface
    ChannelInterfaceCallStateInterface
    ChannelInterfaceChatStateInterface
    ChannelInterfaceConferenceInterface
    ChannelInterfaceDTMFInterface
    ChannelInterfaceGroupInterface
    ChannelInterfaceHoldInterface
    ChannelInterfaceMediaSignallingInterface
    ChannelInterfaceMessagesInterface
    ChannelInterfacePasswordInterface
    ChannelInterfaceSASLAuthenticationInterface
    ChannelInterfaceSecurableInterface
    ChannelInterfaceServicePointInterface
    ChannelInterfaceTubeInterface
    ChannelRequest
    ChannelRequestHints
    channel-request.h
    ChannelRequestInterface
    ChannelTypeContactListInterface
    ChannelTypeContactSearchInterface
    ChannelTypeFileTransferInterface
    ChannelTypeRoomListInterface
    ChannelTypeServerAuthenticationInterface
    ChannelTypeServerTLSConnectionInterface
    ChannelTypeStreamedMediaInterface
    ChannelTypeStreamTubeInterface
    ChannelTypeTextInterface
    ChannelTypeTubeInterface
    ChannelTypeTubesInterface
    Client
    ClientApproverInterface
    client.h
    ClientHandlerInterface
    ClientInterface
    ClientInterfaceRequestsInterface
    ClientObserverInterface
    ClientRegistrar
    client-registrar.h
    Connection
    ConnectionCapabilities
    connection-capabilities.h
    connection.h
    ConnectionFactory
    connection-factory.h
    connection-lowlevel.h
    ConnectionInterface
    ConnectionInterfaceAliasingInterface
    ConnectionInterfaceAnonymityInterface
    ConnectionInterfaceAvatarsInterface
    ConnectionInterfaceBalanceInterface
    ConnectionInterfaceCapabilitiesInterface
    ConnectionInterfaceCellularInterface
    ConnectionInterfaceClientTypes
    ConnectionInterfaceContactBlockingInterface
    ConnectionInterfaceClientTypesInterface
    ConnectionInterfaceContactCapabilitiesInterface
    ConnectionInterfaceContactGroups
    ConnectionInterfaceContactGroupsInterface
    ConnectionInterfaceContactInfoInterface
    ConnectionInterfaceContactList
    ConnectionInterfaceContactListInterface
    ConnectionInterfaceContactsInterface
    ConnectionInterfaceLocationInterface
    ConnectionInterfaceMailNotificationInterface
    ConnectionInterfacePowerSaving
    ConnectionInterfacePowerSavingInterface
    ConnectionInterfacePresenceInterface
    ConnectionInterfaceRequestsInterface
    ConnectionInterfaceServicePointInterface
    ConnectionInterfaceSimplePresenceInterface
    ConnectionLowlevel
    ConnectionManager
    connection-manager.h
    connection-manager-lowlevel.h
    ConnectionManagerInterface
    ConnectionManagerLowlevel
    Constants
    constants.h
    Contact
    contact.h
    ContactCapabilities
    contact-capabilities.h
    ContactFactory
    contact-factory.h
    ContactManager
    contact-manager.h
    ContactMessenger
    contact-messenger.h
    ContactSearchChannel
    contact-search-channel.h
    DBus
    DBusDaemonInterface
    dbus.h
    DBusProxy
    dbus-proxy.h
    DBusProxyFactory
    dbus-proxy-factory.h
    Debug
    debug.h
    Feature
    Features
    feature.h
    FileTransferChannel
    FileTransferChannelCreationProperties
    file-transfer-channel-creation-properties.h
    file-transfer-channel.h
    Filter
    filter.h
    FixedFeatureFactory
    fixed-feature-factory.h
    GenericCapabilityFilter
    generic-capability-filter.h
    GenericPropertyFilter
    generic-property-filter.h
    Global
    global.h
    HandledChannelNotifier
    handled-channel-notifier.h
    IncomingFileTransferChannel
    incoming-file-transfer-channel.h
    IncomingStreamTubeChannel
    incoming-stream-tube-channel.h
    IntrospectableInterface
    KeyFile
    key-file.h
    LocationInfo
    location-info.h
    ManagerFile
    manager-file.h
    MediaSessionHandler
    media-session-handler.h
    MediaSessionHandlerInterface
    MediaStreamHandler
    media-stream-handler.h
    MediaStreamHandlerInterface
    Message
    message.h
    MessageContentPart
    MessageContentPartList
    message-content-part.h
    MethodInvocationContext
    method-invocation-context.h
    NotFilter
    not-filter.h
    Object
    object.h
    OptionalInterfaceFactory
    optional-interface-factory.h
    OrFilter
    or-filter.h
    OutgoingFileTransferChannel
    outgoing-file-transfer-channel.h
    OutgoingStreamTubeChannel
    outgoing-stream-tube-channel.h
    PeerInterface
    PendingAccount
    pending-account.h
    PendingChannel
    pending-channel.h
    PendingChannelRequest
    pending-channel-request.h
    PendingComposite
    PendingConnection
    pending-connection.h
    PendingContactAttributes
    pending-contact-attributes.h
    PendingContactInfo
    pending-contact-info.h
    PendingContacts
    pending-contacts.h
    PendingFailure
    PendingHandles
    pending-handles.h
    PendingOperation
    pending-operation.h
    PendingReady
    pending-ready.h
    PendingSendMessage
    pending-send-message.h
    PendingStreamedMediaStreams
    PendingStreamTubeConnection
    pending-stream-tube-connection.h
    PendingStringList
    pending-string-list.h
    PendingSuccess
    PendingVariant
    pending-variant.h
    PendingVariantMap
    pending-variant-map.h
    PendingVoid
    Presence
    presence.h
    PresenceSpec
    PresenceSpecList
    Profile
    profile.h
    ProfileManager
    profile-manager.h
    Properties
    properties.h
    PropertiesInterface
    PropertiesInterfaceInterface
    ProtocolInfo
    protocol-info.h
    ProtocolParameter
    protocol-parameter.h
    ReadinessHelper
    readiness-helper.h
    ReadyObject
    ready-object.h
    ReceivedMessage
    RefCounted
    ReferencedHandles
    referenced-handles.h
    ReferencedHandlesIterator
    requestable-channel-class-spec.h
    RequestableChannelClassSpec
    RequestableChannelClassSpecList
    RoomListChannel
    room-list-channel.h
    SharedPtr
    shared-ptr.h
    SimpleCallObserver
    simple-call-observer.h
    SimpleObserver
    simple-observer.h
    simple-pending-operations.h
    SimpleTextObserver
    simple-text-observer.h
    StatefulDBusProxy
    StatelessDBusProxy
    StreamTubeChannel
    StreamTubeClient
    StreamTubeServer
    stream-tube-channel.h
    stream-tube-client.h
    stream-tube-server.h
    StreamedMediaChannel
    streamed-media-channel.h
    StreamedMediaStream
    TextChannel
    text-channel.h
    tls-certificate.h
    TubeChannel
    tube-channel.h
    Types
    types.h
    Utils
    utils.h)

# Generated headers which will be installed and exported
set(telepathy_qt4_gen_HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-account.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-account-manager.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-channel.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-channel-dispatcher.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-channel-dispatch-operation.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-channel-request.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-client.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-connection.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-connection-manager.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-dbus.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-media-session-handler.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-media-stream-handler.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-properties.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-tls-certificate.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/constants.h
    ${CMAKE_CURRENT_BINARY_DIR}/_gen/types.h)

# Headers file moc will be run on
set(telepathy_qt4_MOC_SRCS
    abstract-interface.h
    account.h
    account-factory.h
    account-manager.h
    account-set.h
    account-set-internal.h
    channel.h
    channel-dispatch-operation.h
    channel-dispatch-operation-internal.h
    channel-factory.h
    channel-internal.h
    channel-request.h
    client-registrar.h
    client-registrar-internal.h
    connection.h
    connection-internal.h
    connection-lowlevel.h
    connection-manager.h
    connection-manager-internal.h
    connection-manager-lowlevel.h
    contact.h
    contact-manager.h
    contact-manager-internal.h
    contact-messenger.h
    contact-search-channel.h
    contact-search-channel-internal.h
    dbus-proxy.h
    dbus-proxy-factory.h
    dbus-proxy-factory-internal.h
    fake-handler-manager-internal.h
    file-transfer-channel.h
    fixed-feature-factory.h
    handled-channel-notifier.h
    incoming-file-transfer-channel.h
    incoming-stream-tube-channel.h
    object.h
    outgoing-file-transfer-channel.h
    outgoing-stream-tube-channel.h
    outgoing-stream-tube-channel-internal.h
    pending-account.h
    pending-channel.h
    pending-channel-request.h
    pending-channel-request-internal.h
    pending-connection.h
    pending-contact-attributes.h
    pending-contact-info.h
    pending-contacts.h
    pending-handles.h
    pending-operation.h
    pending-ready.h
    pending-send-message.h
    pending-stream-tube-connection.h
    pending-string-list.h
    pending-variant.h
    pending-variant-map.h
    profile-manager.h
    readiness-helper.h
    request-temporary-handler-internal.h
    room-list-channel.h
    simple-call-observer.h
    simple-pending-operations.h
    simple-observer.h
    simple-observer-internal.h
    simple-stream-tube-handler.h
    simple-text-observer.h
    simple-text-observer-internal.h
    stream-tube-channel.h
    stream-tube-client.h
    stream-tube-client-internal.h
    stream-tube-server.h
    stream-tube-server-internal.h
    streamed-media-channel.h
    text-channel.h
    tube-channel.h)

# Generate the spec files for both stable and future spec
set(gen_stable_spec_xml ${CMAKE_CURRENT_BINARY_DIR}/_gen/stable-spec.xml)
set(gen_future_spec_xml ${CMAKE_CURRENT_BINARY_DIR}/_gen/future-spec.xml)

tpqt4_xincludator(stable-ifaces-includator ${CMAKE_CURRENT_SOURCE_DIR}/stable-interfaces.xml ${gen_stable_spec_xml})
tpqt4_xincludator(future-ifaces-includator ${CMAKE_CURRENT_SOURCE_DIR}/future-interfaces.xml ${gen_future_spec_xml})

add_custom_target(all-generated-sources)

tpqt4_constants_gen(stable-constants ${gen_stable_spec_xml} ${CMAKE_CURRENT_BINARY_DIR}/_gen/constants.h
                    --namespace=Tp
                    --str-constant-prefix=TELEPATHY_
                    --define-prefix=TP_QT4_
                    --must-define=IN_TELEPATHY_QT4_HEADER
                    DEPENDS stable-ifaces-includator)
tpqt4_constants_gen(future-constants ${gen_future_spec_xml} ${CMAKE_CURRENT_BINARY_DIR}/_gen/future-constants.h
                    --namespace=TpFuture
                    --str-constant-prefix=TP_FUTURE_
                    --define-prefix=TP_QT4_FUTURE_
                    DEPENDS future-ifaces-includator)

tpqt4_types_gen(stable-typesgen ${gen_stable_spec_xml}
                ${CMAKE_CURRENT_BINARY_DIR}/_gen/types.h ${CMAKE_CURRENT_BINARY_DIR}/_gen/types-body.hpp
                Tp TelepathyQt4/types.h TelepathyQt4/Types
                --must-define=IN_TELEPATHY_QT4_HEADER
                --visibility=TELEPATHY_QT4_EXPORT
                DEPENDS stable-constants)
tpqt4_types_gen(future-typesgen ${gen_future_spec_xml}
                ${CMAKE_CURRENT_BINARY_DIR}/_gen/future-types.h ${CMAKE_CURRENT_BINARY_DIR}/_gen/future-types-body.hpp
                TpFuture TelepathyQt4/future-internal.h TelepathyQt4/future-internal.h
                DEPENDS future-constants)

# Add the generated types to the library's sources
list(APPEND telepathy_qt4_SRCS ${CMAKE_CURRENT_BINARY_DIR}/_gen/types.h)
list(APPEND telepathy_qt4_SRCS ${CMAKE_CURRENT_BINARY_DIR}/_gen/types-body.hpp)
list(APPEND telepathy_qt4_SRCS ${CMAKE_CURRENT_BINARY_DIR}/_gen/future-constants.h)
list(APPEND telepathy_qt4_SRCS ${CMAKE_CURRENT_BINARY_DIR}/_gen/future-types.h)
list(APPEND telepathy_qt4_SRCS ${CMAKE_CURRENT_BINARY_DIR}/_gen/future-types-body.hpp)

# For each spec, both stable and future, generate a cli file and add it to the sources (including mocs).
set(SPECS
    account
    account-manager
    channel
    channel-dispatcher
    channel-dispatch-operation
    channel-request
    client
    connection
    connection-manager
    dbus
    media-session-handler
    media-stream-handler
    properties
    tls-certificate)
foreach(spec ${SPECS})
    tpqt4_xincludator(${spec}-spec-xincludator ${CMAKE_CURRENT_SOURCE_DIR}/${spec}.xml ${CMAKE_CURRENT_BINARY_DIR}/_gen/spec-${spec}.xml
                      DEPENDS stable-typesgen)
    set(NEW_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-${spec}-body.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/_gen/cli-${spec}.moc.hpp)
    list(APPEND telepathy_qt4_SRCS ${NEW_FILES})
    list(APPEND telepathy_qt4_generated_specs_mocs "moc-cli-${spec}.moc.hpp")
    set_source_files_properties(${NEW_FILES} PROPERTIES GENERATED true)
endforeach(spec ${SPECS})

set(FUTURE_SPECS
    channel
    channel-dispatcher
    misc)
foreach(spec ${FUTURE_SPECS})
    tpqt4_xincludator(${spec}-future-xincludator  ${CMAKE_CURRENT_SOURCE_DIR}/future-${spec}.xml ${CMAKE_CURRENT_BINARY_DIR}/_gen/future-${spec}.xml
                      DEPENDS stable-typesgen future-typesgen)
    set(NEW_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/_gen/future-${spec}.h
        ${CMAKE_CURRENT_BINARY_DIR}/_gen/future-${spec}-body.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/_gen/future-${spec}.moc.hpp)
    list(APPEND telepathy_qt4_SRCS ${NEW_FILES})
    list(APPEND telepathy_qt4_generated_specs_mocs "moc-future-${spec}.moc.hpp")
    set_source_files_properties(${NEW_FILES} PROPERTIES GENERATED true)
endforeach(spec ${FUTURE_SPECS})

# The escape character in MSVC is ^
if(MSVC)
    set(TYPES_INCLUDE ^<TelepathyQt4/Types^> )
else(MSVC)
    set(TYPES_INCLUDE '<TelepathyQt4/Types>' )
endif(MSVC)

# Use the client generator for generating headers out of specs
tpqt4_client_generator(account clientaccount AccountManager Tp::Client --mainiface=Tp::Client::AccountInterface DEPENDS account-spec-xincludator)
tpqt4_client_generator(account-manager clientam AccountManager Tp::Client --mainiface=Tp::Client::AccountManagerInterface DEPENDS account-manager-spec-xincludator)
tpqt4_client_generator(channel clientchannel Channel Tp::Client --mainiface=Tp::Client::ChannelInterface DEPENDS channel-spec-xincludator)
tpqt4_client_generator(channel-dispatcher clientchanneldispatcher ChannelDispatcher Tp::Client --mainiface=Tp::Client::ChannelDispatcherInterface DEPENDS channel-dispatcher-spec-xincludator)
tpqt4_client_generator(channel-dispatch-operation clientchanneldispatchoperation ChannelDispatchOperation Tp::Client --mainiface=Tp::Client::ChannelDispatchOperationInterface DEPENDS channel-dispatch-operation-spec-xincludator)
tpqt4_client_generator(channel-request clientchannelrequest ChannelRequest Tp::Client --mainiface=Tp::Client::ChannelRequestInterface DEPENDS channel-request-spec-xincludator)
tpqt4_client_generator(client clientclient Client Tp::Client --mainiface=Tp::Client::ClientInterface DEPENDS client-spec-xincludator)
tpqt4_client_generator(connection clientconn Connection Tp::Client --mainiface=Tp::Client::ConnectionInterface DEPENDS connection-spec-xincludator)
tpqt4_client_generator(connection-manager clientcm ConnectionManager Tp::Client --mainiface=Tp::Client::ConnectionManagerInterface DEPENDS connection-manager-spec-xincludator)
tpqt4_client_generator(dbus clientdbus DBus Tp::Client::DBus DEPENDS dbus-spec-xincludator)
tpqt4_client_generator(media-session-handler clientmsesh MediaSessionHandler Tp::Client --mainiface=Tp::Client::MediaSessionHandlerInterface DEPENDS media-session-handler-spec-xincludator)
tpqt4_client_generator(media-stream-handler clientmstrh MediaStreamHandler Tp::Client --mainiface=Tp::Client::MediaStreamHandlerInterface DEPENDS media-stream-handler-spec-xincludator)
tpqt4_client_generator(properties clientprops Properties Tp::Client DEPENDS properties-spec-xincludator)
tpqt4_client_generator(tls-certificate clienttls TLSCertificate Tp::Client DEPENDS tls-certificate-spec-xincludator)

tpqt4_future_client_generator(channel TpFuture::Client --mainiface=Tp::Client::ChannelInterface DEPENDS channel-future-xincludator)
tpqt4_future_client_generator(channel-dispatcher TpFuture::Client --mainiface=Tp::Client::ChannelDispatcherInterface DEPENDS channel-dispatcher-future-xincludator)
tpqt4_future_client_generator(misc TpFuture::Client DEPENDS misc-future-xincludator)

if (TARGET doxygen-doc)
    add_dependencies(doxygen-doc all-generated-sources)
endif (TARGET doxygen-doc)

# Create the library
if (ENABLE_COMPILER_COVERAGE)
    add_library(telepathy-qt4 STATIC ${telepathy_qt4_SRCS})
else (ENABLE_COMPILER_COVERAGE)
    add_library(telepathy-qt4 SHARED ${telepathy_qt4_SRCS})
endif (ENABLE_COMPILER_COVERAGE)

# generate client moc files
foreach(moc_src ${telepathy_qt4_MOC_SRCS})
    set(generated_file _gen/${moc_src})
    string(REPLACE ".h" ".moc.hpp" generated_file ${generated_file})
    tpqt4_generate_moc_i_target_deps(${CMAKE_CURRENT_SOURCE_DIR}/${moc_src} ${CMAKE_CURRENT_BINARY_DIR}/${generated_file}
                                     ${telepathy_qt4_generated_specs_mocs})
    list(APPEND telepathy_qt4_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${generated_file})
    string(REPLACE ".h" ".moc.hpp" moc_src ${moc_src})
    add_dependencies(telepathy-qt4 "moc-${moc_src}")
endforeach(moc_src ${telepathy_qt4_MOC_SRCS})

# Link
target_link_libraries(telepathy-qt4
    ${QT_QTCORE_LIBRARY}
    ${QT_QTDBUS_LIBRARY}
    ${QT_QTNETWORK_LIBRARIES}
    ${QT_QTXML_LIBRARIES})

if (ENABLE_COMPILER_COVERAGE)
    target_link_libraries(telepathy-qt4 gcov)
endif (ENABLE_COMPILER_COVERAGE)

# Set the correct version number
set_target_properties(telepathy-qt4 PROPERTIES
                                    SOVERSION ${TP_QT4_ABI_VERSION}
                                    VERSION ${TP_QT4_LIBRARY_VERSION})


# Install header files
install(FILES ${telepathy_qt4_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/telepathy-1.0/TelepathyQt4 COMPONENT headers)
install(FILES ${telepathy_qt4_gen_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/telepathy-1.0/TelepathyQt4/_gen COMPONENT headers)

# Install the library - watch out for the correct components
if (WIN32)
    install(TARGETS telepathy-qt4 RUNTIME DESTINATION ${LIB_INSTALL_DIR} COMPONENT mainlibrary
                                  ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT headers)
else (WIN32)
    install(TARGETS telepathy-qt4 LIBRARY DESTINATION ${LIB_INSTALL_DIR} COMPONENT mainlibrary
                                  ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT headers)
endif (WIN32)

# pkg-config file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/TelepathyQt4.pc.in ${CMAKE_CURRENT_BINARY_DIR}/TelepathyQt4.pc)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/TelepathyQt4-uninstalled.pc.in ${CMAKE_CURRENT_BINARY_DIR}/TelepathyQt4-uninstalled.pc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/TelepathyQt4.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig COMPONENT headers)

# If Farsight was found, install its pkg-config files as well, and go into the subdirectory
if(FARSIGHT_COMPONENTS_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/TelepathyQt4Farsight.pc.in ${CMAKE_CURRENT_BINARY_DIR}/TelepathyQt4Farsight.pc)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/TelepathyQt4Farsight-uninstalled.pc.in ${CMAKE_CURRENT_BINARY_DIR}/TelepathyQt4Farsight-uninstalled.pc)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/TelepathyQt4Farsight.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig COMPONENT farsight_headers)
endif(FARSIGHT_COMPONENTS_FOUND)

add_subdirectory(Farsight)

