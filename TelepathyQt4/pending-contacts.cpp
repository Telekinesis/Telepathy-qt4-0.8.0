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

#include <TelepathyQt4/PendingContacts>
#include "TelepathyQt4/_gen/pending-contacts.moc.hpp"

#include <TelepathyQt4/Connection>
#include <TelepathyQt4/ConnectionLowlevel>
#include <TelepathyQt4/ContactManager>
#include <TelepathyQt4/ContactFactory>
#include <TelepathyQt4/PendingContactAttributes>
#include <TelepathyQt4/PendingHandles>
#include <TelepathyQt4/ReferencedHandles>

#include "TelepathyQt4/debug-internal.h"

namespace Tp
{

struct TELEPATHY_QT4_NO_EXPORT PendingContacts::Private
{
    enum RequestType
    {
        ForHandles,
        ForIdentifiers,
        Upgrade
    };

    Private(PendingContacts *parent, const ContactManagerPtr &manager, const UIntList &handles,
            const Features &features, const Features &missingFeatures,
            const QMap<uint, ContactPtr> &satisfyingContacts)
        : parent(parent),
          manager(manager),
          features(features),
          missingFeatures(missingFeatures),
          satisfyingContacts(satisfyingContacts),
          requestType(ForHandles),
          handles(handles),
          nested(0)
    {
    }

    Private(PendingContacts *parent, const ContactManagerPtr &manager, const QStringList &identifiers,
            const Features &features)
        : parent(parent),
          manager(manager),
          features(features),
          requestType(ForIdentifiers),
          identifiers(identifiers),
          nested(0)
    {
    }

    Private(PendingContacts *parent,
            const ContactManagerPtr &manager, const QList<ContactPtr> &contactsToUpgrade,
            const Features &features)
        : parent(parent),
          manager(manager),
          features(features),
          requestType(Upgrade),
          contactsToUpgrade(contactsToUpgrade),
          nested(0)
    {
    }

    void setFinished();

    // Public object
    PendingContacts *parent;

    // Generic parameters
    ContactManagerPtr manager;
    Features features;
    Features missingFeatures;
    QMap<uint, ContactPtr> satisfyingContacts;

    // Request type specific parameters
    RequestType requestType;
    UIntList handles;
    QStringList identifiers;
    QList<ContactPtr> contactsToUpgrade;
    PendingContacts *nested;

    // Results
    QList<ContactPtr> contacts;
    UIntList invalidHandles;
    QStringList validIds;
    QHash<QString, QPair<QString, QString> > invalidIds;

    ReferencedHandles handlesToInspect;
};

void PendingContacts::Private::setFinished()
{
    ConnectionLowlevelPtr connLowlevel = manager->connection()->lowlevel();
    UIntList handles = invalidHandles;
    foreach (uint handle, handles) {
        if (connLowlevel->hasContactId(handle)) {
            satisfyingContacts.insert(handle, manager->ensureContact(handle,
                        connLowlevel->contactId(handle), missingFeatures));
            invalidHandles.removeOne(handle);
        }
    }

    parent->setFinished();
}

/**
 * \class PendingContacts
 * \ingroup clientconn
 * \headerfile TelepathyQt4/pending-contacts.h <TelepathyQt4/PendingContacts>
 *
 * \brief The PendingContacts class is used by ContactManager when
 * creating/updating Contact objects.
 *
 * See \ref async_model
 */

PendingContacts::PendingContacts(const ContactManagerPtr &manager,
        const UIntList &handles,
        const Features &features,
        const Features &missingFeatures,
        const QStringList &interfaces,
        const QMap<uint, ContactPtr> &satisfyingContacts,
        const QSet<uint> &otherContacts,
        const QString &errorName,
        const QString &errorMessage)
    : PendingOperation(manager->connection()),
      mPriv(new Private(this, manager, handles, features, missingFeatures, satisfyingContacts))
{
    if (!errorName.isEmpty()) {
        setFinishedWithError(errorName, errorMessage);
        return;
    }

    if (!otherContacts.isEmpty()) {
        ConnectionPtr conn = manager->connection();
        if (conn->interfaces().contains(QLatin1String(TELEPATHY_INTERFACE_CONNECTION_INTERFACE_CONTACTS))) {
            PendingContactAttributes *attributes =
                conn->lowlevel()->contactAttributes(otherContacts.toList(),
                        interfaces, true);

            connect(attributes,
                    SIGNAL(finished(Tp::PendingOperation*)),
                    SLOT(onAttributesFinished(Tp::PendingOperation*)));
        } else {
            // fallback to just create the contacts
            PendingHandles *handles = conn->lowlevel()->referenceHandles(HandleTypeContact,
                    otherContacts.toList());
            connect(handles,
                    SIGNAL(finished(Tp::PendingOperation*)),
                    SLOT(onReferenceHandlesFinished(Tp::PendingOperation*)));
        }
    } else {
        allAttributesFetched();
    }
}

PendingContacts::PendingContacts(const ContactManagerPtr &manager,
        const QStringList &identifiers, const Features &features,
        const QString &errorName, const QString &errorMessage)
    : PendingOperation(manager->connection()),
      mPriv(new Private(this, manager, identifiers, features))
{
    if (!errorName.isEmpty()) {
        setFinishedWithError(errorName, errorMessage);
        return;
    }

    ConnectionPtr conn = manager->connection();
    PendingHandles *handles = conn->lowlevel()->requestHandles(HandleTypeContact, identifiers);

    connect(handles,
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onRequestHandlesFinished(Tp::PendingOperation*)));
}

PendingContacts::PendingContacts(const ContactManagerPtr &manager,
        const QList<ContactPtr> &contacts, const Features &features,
        const QString &errorName, const QString &errorMessage)
    : PendingOperation(manager->connection()),
      mPriv(new Private(this, manager, contacts, features))
{
    if (!errorName.isEmpty()) {
        setFinishedWithError(errorName, errorMessage);
        return;
    }

    UIntList handles;
    foreach (const ContactPtr &contact, contacts) {
        handles.push_back(contact->handle()[0]);
    }

    mPriv->nested = manager->contactsForHandles(handles, features);
    connect(mPriv->nested,
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onNestedFinished(Tp::PendingOperation*)));
}

/**
 * Class destructor.
 */
PendingContacts::~PendingContacts()
{
    delete mPriv;
}

ContactManagerPtr PendingContacts::manager() const
{
    return mPriv->manager;
}

Features PendingContacts::features() const
{
    return mPriv->features;
}

bool PendingContacts::isForHandles() const
{
    return mPriv->requestType == Private::ForHandles;
}

UIntList PendingContacts::handles() const
{
    if (!isForHandles()) {
        warning() << "Tried to get handles from" << this << "which is not for handles!";
    }

    return mPriv->handles;
}

bool PendingContacts::isForIdentifiers() const
{
    return mPriv->requestType == Private::ForIdentifiers;
}

QStringList PendingContacts::identifiers() const
{
    if (!isForIdentifiers()) {
        warning() << "Tried to get identifiers from" << this << "which is not for identifiers!";
    }

    return mPriv->identifiers;
}

bool PendingContacts::isUpgrade() const
{
    return mPriv->requestType == Private::Upgrade;
}

QList<ContactPtr> PendingContacts::contactsToUpgrade() const
{
    if (!isUpgrade()) {
        warning() << "Tried to get contacts to upgrade from" << this << "which is not an upgrade!";
    }

    return mPriv->contactsToUpgrade;
}

QList<ContactPtr> PendingContacts::contacts() const
{
    if (!isFinished()) {
        warning() << "PendingContacts::contacts() called before finished";
    } else if (isError()) {
        warning() << "PendingContacts::contacts() called when errored";
    }

    return mPriv->contacts;
}

UIntList PendingContacts::invalidHandles() const
{
    if (!isFinished()) {
        warning() << "PendingContacts::invalidHandles() called before finished";
    } else if (isError()) {
        warning() << "PendingContacts::invalidHandles() called when errored";
    } else if (!isForHandles()) {
        warning() << "PendingContacts::invalidHandles() called for" << this << "which is for IDs!";
    }

    return mPriv->invalidHandles;
}

QStringList PendingContacts::validIdentifiers() const
{
    if (!isFinished()) {
        warning() << "PendingContacts::validIdentifiers called before finished";
    } else if (!isValid()) {
        warning() << "PendingContacts::validIdentifiers called when not valid";
    }

    return mPriv->validIds;
}

QHash<QString, QPair<QString, QString> > PendingContacts::invalidIdentifiers() const
{
    if (!isFinished()) {
        warning() << "PendingContacts::invalidIdentifiers called before finished";
    }

    return mPriv->invalidIds;
}

void PendingContacts::onAttributesFinished(PendingOperation *operation)
{
    PendingContactAttributes *pendingAttributes =
        qobject_cast<PendingContactAttributes *>(operation);

    if (pendingAttributes->isError()) {
        debug() << "PendingAttrs error" << pendingAttributes->errorName()
                << "message" << pendingAttributes->errorMessage();
        setFinishedWithError(pendingAttributes->errorName(), pendingAttributes->errorMessage());
        return;
    }

    ReferencedHandles validHandles = pendingAttributes->validHandles();
    ContactAttributesMap attributes = pendingAttributes->attributes();

    foreach (uint handle, mPriv->handles) {
        if (!mPriv->satisfyingContacts.contains(handle)) {
            int indexInValid = validHandles.indexOf(handle);
            if (indexInValid >= 0) {
                ReferencedHandles referencedHandle = validHandles.mid(indexInValid, 1);
                QVariantMap handleAttributes = attributes[handle];
                mPriv->satisfyingContacts.insert(handle, manager()->ensureContact(referencedHandle,
                            mPriv->missingFeatures, handleAttributes));
            } else {
                mPriv->invalidHandles.push_back(handle);
            }
        }
    }

    allAttributesFetched();
}

void PendingContacts::onRequestHandlesFinished(PendingOperation *operation)
{
    PendingHandles *pendingHandles = qobject_cast<PendingHandles *>(operation);

    mPriv->validIds = pendingHandles->validNames();
    mPriv->invalidIds = pendingHandles->invalidNames();

    if (pendingHandles->isError()) {
        debug() << "RequestHandles error" << operation->errorName()
                << "message" << operation->errorMessage();
        setFinishedWithError(operation->errorName(), operation->errorMessage());
        return;
    }

    mPriv->nested = manager()->contactsForHandles(pendingHandles->handles(), features());
    connect(mPriv->nested,
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onNestedFinished(Tp::PendingOperation*)));
}

void PendingContacts::onReferenceHandlesFinished(PendingOperation *operation)
{
    PendingHandles *pendingHandles = qobject_cast<PendingHandles *>(operation);

    if (pendingHandles->isError()) {
        debug() << "ReferenceHandles error" << operation->errorName()
                << "message" << operation->errorMessage();
        setFinishedWithError(operation->errorName(), operation->errorMessage());
        return;
    }

    ReferencedHandles validHandles = pendingHandles->handles();
    UIntList invalidHandles = pendingHandles->invalidHandles();
    ConnectionPtr conn = mPriv->manager->connection();
    mPriv->handlesToInspect = ReferencedHandles(conn, HandleTypeContact, UIntList());
    foreach (uint handle, mPriv->handles) {
        if (!mPriv->satisfyingContacts.contains(handle)) {
            int indexInValid = validHandles.indexOf(handle);
            if (indexInValid >= 0) {
                ReferencedHandles referencedHandle = validHandles.mid(indexInValid, 1);
                mPriv->handlesToInspect.append(referencedHandle);
            } else {
                mPriv->invalidHandles.push_back(handle);
            }
        }
    }

    QDBusPendingCallWatcher *watcher =
        new QDBusPendingCallWatcher(
                conn->baseInterface()->InspectHandles(HandleTypeContact,
                    mPriv->handlesToInspect.toList()),
                this);
    connect(watcher,
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(onInspectHandlesFinished(QDBusPendingCallWatcher*)));
}

void PendingContacts::onNestedFinished(PendingOperation *operation)
{
    Q_ASSERT(operation == mPriv->nested);

    if (operation->isError()) {
        debug() << " error" << operation->errorName()
                << "message" << operation->errorMessage();
        setFinishedWithError(operation->errorName(), operation->errorMessage());
        return;
    }

    mPriv->contacts = mPriv->nested->contacts();
    mPriv->nested = 0;
    mPriv->setFinished();
}

void PendingContacts::onInspectHandlesFinished(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QStringList> reply = *watcher;

    if (reply.isError()) {
        debug().nospace() << "InspectHandles: error " << reply.error().name() << ": "
            << reply.error().message();
        setFinishedWithError(reply.error());
        return;
    }

    QStringList names = reply.value();
    int i = 0;
    ConnectionPtr conn = mPriv->manager->connection();
    foreach (uint handle, mPriv->handlesToInspect) {
        QVariantMap handleAttributes;
        handleAttributes.insert(QLatin1String(TELEPATHY_INTERFACE_CONNECTION "/contact-id"),
                names[i++]);
        ReferencedHandles referencedHandle(conn, HandleTypeContact,
                UIntList() << handle);
        mPriv->satisfyingContacts.insert(handle, manager()->ensureContact(referencedHandle,
                    mPriv->missingFeatures, handleAttributes));
    }

    allAttributesFetched();

    watcher->deleteLater();
}

void PendingContacts::allAttributesFetched()
{
    foreach (uint handle, mPriv->handles) {
        if (mPriv->satisfyingContacts.contains(handle)) {
            mPriv->contacts.push_back(mPriv->satisfyingContacts[handle]);
        }
    }

    mPriv->setFinished();
}

} // Tp
