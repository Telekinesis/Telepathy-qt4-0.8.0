/*
 * This file is part of TelepathyQt4
 *
 * Copyright (C) 2011 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2011 Nokia Corporation
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

#ifndef _TelepathyQt4_examples_file_transfer_pending_file_receive_h_HEADER_GUARD_
#define _TelepathyQt4_examples_file_transfer_pending_file_receive_h_HEADER_GUARD_

#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/IncomingFileTransferChannel>
#include <TelepathyQt4/Types>

#include "pending-file-transfer.h"

using namespace Tp;

class PendingFileReceive : public PendingFileTransfer
{
    Q_OBJECT
    Q_DISABLE_COPY(PendingFileReceive)

public:
    PendingFileReceive(const IncomingFileTransferChannelPtr &chan, const SharedPtr<RefCounted> &object);
    ~PendingFileReceive();

private Q_SLOTS:
    void onTransferStateChanged(Tp::FileTransferState state,
            Tp::FileTransferStateChangeReason stateReason);

private:
    bool mReceivingFile;
    QFile mFile;
};

#endif
