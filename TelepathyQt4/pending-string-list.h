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

#ifndef _TelepathyQt4_pending_string_list_h_HEADER_GUARD_
#define _TelepathyQt4_pending_string_list_h_HEADER_GUARD_

#ifndef IN_TELEPATHY_QT4_HEADER
#error IN_TELEPATHY_QT4_HEADER
#endif

#include <TelepathyQt4/PendingOperation>

#include <QStringList>

namespace Tp
{

class TELEPATHY_QT4_EXPORT PendingStringList : public PendingOperation
{
    Q_OBJECT
    Q_DISABLE_COPY(PendingStringList);

public:
    PendingStringList(const SharedPtr<RefCounted> &object);
    PendingStringList(QDBusPendingCall call, const SharedPtr<RefCounted> &object);
    ~PendingStringList();

    QStringList result() const;

protected:
    void setResult(const QStringList &result);

private Q_SLOTS:
    TELEPATHY_QT4_NO_EXPORT void watcherFinished(QDBusPendingCallWatcher *watcher);

private:
    struct Private;
    friend struct Private;
    Private *mPriv;
};

} // Tp

#endif
