/**
 * This file is part of TelepathyQt4
 *
 * @copyright Copyright (C) 2010-2011 Collabora Ltd. <http://www.collabora.co.uk/>
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

#ifndef _TelepathyQt4_outgoing_stream_tube_channel_h_HEADER_GUARD_
#define _TelepathyQt4_outgoing_stream_tube_channel_h_HEADER_GUARD_

#ifndef IN_TELEPATHY_QT4_HEADER
#error IN_TELEPATHY_QT4_HEADER
#endif

#include <TelepathyQt4/StreamTubeChannel>
#include <TelepathyQt4/PendingOperation>

class QHostAddress;
class QTcpServer;
class QLocalServer;

namespace Tp
{

class TELEPATHY_QT4_EXPORT OutgoingStreamTubeChannel : public StreamTubeChannel
{
    Q_OBJECT
    Q_DISABLE_COPY(OutgoingStreamTubeChannel)

public:
    static const Feature FeatureCore;

    static OutgoingStreamTubeChannelPtr create(const ConnectionPtr &connection,
            const QString &objectPath, const QVariantMap &immutableProperties);

    virtual ~OutgoingStreamTubeChannel();

    PendingOperation *offerTcpSocket(const QHostAddress &address, quint16 port,
            const QVariantMap &parameters = QVariantMap());
    PendingOperation *offerTcpSocket(const QTcpServer *server,
            const QVariantMap &parameters = QVariantMap());

    PendingOperation *offerUnixSocket(const QString &socketAddress,
            const QVariantMap &parameters = QVariantMap(), bool requireCredentials = false);
    PendingOperation *offerUnixSocket(const QLocalServer *server,
            const QVariantMap &parameters = QVariantMap(), bool requireCredentials = false);

    QHash<uint, Tp::ContactPtr> contactsForConnections() const;

    QHash<QPair<QHostAddress,quint16>, uint> connectionsForSourceAddresses() const;
    QHash<uchar, uint> connectionsForCredentials() const;

protected:
    OutgoingStreamTubeChannel(const ConnectionPtr &connection, const QString &objectPath,
            const QVariantMap &immutableProperties,
            const Feature &coreFeature = OutgoingStreamTubeChannel::FeatureCore);

private Q_SLOTS:
    TELEPATHY_QT4_NO_EXPORT void onNewRemoteConnection(uint contactId,
            const QDBusVariant &parameter, uint connectionId);
    TELEPATHY_QT4_NO_EXPORT void onContactsRetrieved(const QUuid &uuid,
            const QList<Tp::ContactPtr> &contacts);
    TELEPATHY_QT4_NO_EXPORT void onConnectionClosed(uint connectionId,
            const QString &errorName, const QString &errorMessage);

private:
    struct Private;
    friend struct PendingOpenTube;
    friend struct Private;
    Private *mPriv;
};

}

#endif
