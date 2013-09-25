/**
 * This file is part of TelepathyQt4
 *
 * @copyright Copyright (C) 2008-2010 Collabora Ltd. <http://www.collabora.co.uk/>
 * @copyright Copyright (C) 2008-2010 Nokia Corporation
 * @license LGPL 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <TelepathyQt4/ConnectionManager>
#include <TelepathyQt4/ConnectionManagerLowlevel>
#include "TelepathyQt4/connection-manager-internal.h"

#include "TelepathyQt4/_gen/cli-connection-manager-body.hpp"
#include "TelepathyQt4/_gen/cli-connection-manager.moc.hpp"
#include "TelepathyQt4/_gen/connection-manager.moc.hpp"
#include "TelepathyQt4/_gen/connection-manager-internal.moc.hpp"
#include "TelepathyQt4/_gen/connection-manager-lowlevel.moc.hpp"

#include "TelepathyQt4/debug-internal.h"

#include <TelepathyQt4/ConnectionCapabilities>
#include <TelepathyQt4/DBus>
#include <TelepathyQt4/PendingConnection>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/Constants>
#include <TelepathyQt4/ManagerFile>
#include <TelepathyQt4/Types>

#include <QDBusConnectionInterface>
#include <QQueue>
#include <QStringList>
#include <QTimer>

namespace
{

bool checkValidProtocolName(const QString &protocolName)
{
    if (!protocolName[0].isLetter()) {
        return false;
    }

    int length = protocolName.length();
    if (length <= 1) {
        return true;
    }

    QChar ch;
    for (int i = 1; i < length; ++i) {
        ch = protocolName[i];
        if (!ch.isLetterOrNumber() && ch != QLatin1Char('-')) {
            return false;
        }
    }

    return true;
}

}

namespace Tp
{

ConnectionManager::Private::PendingNames::PendingNames(const QDBusConnection &bus)
    : PendingStringList(SharedPtr<RefCounted>()),
      mBus(bus)
{
    mMethodsQueue.enqueue(QLatin1String("ListNames"));
    mMethodsQueue.enqueue(QLatin1String("ListActivatableNames"));
    QTimer::singleShot(0, this, SLOT(continueProcessing()));
}

void ConnectionManager::Private::PendingNames::onCallFinished(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QStringList> reply = *watcher;

    if (!reply.isError()) {
        parseResult(reply.value());
        continueProcessing();
    } else {
        warning() << "Failure: error " << reply.error().name() <<
            ": " << reply.error().message();
        setFinishedWithError(reply.error());
    }

    watcher->deleteLater();
}

void ConnectionManager::Private::PendingNames::continueProcessing()
{
    if (!mMethodsQueue.isEmpty()) {
        QLatin1String method = mMethodsQueue.dequeue();
        invokeMethod(method);
    }
    else {
        debug() << "Success: list" << mResult;
        setResult(mResult.toList());
        setFinished();
    }
}

void ConnectionManager::Private::PendingNames::invokeMethod(const QLatin1String &method)
{
    QDBusPendingCall call = mBus.interface()->asyncCallWithArgumentList(
                method, QList<QVariant>());
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher,
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(onCallFinished(QDBusPendingCallWatcher*)));
}

void ConnectionManager::Private::PendingNames::parseResult(const QStringList &names)
{
    foreach (const QString name, names) {
        if (name.startsWith(QLatin1String(TELEPATHY_INTERFACE_CONNECTION_MANAGER "."))) {
            mResult << name.right(name.length() - 44);
        }
    }
}

const Feature ConnectionManager::Private::ProtocolWrapper::FeatureCore =
    Feature(QLatin1String(ConnectionManager::Private::ProtocolWrapper::staticMetaObject.className()),
            0, true);

ConnectionManager::Private::ProtocolWrapper::ProtocolWrapper(
        const QDBusConnection &bus,
        const QString &busName, const QString &objectPath,
        const QString &cmName, const QString &name, const QVariantMap &props)
    : StatelessDBusProxy(bus, busName, objectPath, FeatureCore),
      OptionalInterfaceFactory<ProtocolWrapper>(this),
      mReadinessHelper(readinessHelper()),
      mInfo(ProtocolInfo(cmName, name)),
      mImmutableProps(props)
{
    fillRCCs();

    ReadinessHelper::Introspectables introspectables;

    // As Protocol does not have predefined statuses let's simulate one (0)
    ReadinessHelper::Introspectable introspectableCore(
        QSet<uint>() << 0,                                           // makesSenseForStatuses
        Features(),                                                  // dependsOnFeatures
        QStringList(),                                               // dependsOnInterfaces
        (ReadinessHelper::IntrospectFunc) &ProtocolWrapper::introspectMain,
        this);
    introspectables[FeatureCore] = introspectableCore;

    mReadinessHelper->addIntrospectables(introspectables);
}

ConnectionManager::Private::ProtocolWrapper::~ProtocolWrapper()
{
}

void ConnectionManager::Private::ProtocolWrapper::introspectMain(
        ConnectionManager::Private::ProtocolWrapper *self)
{
    Client::DBus::PropertiesInterface *properties = self->propertiesInterface();
    Q_ASSERT(properties != 0);

    if (self->receiveProperties(self->mImmutableProps)) {
        debug() << "Got everything we want from the immutable props for" <<
            self->info().name();

        if (self->hasInterface(TP_QT4_IFACE_PROTOCOL_INTERFACE_AVATARS)) {
            self->introspectQueue.enqueue(&ProtocolWrapper::introspectAvatars);
        } else {
            debug() << "Full functionality requires CM support for the Protocol.Avatars interface";
        }

        if (self->hasInterface(TP_QT4_IFACE_PROTOCOL_INTERFACE_PRESENCE)) {
            self->introspectQueue.enqueue(&ProtocolWrapper::introspectPresence);
        } else {
            debug() << "Full functionality requires CM support for the Protocol.Presence interface";
        }

        self->continueIntrospection();
        return;
    }

    debug() << "Not enough immutable properties, calling Properties::GetAll(Protocol) for" <<
        self->info().name();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(
            properties->GetAll(TP_QT4_IFACE_PROTOCOL),
            self);
    self->connect(watcher,
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(gotMainProperties(QDBusPendingCallWatcher*)));
}

void ConnectionManager::Private::ProtocolWrapper::introspectAvatars()
{
    Client::DBus::PropertiesInterface *properties = propertiesInterface();
    Q_ASSERT(properties != 0);

    debug() << "Calling Properties::GetAll(Protocol.Avatars)";
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(
            properties->GetAll(TP_QT4_IFACE_PROTOCOL_INTERFACE_AVATARS),
            this);
    connect(watcher,
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(gotAvatarsProperties(QDBusPendingCallWatcher*)));
}

void ConnectionManager::Private::ProtocolWrapper::introspectPresence()
{
    Client::DBus::PropertiesInterface *properties = propertiesInterface();
    Q_ASSERT(properties != 0);

    debug() << "Calling Properties::GetAll(Protocol.Presence)";
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(
            properties->GetAll(TP_QT4_IFACE_PROTOCOL_INTERFACE_PRESENCE),
            this);
    connect(watcher,
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(gotPresenceProperties(QDBusPendingCallWatcher*)));
}

void ConnectionManager::Private::ProtocolWrapper::continueIntrospection()
{
    if (introspectQueue.isEmpty()) {
        mReadinessHelper->setIntrospectCompleted(FeatureCore, true);
    } else {
        (this->*(introspectQueue.dequeue()))();
    }
}

void ConnectionManager::Private::ProtocolWrapper::gotMainProperties(
        QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QVariantMap> reply = *watcher;
    QVariantMap props;

    if (!reply.isError()) {
        debug() << "Got reply to Properties.GetAll(Protocol)";

        QVariantMap unqualifiedProps = reply.value();
        QVariantMap qualifiedProps;
        foreach (QString unqualified, unqualifiedProps.keys()) {
            qualifiedProps.insert(
                    QString(QLatin1String("%1.%2")).
                        arg(TP_QT4_IFACE_PROTOCOL).
                        arg(unqualified),
                    unqualifiedProps.value(unqualified));
        }
        receiveProperties(qualifiedProps);
    } else {
        warning().nospace() <<
            "Properties.GetAll(Protocol) failed: " <<
            reply.error().name() << ": " << reply.error().message();
        warning() << "  Full functionality requires CM support for the Protocol interface";
    }

    if (hasInterface(TP_QT4_IFACE_PROTOCOL_INTERFACE_AVATARS)) {
        introspectQueue.enqueue(&ProtocolWrapper::introspectAvatars);
    } else {
        debug() << "Full functionality requires CM support for the Protocol.Avatars interface";
    }

    if (hasInterface(TP_QT4_IFACE_PROTOCOL_INTERFACE_PRESENCE)) {
        introspectQueue.enqueue(&ProtocolWrapper::introspectPresence);
    } else {
        debug() << "Full functionality requires CM support for the Protocol.Presence interface";
    }

    continueIntrospection();

    watcher->deleteLater();
}

void ConnectionManager::Private::ProtocolWrapper::gotAvatarsProperties(
        QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QVariantMap> reply = *watcher;
    QVariantMap props;

    if (!reply.isError()) {
        debug() << "Got reply to Properties.GetAll(Protocol.Avatars)";
        props = reply.value();

        QStringList supportedMimeTypes = qdbus_cast<QStringList>(
                props[QLatin1String("SupportedAvatarMIMETypes")]);
        uint minHeight = qdbus_cast<uint>(props[QLatin1String("MinimumAvatarHeight")]);
        uint maxHeight = qdbus_cast<uint>(props[QLatin1String("MaximumAvatarHeight")]);
        uint recommendedHeight = qdbus_cast<uint>(
                props[QLatin1String("RecommendedAvatarHeight")]);
        uint minWidth = qdbus_cast<uint>(props[QLatin1String("MinimumAvatarWidth")]);
        uint maxWidth = qdbus_cast<uint>(props[QLatin1String("MaximumAvatarWidth")]);
        uint recommendedWidth = qdbus_cast<uint>(
                props[QLatin1String("RecommendedAvatarWidth")]);
        uint maxBytes = qdbus_cast<uint>(props[QLatin1String("MaximumAvatarBytes")]);
        mInfo.setAvatarRequirements(AvatarSpec(supportedMimeTypes,
                    minHeight, maxHeight, recommendedHeight,
                    minWidth, maxWidth, recommendedWidth,
                    maxBytes));
    } else {
        warning().nospace() <<
            "Properties.GetAll(Protocol.Avatars) failed: " <<
            reply.error().name() << ": " << reply.error().message();
        warning() << "  Full functionality requires CM support for the Protocol.Avatars interface";
    }

    continueIntrospection();

    watcher->deleteLater();
}

void ConnectionManager::Private::ProtocolWrapper::gotPresenceProperties(
        QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QVariantMap> reply = *watcher;
    QVariantMap props;

    if (!reply.isError()) {
        debug() << "Got reply to Properties.GetAll(Protocol.Presence)";
        props = reply.value();
        mInfo.setAllowedPresenceStatuses(PresenceSpecList(qdbus_cast<SimpleStatusSpecMap>(
                props[QLatin1String("Statuses")])));
    } else {
        warning().nospace() <<
            "Properties.GetAll(Protocol.Presence) failed: " <<
            reply.error().name() << ": " << reply.error().message();
        warning() << "  Full functionality requires CM support for the Protocol.Presence interface";
    }

    continueIntrospection();

    watcher->deleteLater();
}

void ConnectionManager::Private::ProtocolWrapper::fillRCCs()
{
    RequestableChannelClassList classes;

    QVariantMap fixedProps;
    QStringList allowedProps;

    // Text chatrooms
    fixedProps.insert(
            QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"),
            QLatin1String(TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT));
    fixedProps.insert(
            QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandleType"),
            static_cast<uint>(HandleTypeRoom));

    RequestableChannelClass textChatroom = {fixedProps, allowedProps};
    classes.append(textChatroom);

    // 1-1 text chats
    fixedProps[QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandleType")] =
        static_cast<uint>(HandleTypeContact);

    RequestableChannelClass text = {fixedProps, allowedProps};
    classes.append(text);

    // Media calls
    fixedProps[QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType")] =
            QLatin1String(TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA);

    RequestableChannelClass media = {fixedProps, allowedProps};
    classes.append(media);

    // Initially audio-only calls
    allowedProps.push_back(
            QLatin1String(TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA ".InitialAudio"));

    RequestableChannelClass initialAudio = {fixedProps, allowedProps};
    classes.append(initialAudio);

    // Initially AV calls
    allowedProps.push_back(
            QLatin1String(TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA ".InitialVideo"));

    RequestableChannelClass initialAV = {fixedProps, allowedProps};
    classes.append(initialAV);

    // Initially video-only calls
    allowedProps.removeAll(
            QLatin1String(TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA ".InitialAudio"));

    RequestableChannelClass initialVideo = {fixedProps, allowedProps};
    classes.append(initialVideo);

    // That also settles upgrading calls, because the media classes don't have ImmutableStreams

    mInfo.setRequestableChannelClasses(classes);
}

bool ConnectionManager::Private::ProtocolWrapper::receiveProperties(const QVariantMap &props)
{
    bool gotEverything =
        props.contains(QLatin1String(TELEPATHY_INTERFACE_PROTOCOL ".Interfaces")) &&
        props.contains(QLatin1String(TELEPATHY_INTERFACE_PROTOCOL ".Parameters")) &&
        props.contains(QLatin1String(TELEPATHY_INTERFACE_PROTOCOL ".ConnectionInterfaces")) &&
        props.contains(QLatin1String(TELEPATHY_INTERFACE_PROTOCOL ".RequestableChannelClasses")) &&
        props.contains(QLatin1String(TELEPATHY_INTERFACE_PROTOCOL ".VCardField")) &&
        props.contains(QLatin1String(TELEPATHY_INTERFACE_PROTOCOL ".EnglishName")) &&
        props.contains(QLatin1String(TELEPATHY_INTERFACE_PROTOCOL ".Icon"));

    setInterfaces(qdbus_cast<QStringList>(
                props[QLatin1String(TELEPATHY_INTERFACE_PROTOCOL ".Interfaces")]));
    mReadinessHelper->setInterfaces(interfaces());

    ParamSpecList parameters = qdbus_cast<ParamSpecList>(
            props[QLatin1String(TELEPATHY_INTERFACE_PROTOCOL ".Parameters")]);
    foreach (const ParamSpec &spec, parameters) {
        mInfo.addParameter(spec);
    }

    mInfo.setVCardField(qdbus_cast<QString>(
                props[QLatin1String(TELEPATHY_INTERFACE_PROTOCOL ".VCardField")]));
    QString englishName = qdbus_cast<QString>(
            props[QLatin1String(TELEPATHY_INTERFACE_PROTOCOL ".EnglishName")]);
    if (englishName.isEmpty()) {
        QStringList words = mInfo.name().split(QLatin1Char('-'));
        for (int i = 0; i < words.size(); ++i) {
            words[i][0] = words[i].at(0).toUpper();
        }
        englishName = words.join(QLatin1String(" "));
    }
    mInfo.setEnglishName(englishName);
    QString iconName = qdbus_cast<QString>(
            props[QLatin1String(TELEPATHY_INTERFACE_PROTOCOL ".Icon")]);
    if (iconName.isEmpty()) {
        iconName = QString(QLatin1String("im-%1")).arg(mInfo.name());
    }
    mInfo.setIconName(iconName);

    // Don't overwrite the everything-is-possible RCCs with an empty list if there is no RCCs key
    if (props.contains(QLatin1String(TELEPATHY_INTERFACE_PROTOCOL ".RequestableChannelClasses"))) {
        mInfo.setRequestableChannelClasses(qdbus_cast<RequestableChannelClassList>(
                props[QLatin1String(TELEPATHY_INTERFACE_PROTOCOL ".RequestableChannelClasses")]));
    }

    return gotEverything;
}

ConnectionManager::Private::Private(ConnectionManager *parent, const QString &name,
        const ConnectionFactoryConstPtr &connFactory,
        const ChannelFactoryConstPtr &chanFactory,
        const ContactFactoryConstPtr &contactFactory)
    : parent(parent),
      lowlevel(ConnectionManagerLowlevelPtr(new ConnectionManagerLowlevel(parent))),
      name(name),
      baseInterface(new Client::ConnectionManagerInterface(parent)),
      properties(parent->interface<Client::DBus::PropertiesInterface>()),
      readinessHelper(parent->readinessHelper()),
      connFactory(connFactory),
      chanFactory(chanFactory),
      contactFactory(contactFactory)
{
    debug() << "Creating new ConnectionManager:" << parent->busName();

    if (connFactory->dbusConnection().name() != parent->dbusConnection().name()) {
        warning() << "  The D-Bus connection in the connection factory is not the proxy connection";
    }

    if (chanFactory->dbusConnection().name() != parent->dbusConnection().name()) {
        warning() << "  The D-Bus connection in the channel factory is not the proxy connection";
    }

    ReadinessHelper::Introspectables introspectables;

    // As ConnectionManager does not have predefined statuses let's simulate one (0)
    ReadinessHelper::Introspectable introspectableCore(
        QSet<uint>() << 0,                                           // makesSenseForStatuses
        Features(),                                                  // dependsOnFeatures
        QStringList(),                                               // dependsOnInterfaces
        (ReadinessHelper::IntrospectFunc) &Private::introspectMain,
        this);
    introspectables[FeatureCore] = introspectableCore;

    readinessHelper->addIntrospectables(introspectables);
}

ConnectionManager::Private::~Private()
{
    delete baseInterface;
}

bool ConnectionManager::Private::parseConfigFile()
{
    ManagerFile f(name);
    if (!f.isValid()) {
        return false;
    }

    foreach (const QString &protocol, f.protocols()) {
        ProtocolInfo info(name, protocol);

        foreach (const ParamSpec &spec, f.parameters(protocol)) {
            info.addParameter(spec);
        }
        info.setRequestableChannelClasses(
                f.requestableChannelClasses(protocol));
        info.setVCardField(f.vcardField(protocol));
        info.setEnglishName(f.englishName(protocol));
        info.setIconName(f.iconName(protocol));
        info.setAllowedPresenceStatuses(f.allowedPresenceStatuses(protocol));
        info.setAvatarRequirements(f.avatarRequirements(protocol));

        protocols.append(info);
    }

    return true;
}

void ConnectionManager::Private::introspectMain(ConnectionManager::Private *self)
{
    if (self->parseConfigFile()) {
        self->readinessHelper->setIntrospectCompleted(FeatureCore, true);
        return;
    }

    warning() << "Error parsing config file for connection manager"
        << self->name << "- introspecting";

    debug() << "Calling Properties::GetAll(ConnectionManager)";
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(
            self->properties->GetAll(
                QLatin1String(TELEPATHY_INTERFACE_CONNECTION_MANAGER)),
            self->parent);
    self->parent->connect(watcher,
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(gotMainProperties(QDBusPendingCallWatcher*)));
}

void ConnectionManager::Private::introspectProtocolsLegacy()
{
    debug() << "Calling ConnectionManager::ListProtocols";
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(
            baseInterface->ListProtocols(), parent);
    parent->connect(watcher,
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(gotProtocolsLegacy(QDBusPendingCallWatcher*)));
}

void ConnectionManager::Private::introspectParametersLegacy()
{
    foreach (const QString &protocolName, parametersQueue) {
        debug() << "Calling ConnectionManager::GetParameters(" << protocolName << ")";
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(
                baseInterface->GetParameters(protocolName), parent);
        parent->connect(watcher,
                SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(gotParametersLegacy(QDBusPendingCallWatcher*)));
    }
}

QString ConnectionManager::Private::makeBusName(const QString &name)
{
    return QString(QLatin1String(
                TELEPATHY_CONNECTION_MANAGER_BUS_NAME_BASE)).append(name);
}

QString ConnectionManager::Private::makeObjectPath(const QString &name)
{
    return QString(QLatin1String(
                TELEPATHY_CONNECTION_MANAGER_OBJECT_PATH_BASE)).append(name);
}

/**
 * \class ConnectionManagerLowlevel
 * \ingroup clientcm
 * \headerfile TelepathyQt4/connection-manager-lowlevel.h <TelepathyQt4/ConnectionManagerLowlevel>
 *
 * \brief The ConnectionManagerLowlevel class extends ConnectionManager with
 * support to low-level features.
 */

ConnectionManagerLowlevel::ConnectionManagerLowlevel(ConnectionManager *cm)
    : mPriv(new Private(cm))
{
}

ConnectionManagerLowlevel::~ConnectionManagerLowlevel()
{
    delete mPriv;
}

bool ConnectionManagerLowlevel::isValid() const
{
    return !mPriv->cm.isNull();
}

ConnectionManagerPtr ConnectionManagerLowlevel::connectionManager() const
{
    return ConnectionManagerPtr(mPriv->cm);
}

/**
 * \class ConnectionManager
 * \ingroup clientcm
 * \headerfile TelepathyQt4/connection-manager.h <TelepathyQt4/ConnectionManager>
 *
 * \brief The ConnectionManager class represents a Telepathy connection manager.
 *
 * Connection managers allow connections to be made on one or more protocols.
 *
 * Most client applications should use this functionality via the
 * AccountManager, to allow connections to be shared between client
 * applications.
 */

/**
 * Feature representing the core that needs to become ready to make the
 * ConnectionManager object usable.
 *
 * Note that this feature must be enabled in order to use most ConnectionManager
 * methods.
 * See specific methods documentation for more details.
 *
 * When calling isReady(), becomeReady(), this feature is implicitly added
 * to the requested features.
 */
const Feature ConnectionManager::FeatureCore = Feature(QLatin1String(ConnectionManager::staticMetaObject.className()), 0, true);

/**
 * Create a new ConnectionManager object.
 *
 * The instance will use a connection factory creating Tp::Connection objects with no features
 * ready, and a channel factory creating stock Telepathy-Qt4 channel subclasses, as appropriate,
 * with no features ready.
 *
 * \param bus QDBusConnection to use.
 * \param name Name of the connection manager.
 * \return A ConnectionManagerPtr object pointing to the newly created
 *         ConnectionManager object.
 */
ConnectionManagerPtr ConnectionManager::create(const QDBusConnection &bus, const QString &name)
{
    return ConnectionManagerPtr(new ConnectionManager(QDBusConnection::sessionBus(), name,
                ConnectionFactory::create(bus), ChannelFactory::create(bus),
                ContactFactory::create()));
}

/**
 * Create a new ConnectionManager using QDBusConnection::sessionBus() and the given factories.
 *
 * The channel factory is passed to any Connection objects created by this manager
 * object. In fact, they're not used directly by ConnectionManager at all.
 *
 * A warning is printed if the factories are for a bus different from QDBusConnection::sessionBus().
 *
 * \param name Name of the connection manager.
 * \param connectionFactory The connection factory to use.
 * \param channelFactory The channel factory to use.
 * \param contactFactory The contact factory to use.
 * \return A ConnectionManagerPtr object pointing to the newly created
 *         ConnectionManager object.
 */
ConnectionManagerPtr ConnectionManager::create(const QString &name,
        const ConnectionFactoryConstPtr &connectionFactory,
        const ChannelFactoryConstPtr &channelFactory,
        const ContactFactoryConstPtr &contactFactory)
{
    return ConnectionManagerPtr(new ConnectionManager(QDBusConnection::sessionBus(), name,
                connectionFactory, channelFactory, contactFactory));
}

/**
 * Create a new ConnectionManager using the given \a bus and the given factories.
 *
 * The channel factory is passed to any Connection objects created by this manager
 * object. In fact, they're not used directly by ConnectionManager at all.
 *
 * A warning is printed if the factories are for a bus different from QDBusConnection::sessionBus().
 *
 * \param bus QDBusConnection to use.
 * \param name Name of the connection manager.
 * \param connectionFactory The connection factory to use.
 * \param channelFactory The channel factory to use.
 * \param contactFactory The contact factory to use.
 * \return A ConnectionManagerPtr object pointing to the newly created
 *         ConnectionManager object.
 */
ConnectionManagerPtr ConnectionManager::create(const QDBusConnection &bus,
        const QString &name,
        const ConnectionFactoryConstPtr &connectionFactory,
        const ChannelFactoryConstPtr &channelFactory,
        const ContactFactoryConstPtr &contactFactory)
{
    return ConnectionManagerPtr(new ConnectionManager(bus, name,
                connectionFactory, channelFactory, contactFactory));
}

/**
 * Construct a new ConnectionManager object using the given \a bus.
 *
 * \param bus QDBusConnection to use.
 * \param name Name of the connection manager.
 */
ConnectionManager::ConnectionManager(const QDBusConnection &bus,
        const QString &name,
        const ConnectionFactoryConstPtr &connectionFactory,
        const ChannelFactoryConstPtr &channelFactory,
        const ContactFactoryConstPtr &contactFactory)
    : StatelessDBusProxy(bus, Private::makeBusName(name),
            Private::makeObjectPath(name), FeatureCore),
      OptionalInterfaceFactory<ConnectionManager>(this),
      mPriv(new Private(this, name, connectionFactory, channelFactory, contactFactory))
{
}

/**
 * Class destructor.
 */
ConnectionManager::~ConnectionManager()
{
    delete mPriv;
}

/**
 * Return the short name of the connection manager (e.g. "gabble").
 *
 * \return The name of the connection manager.
 */
QString ConnectionManager::name() const
{
    return mPriv->name;
}

/**
 * Return the connection factory used by this manager.
 *
 * Only read access is provided. This allows constructing object instances and examining the object
 * construction settings, but not changing settings. Allowing changes would lead to tricky
 * situations where objects constructed at different times by the manager would have unpredictably
 * different construction settings (eg. subclass).
 *
 * \return A read-only pointer to the ConnectionFactory object.
 */
ConnectionFactoryConstPtr ConnectionManager::connectionFactory() const
{
    return mPriv->connFactory;
}

/**
 * Return the channel factory used by this manager.
 *
 * Only read access is provided. This allows constructing object instances and examining the object
 * construction settings, but not changing settings. Allowing changes would lead to tricky
 * situations where objects constructed at different times by the manager would have unpredictably
 * different construction settings (eg. subclass).
 *
 * \return A read-only pointer to the ChannelFactory object.
 */
ChannelFactoryConstPtr ConnectionManager::channelFactory() const
{
    return mPriv->chanFactory;
}

/**
 * Return the contact factory used by this manager.
 *
 * Only read access is provided. This allows constructing object instances and examining the object
 * construction settings, but not changing settings. Allowing changes would lead to tricky
 * situations where objects constructed at different times by the manager would have unpredictably
 * different construction settings (eg. subclass).
 *
 * \return A read-only pointer to the ContactFactory object.
 */
ContactFactoryConstPtr ConnectionManager::contactFactory() const
{
    return mPriv->contactFactory;
}

/**
 * Return a list of strings identifying the protocols supported by this
 * connection manager, as described in the \telepathy_spec (e.g. "jabber").
 *
 * These identifiers are not intended to be displayed to users directly; user
 * interfaces are responsible for mapping them to localized strings.
 *
 * This method requires ConnectionManager::FeatureCore to be ready.
 *
 * \return A list of supported protocols.
 */
QStringList ConnectionManager::supportedProtocols() const
{
    QStringList protocols;
    foreach (const ProtocolInfo &info, mPriv->protocols) {
        protocols.append(info.name());
    }
    return protocols;
}

/**
 * Return a list of protocols info for this connection manager.
 *
 * Note that the returned ProtocolInfoList contents should not be freed.
 *
 * This method requires ConnectionManager::FeatureCore to be ready.
 *
 * \return A list of ṔrotocolInfo.
 */
const ProtocolInfoList &ConnectionManager::protocols() const
{
    return mPriv->protocols;
}

/**
 * Return whether this connection manager implements the protocol specified by
 * \a protocolName.
 *
 * This method requires ConnectionManager::FeatureCore to be ready.
 *
 * \return \c true if the protocol is supported, \c false otherwise.
 * \sa protocol(), protocols()
 */
bool ConnectionManager::hasProtocol(const QString &protocolName) const
{
    foreach (const ProtocolInfo &info, mPriv->protocols) {
        if (info.name() == protocolName) {
            return true;
        }
    }
    return false;
}

/**
 * Return the ProtocolInfo object for the protocol specified by
 * \a protocolName.
 *
 * This method requires ConnectionManager::FeatureCore to be ready.
 *
 * \param protocolName The name of the protocol.
 * \return A ProtocolInfo object which will return \c for ProtocolInfo::isValid() if the protocol
 *         specified by \a protocolName is not supported.
 * \sa hasProtocol()
 */
ProtocolInfo ConnectionManager::protocol(const QString &protocolName) const
{
    foreach (const ProtocolInfo &info, mPriv->protocols) {
        if (info.name() == protocolName) {
            return info;
        }
    }
    return ProtocolInfo();
}

/**
 * Request a Connection object representing a given account on a given protocol
 * with the given parameters.
 *
 * Return a pending operation representing the Connection object which will
 * succeed when the connection has been created or fail if an error occurred.
 *
 * \param protocol Name of the protocol to create the account for.
 * \param parameters Account parameters.
 * \return A PendingOperation which will emit PendingConnection::finished when
 *         the account has been created of failed its creation process.
 */
PendingConnection *ConnectionManagerLowlevel::requestConnection(const QString &protocol,
        const QVariantMap &parameters)
{
    if (!isValid()) {
        return new PendingConnection(TP_QT4_ERROR_NOT_AVAILABLE,
                QLatin1String("The connection manager has been destroyed already"));
    }

    return new PendingConnection(ConnectionManagerPtr(mPriv->cm),
            protocol, parameters);
}

/**
 * Return a pending operation from which a list of all installed connection
 * manager short names (such as "gabble" or "haze") can be retrieved if it
 * succeeds.
 *
 * \return A PendingStringList which will emit PendingStringList::finished
 *         when this object has finished or failed getting the connection
 *         manager names.
 */
PendingStringList *ConnectionManager::listNames(const QDBusConnection &bus)
{
    return new ConnectionManager::Private::PendingNames(bus);
}

ConnectionManagerLowlevelPtr ConnectionManager::lowlevel()
{
    return mPriv->lowlevel;
}

ConnectionManagerLowlevelConstPtr ConnectionManager::lowlevel() const
{
    return mPriv->lowlevel;
}

/**
 * Return the Client::ConnectionManagerInterface for this ConnectionManager.
 * This method is protected since the convenience methods provided by this
 * class should generally be used instead of calling D-Bus methods
 * directly.
 *
 * \return A pointer to the existing Client::ConnectionManagerInterface for this
 *         ConnectionManager object.
 */
Client::ConnectionManagerInterface *ConnectionManager::baseInterface() const
{
    return mPriv->baseInterface;
}

/**** Private ****/
void ConnectionManager::gotMainProperties(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QVariantMap> reply = *watcher;
    QVariantMap props;

    if (!reply.isError()) {
        debug() << "Got reply to Properties.GetAll(ConnectionManager)";
        props = reply.value();

        // If Interfaces is not supported, the spec says to assume it's
        // empty, so keep the empty list mPriv was initialized with
        if (props.contains(QLatin1String("Interfaces"))) {
            setInterfaces(qdbus_cast<QStringList>(props[QLatin1String("Interfaces")]));
            mPriv->readinessHelper->setInterfaces(interfaces());
        }
    } else {
        warning().nospace() <<
            "Properties.GetAll(ConnectionManager) failed: " <<
            reply.error().name() << ": " << reply.error().message();

        mPriv->readinessHelper->setIntrospectCompleted(FeatureCore, false, reply.error());
        watcher->deleteLater();
        return;
    }

    ProtocolPropertiesMap protocolsMap =
        qdbus_cast<ProtocolPropertiesMap>(props[QLatin1String("Protocols")]);
    if (!protocolsMap.isEmpty()) {
        ProtocolPropertiesMap::const_iterator i = protocolsMap.constBegin();
        ProtocolPropertiesMap::const_iterator end = protocolsMap.constEnd();
        while (i != end) {
            QString protocolName = i.key();
            if (!checkValidProtocolName(protocolName)) {
                warning() << "Protocol has an invalid name" << protocolName << "- ignoring";
                continue;
            }

            QString escapedProtocolName = protocolName;
            escapedProtocolName.replace(QLatin1Char('-'), QLatin1Char('_'));
            QString protocolPath = QString(
                    QLatin1String("%1/%2")).arg(objectPath()).arg(escapedProtocolName);
            SharedPtr<Private::ProtocolWrapper> wrapper = SharedPtr<Private::ProtocolWrapper>(
                    new Private::ProtocolWrapper(
                        dbusConnection(), busName(), protocolPath,
                        mPriv->name, protocolName, i.value()));
            connect(wrapper->becomeReady(),
                    SIGNAL(finished(Tp::PendingOperation*)),
                    SLOT(onProtocolReady(Tp::PendingOperation*)));
            mPriv->wrappers.insert(wrapper);
            ++i;
        }
    } else {
        mPriv->introspectProtocolsLegacy();
    }

    watcher->deleteLater();
}

void ConnectionManager::gotProtocolsLegacy(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QStringList> reply = *watcher;
    QStringList protocolsNames;

    if (!reply.isError()) {
        debug() << "Got reply to ConnectionManager.ListProtocols";
        protocolsNames = reply.value();

        foreach (const QString &protocolName, protocolsNames) {
            mPriv->protocols.append(ProtocolInfo(mPriv->name, protocolName));
            mPriv->parametersQueue.enqueue(protocolName);
        }

        mPriv->introspectParametersLegacy();
    } else {
        mPriv->readinessHelper->setIntrospectCompleted(FeatureCore, false, reply.error());

        warning().nospace() <<
            "ConnectionManager.ListProtocols failed: " <<
            reply.error().name() << ": " << reply.error().message();

        // FIXME shouldn't this invalidate the CM?
    }

    watcher->deleteLater();
}

void ConnectionManager::gotParametersLegacy(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<ParamSpecList> reply = *watcher;
    QString protocolName = mPriv->parametersQueue.dequeue();
    bool found = false;
    int pos = 0;
    foreach (const ProtocolInfo &info, mPriv->protocols) {
        if (info.name() == protocolName) {
            found = true;
            break;
        }
        ++pos;
    }
    Q_ASSERT(found);

    if (!reply.isError()) {
        debug() << QString(QLatin1String("Got reply to ConnectionManager.GetParameters(%1)")).arg(protocolName);
        ParamSpecList parameters = reply.value();
        ProtocolInfo &info = mPriv->protocols[pos];
        foreach (const ParamSpec &spec, parameters) {
            debug() << "Parameter" << spec.name << "has flags" << spec.flags
                << "and signature" << spec.signature;

            info.addParameter(spec);
        }
    } else {
        // let's remove this protocol as we can't get the params
        mPriv->protocols.removeAt(pos);

        warning().nospace() <<
            QString(QLatin1String("ConnectionManager.GetParameters(%1) failed: ")).arg(protocolName) <<
            reply.error().name() << ": " << reply.error().message();
    }

    if (mPriv->parametersQueue.isEmpty()) {
        if (!mPriv->protocols.isEmpty()) {
            mPriv->readinessHelper->setIntrospectCompleted(FeatureCore, true);
        } else {
            // we could not retrieve the params for any protocol, fail core.
            mPriv->readinessHelper->setIntrospectCompleted(FeatureCore, false, reply.error());
        }
    }

    watcher->deleteLater();
}

void ConnectionManager::onProtocolReady(Tp::PendingOperation *op)
{
    PendingReady *pr = qobject_cast<PendingReady*>(op);
    SharedPtr<Private::ProtocolWrapper> wrapper =
        SharedPtr<Private::ProtocolWrapper>::qObjectCast(pr->proxy());
    ProtocolInfo info = wrapper->info();

    mPriv->wrappers.remove(wrapper);

    if (!op->isError()) {
        mPriv->protocols.append(info);
    } else {
        warning().nospace() << "Protocol(" << info.name() << ")::becomeReady "
            "failed: " << op->errorName() << ": " << op->errorMessage();
    }

    if (mPriv->wrappers.isEmpty()) {
        if (!mPriv->protocols.isEmpty()) {
            mPriv->readinessHelper->setIntrospectCompleted(FeatureCore, true);
        } else {
            // we could not make any Protocol objects ready, fail core.
            mPriv->readinessHelper->setIntrospectCompleted(FeatureCore, false,
                    op->errorName(), op->errorMessage());
        }
    }
}

} // Tp
