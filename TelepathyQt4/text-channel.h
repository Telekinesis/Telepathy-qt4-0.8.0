/**
 * This file is part of TelepathyQt4
 *
 * @copyright Copyright (C) 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 * @copyright Copyright (C) 2009 Nokia Corporation
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

#ifndef _TelepathyQt4_text_channel_h_HEADER_GUARD_
#define _TelepathyQt4_text_channel_h_HEADER_GUARD_

#ifndef IN_TELEPATHY_QT4_HEADER
#error IN_TELEPATHY_QT4_HEADER
#endif

#include <TelepathyQt4/Channel>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingSendMessage>

namespace Tp
{

class Message;
class ReceivedMessage;

class TELEPATHY_QT4_EXPORT TextChannel : public Channel
{
    Q_OBJECT
    Q_DISABLE_COPY(TextChannel)

public:
    static const Feature FeatureCore;
    static const Feature FeatureMessageQueue;
    static const Feature FeatureMessageCapabilities;
    static const Feature FeatureMessageSentSignal;
    static const Feature FeatureChatState;

    static TextChannelPtr create(const ConnectionPtr &connection,
            const QString &objectPath, const QVariantMap &immutableProperties);

    virtual ~TextChannel();

    bool hasMessagesInterface() const;
    bool hasChatStateInterface() const;
    bool canInviteContacts() const;

    // requires FeatureMessageCapabilities
    QStringList supportedContentTypes() const;
    MessagePartSupportFlags messagePartSupport() const;
    DeliveryReportingSupportFlags deliveryReportingSupport() const;

    // requires FeatureMessageQueue
    QList<ReceivedMessage> messageQueue() const;

    // requires FeatureChatState
    ChannelChatState chatState(const ContactPtr &contact) const;

public Q_SLOTS:
    void acknowledge(const QList<ReceivedMessage> &messages);

    void forget(const QList<ReceivedMessage> &messages);

    PendingSendMessage *send(const QString &text,
            ChannelTextMessageType type = ChannelTextMessageTypeNormal,
            MessageSendingFlags flags = 0);

    PendingSendMessage *send(const MessagePartList &parts,
            MessageSendingFlags flags = 0);

    inline PendingOperation *inviteContacts(
            const QList<ContactPtr> &contacts,
            const QString &message = QString())
    {
        return groupAddContacts(contacts, message);
    }

    PendingOperation *requestChatState(ChannelChatState state);

Q_SIGNALS:
    // FeatureMessageSentSignal
    void messageSent(const Tp::Message &message,
            Tp::MessageSendingFlags flags,
            const QString &sentMessageToken);

    // FeatureMessageQueue
    void messageReceived(const Tp::ReceivedMessage &message);
    void pendingMessageRemoved(
            const Tp::ReceivedMessage &message);

    // FeatureChatState
    void chatStateChanged(const Tp::ContactPtr &contact,
            Tp::ChannelChatState state);

protected:
    TextChannel(const ConnectionPtr &connection, const QString &objectPath,
            const QVariantMap &immutableProperties,
            const Feature &coreFeature = TextChannel::FeatureCore);

private Q_SLOTS:
    TELEPATHY_QT4_NO_EXPORT void onContactsFinished(Tp::PendingOperation *);
    TELEPATHY_QT4_NO_EXPORT void onAcknowledgePendingMessagesReply(QDBusPendingCallWatcher *);

    TELEPATHY_QT4_NO_EXPORT void onMessageSent(const Tp::MessagePartList &, uint,
            const QString &);
    TELEPATHY_QT4_NO_EXPORT void onMessageReceived(const Tp::MessagePartList &);
    TELEPATHY_QT4_NO_EXPORT void onPendingMessagesRemoved(const Tp::UIntList &);

    TELEPATHY_QT4_NO_EXPORT void onTextSent(uint, uint, const QString &);
    TELEPATHY_QT4_NO_EXPORT void onTextReceived(uint, uint, uint, uint, uint, const QString &);
    TELEPATHY_QT4_NO_EXPORT void onTextSendError(uint, uint, uint, const QString &);

    TELEPATHY_QT4_NO_EXPORT void gotProperties(QDBusPendingCallWatcher *);
    TELEPATHY_QT4_NO_EXPORT void gotPendingMessages(QDBusPendingCallWatcher *);

    TELEPATHY_QT4_NO_EXPORT void onChatStateChanged(uint, uint);

private:
    struct Private;
    friend struct Private;
    Private *mPriv;
};

} // Tp

#endif
