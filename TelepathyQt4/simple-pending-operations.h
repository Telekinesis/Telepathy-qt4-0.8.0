/**
 * This file is part of TelepathyQt4
 *
 * @copyright Copyright (C) 2008-2009 Collabora Ltd. <http://www.collabora.co.uk/>
 * @copyright Copyright (C) 2008-2009 Nokia Corporation
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

#ifndef _TelepathyQt4_pending_operations_h_HEADER_GUARD_
#define _TelepathyQt4_pending_operations_h_HEADER_GUARD_

#ifndef IN_TELEPATHY_QT4_HEADER
#error IN_TELEPATHY_QT4_HEADER
#endif

#include <QObject>

#include <TelepathyQt4/PendingOperation>

namespace Tp
{

class TELEPATHY_QT4_EXPORT PendingSuccess : public PendingOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(PendingSuccess)

public:
    PendingSuccess(const SharedPtr<RefCounted> &object)
        : PendingOperation(object)
    {
        setFinished();
    }
};

class TELEPATHY_QT4_EXPORT PendingFailure : public PendingOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(PendingFailure)

public:
    PendingFailure(const QString &name, const QString &message,
            const SharedPtr<RefCounted> &object)
        : PendingOperation(object)
    {
        setFinishedWithError(name, message);
    }

    PendingFailure(const QDBusError &error,
            const SharedPtr<RefCounted> &object)
        : PendingOperation(object)
    {
        setFinishedWithError(error);
    }
};

class TELEPATHY_QT4_EXPORT PendingVoid : public PendingOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(PendingVoid)

public:
    PendingVoid(QDBusPendingCall call, const SharedPtr<RefCounted> &object);

private Q_SLOTS:
    TELEPATHY_QT4_NO_EXPORT void watcherFinished(QDBusPendingCallWatcher*);

private:
    struct Private;
    friend struct Private;
    Private *mPriv;
};

class TELEPATHY_QT4_EXPORT PendingComposite : public PendingOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(PendingComposite)

public:
    PendingComposite(const QList<PendingOperation*> &operations, const SharedPtr<RefCounted> &object);
    PendingComposite(const QList<PendingOperation*> &operations, bool failOnFirstError,
            const SharedPtr<RefCounted> &object);
    ~PendingComposite();

private Q_SLOTS:
    TELEPATHY_QT4_NO_EXPORT void onOperationFinished(Tp::PendingOperation *);

private:
    struct Private;
    friend struct Private;
    Private *mPriv;
};

} // Tp

#endif
