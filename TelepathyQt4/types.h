/**
 * This file is part of TelepathyQt4
 *
 * @copyright Copyright (C) 2008 Collabora Ltd. <http://www.collabora.co.uk/>
 * @copyright Copyright (C) 2008 Nokia Corporation
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

#ifndef _TelepathyQt4_types_h_HEADER_GUARD_
#define _TelepathyQt4_types_h_HEADER_GUARD_

#ifndef IN_TELEPATHY_QT4_HEADER
#error IN_TELEPATHY_QT4_HEADER
#endif

#include <TelepathyQt4/_gen/types.h>

#include <TelepathyQt4/Global>
#include <TelepathyQt4/SharedPtr>
#include <TelepathyQt4/MethodInvocationContext>

#include <QDBusVariant>

namespace Tp
{

TELEPATHY_QT4_EXPORT void registerTypes();

template <typename T> class Filter;
template <typename T> class GenericCapabilityFilter;
template <typename T> class GenericPropertyFilter;

class AbstractClient;
class AbstractClientApprover;
class AbstractClientHandler;
class AbstractClientObserver;
class Account;
typedef GenericCapabilityFilter<Account> AccountCapabilityFilter;
class AccountFactory;
typedef Filter<Account> AccountFilter;
class AccountManager;
class AccountPropertyFilter;
class AccountSet;
class Channel;
class ChannelDispatchOperation;
class ChannelFactory;
class ChannelRequest;
class ClientObject;
class ClientRegistrar;
class Connection;
class ConnectionFactory;
class ConnectionLowlevel;
class ConnectionManager;
class ConnectionManagerLowlevel;
class Contact;
class ContactFactory;
class ContactManager;
class ContactMessenger;
class ContactSearchChannel;
class DBusProxy;
class FileTransferChannel;
class IncomingFileTransferChannel;
class IncomingStreamTubeChannel;
class OutgoingFileTransferChannel;
class OutgoingStreamTubeChannel;
class Profile;
class ProfileManager;
class RoomListChannel;
class SimpleObserver;
class SimpleCallObserver;
class SimpleTextObserver;
class StreamedMediaChannel;
class StreamedMediaStream;
class StreamTubeChannel;
class StreamTubeClient;
class StreamTubeServer;
class TextChannel;
class TubeChannel;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

typedef SharedPtr<AbstractClient> AbstractClientPtr;
typedef SharedPtr<AbstractClientApprover> AbstractClientApproverPtr;
typedef SharedPtr<AbstractClientHandler> AbstractClientHandlerPtr;
typedef SharedPtr<AbstractClientObserver> AbstractClientObserverPtr;
typedef SharedPtr<Account> AccountPtr;
typedef SharedPtr<AccountCapabilityFilter> AccountCapabilityFilterPtr;
typedef SharedPtr<const AccountCapabilityFilter> AccountCapabilityFilterConstPtr;
typedef SharedPtr<AccountFactory> AccountFactoryPtr;
typedef SharedPtr<const AccountFactory> AccountFactoryConstPtr;
typedef SharedPtr<AccountFilter> AccountFilterPtr;
typedef SharedPtr<const AccountFilter> AccountFilterConstPtr;
typedef SharedPtr<AccountManager> AccountManagerPtr;
typedef SharedPtr<AccountPropertyFilter> AccountPropertyFilterPtr;
typedef SharedPtr<const AccountPropertyFilter> AccountPropertyFilterConstPtr;
typedef SharedPtr<AccountSet> AccountSetPtr;
typedef SharedPtr<Channel> ChannelPtr;
typedef SharedPtr<ChannelDispatchOperation> ChannelDispatchOperationPtr;
typedef SharedPtr<ChannelFactory> ChannelFactoryPtr;
typedef SharedPtr<const ChannelFactory> ChannelFactoryConstPtr;
typedef SharedPtr<ChannelRequest> ChannelRequestPtr;
typedef SharedPtr<ClientObject> ClientObjectPtr;
typedef SharedPtr<ClientRegistrar> ClientRegistrarPtr;
typedef SharedPtr<Connection> ConnectionPtr;
typedef SharedPtr<ConnectionFactory> ConnectionFactoryPtr;
typedef SharedPtr<const ConnectionFactory> ConnectionFactoryConstPtr;
typedef SharedPtr<ConnectionLowlevel> ConnectionLowlevelPtr;
typedef SharedPtr<const ConnectionLowlevel> ConnectionLowlevelConstPtr;
typedef SharedPtr<ConnectionManager> ConnectionManagerPtr;
typedef SharedPtr<ConnectionManagerLowlevel> ConnectionManagerLowlevelPtr;
typedef SharedPtr<const ConnectionManagerLowlevel> ConnectionManagerLowlevelConstPtr;
typedef SharedPtr<Contact> ContactPtr;
typedef QSet<ContactPtr> Contacts;
typedef SharedPtr<ContactFactory> ContactFactoryPtr;
typedef SharedPtr<const ContactFactory> ContactFactoryConstPtr;
typedef SharedPtr<ContactManager> ContactManagerPtr;
typedef SharedPtr<ContactMessenger> ContactMessengerPtr;
typedef SharedPtr<ContactSearchChannel> ContactSearchChannelPtr;
typedef SharedPtr<DBusProxy> DBusProxyPtr;
typedef SharedPtr<FileTransferChannel> FileTransferChannelPtr;
typedef SharedPtr<IncomingFileTransferChannel> IncomingFileTransferChannelPtr;
typedef SharedPtr<IncomingStreamTubeChannel> IncomingStreamTubeChannelPtr;
typedef SharedPtr<OutgoingFileTransferChannel> OutgoingFileTransferChannelPtr;
typedef SharedPtr<OutgoingStreamTubeChannel> OutgoingStreamTubeChannelPtr;
typedef SharedPtr<Profile> ProfilePtr;
typedef SharedPtr<ProfileManager> ProfileManagerPtr;
typedef SharedPtr<RoomListChannel> RoomListChannelPtr;
typedef SharedPtr<SimpleObserver> SimpleObserverPtr;
typedef SharedPtr<SimpleCallObserver> SimpleCallObserverPtr;
typedef SharedPtr<SimpleTextObserver> SimpleTextObserverPtr;
typedef SharedPtr<StreamedMediaChannel> StreamedMediaChannelPtr;
typedef SharedPtr<StreamedMediaStream> StreamedMediaStreamPtr;
typedef SharedPtr<StreamTubeChannel> StreamTubeChannelPtr;
typedef SharedPtr<StreamTubeClient> StreamTubeClientPtr;
typedef SharedPtr<StreamTubeServer> StreamTubeServerPtr;
typedef SharedPtr<TextChannel> TextChannelPtr;
typedef SharedPtr<TubeChannel> TubeChannelPtr;

template<typename T1 = MethodInvocationContextTypes::Nil, typename T2 = MethodInvocationContextTypes::Nil,
         typename T3 = MethodInvocationContextTypes::Nil, typename T4 = MethodInvocationContextTypes::Nil,
         typename T5 = MethodInvocationContextTypes::Nil, typename T6 = MethodInvocationContextTypes::Nil,
         typename T7 = MethodInvocationContextTypes::Nil, typename T8 = MethodInvocationContextTypes::Nil>
class MethodInvocationContextPtr : public SharedPtr<MethodInvocationContext<T1, T2, T3, T4, T5, T6, T7, T8> >
{
public:
    inline MethodInvocationContextPtr() { }
    explicit inline MethodInvocationContextPtr(MethodInvocationContext<T1, T2, T3, T4, T5, T6, T7, T8> *d)
        : SharedPtr<MethodInvocationContext<T1, T2, T3, T4, T5, T6, T7, T8> >(d) { }
    inline MethodInvocationContextPtr(const SharedPtr<MethodInvocationContext<T1, T2, T3, T4, T5, T6, T7, T8> > &o)
        : SharedPtr<MethodInvocationContext<T1, T2, T3, T4, T5, T6, T7, T8> >(o) { }
};

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

} // Tp

#endif
