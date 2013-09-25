/**
 * This file is part of TelepathyQt4
 *
 * @copyright Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
 * @copyright Copyright (C) 2010 Nokia Corporation
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

#include <TelepathyQt4/PendingContactInfo>

#include "TelepathyQt4/_gen/pending-contact-info.moc.hpp"

#include "TelepathyQt4/debug-internal.h"

#include <TelepathyQt4/Connection>
#include <TelepathyQt4/ContactManager>

#include <QDBusPendingReply>

namespace Tp
{

struct TELEPATHY_QT4_NO_EXPORT PendingContactInfo::Private
{
    Contact::InfoFields info;
};

/**
 * \class PendingContactInfo
 * \ingroup clientconn
 * \headerfile TelepathyQt4/pending-contact-info.h <TelepathyQt4/PendingContactInfo>
 *
 * \brief The PendingContactInfo class represents the parameters of and the
 * reply to an asynchronous contact info request.
 *
 * Instances of this class cannot be constructed directly; the only way to get
 * one is via Contact.
 *
 * See \ref async_model
 */

/**
 * Construct a new PendingContactInfo object.
 *
 * \param contact Contact to use.
 */
PendingContactInfo::PendingContactInfo(const ContactPtr &contact)
    : PendingOperation(contact),
      mPriv(new Private)
{
    ConnectionPtr connection = contact->manager()->connection();
    Client::ConnectionInterfaceContactInfoInterface *contactInfoInterface =
        connection->interface<Client::ConnectionInterfaceContactInfoInterface>();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(
            contactInfoInterface->RequestContactInfo(
                contact->handle()[0]), this);
    connect(watcher,
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(onCallFinished(QDBusPendingCallWatcher*)));
}

/**
 * Class destructor.
 */
PendingContactInfo::~PendingContactInfo()
{
    delete mPriv;
}

/**
 * Return the contact through which the request was made.
 *
 * \return A pointer to the Contact object.
 */
ContactPtr PendingContactInfo::contact() const
{
    return ContactPtr(qobject_cast<Contact*>((Contact*) _object().data()));
}

/**
 * Return the information for contact().
 *
 * \return The contact infor as a Contact::InfoFields object.
 */
Contact::InfoFields PendingContactInfo::infoFields() const
{
    if (!isFinished()) {
        warning() << "PendingContactInfo::info called before finished";
    } else if (!isValid()) {
        warning() << "PendingContactInfo::info called when not valid";
    }

    return mPriv->info;
}

void PendingContactInfo::onCallFinished(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<Tp::ContactInfoFieldList> reply = *watcher;

    if (!reply.isError()) {
        mPriv->info = Contact::InfoFields(reply.value());
        debug() << "Got reply to ContactInfo.RequestContactInfo";
        setFinished();
    } else {
        debug().nospace() <<
            "ContactInfo.RequestContactInfo failed: " <<
            reply.error().name() << ": " << reply.error().message();
        setFinishedWithError(reply.error());
    }

    watcher->deleteLater();
}

} // Tp
