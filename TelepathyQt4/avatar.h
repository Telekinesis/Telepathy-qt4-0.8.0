/**
 * This file is part of TelepathyQt4
 *
 * @copyright Copyright (C) 2010-2011 Collabora Ltd. <http://www.collabora.co.uk/>
 * @copyright Copyright (C) 2010-2011 Nokia Corporation
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

#ifndef _TelepathyQt4_avatar_h_HEADER_GUARD_
#define _TelepathyQt4_avatar_h_HEADER_GUARD_

#ifndef IN_TELEPATHY_QT4_HEADER
#error IN_TELEPATHY_QT4_HEADER
#endif

#include <TelepathyQt4/Global>

#include <QSharedDataPointer>
#include <QString>
#include <QStringList>
#include <QMetaType>

namespace Tp
{

struct TELEPATHY_QT4_EXPORT AvatarData
{
public:
    inline AvatarData(const QString &fileName, const QString &mimeType)
        : fileName(fileName), mimeType(mimeType) {}
    inline AvatarData() {}

    QString fileName;
    QString mimeType;
};

class TELEPATHY_QT4_EXPORT AvatarSpec
{
public:
    AvatarSpec();
    AvatarSpec(const QStringList &supportedMimeTypes,
            uint minHeight, uint maxHeight, uint recommendedHeight,
            uint minWidth, uint maxWidth, uint recommendedWidth,
            uint maxBytes);
    AvatarSpec(const AvatarSpec &other);
    ~AvatarSpec();

    bool isValid() const { return mPriv.constData() != 0; }

    AvatarSpec &operator=(const AvatarSpec &other);

    QStringList supportedMimeTypes() const;
    uint minimumHeight() const;
    uint maximumHeight() const;
    uint recommendedHeight() const;
    uint minimumWidth() const;
    uint maximumWidth() const;
    uint recommendedWidth() const;
    uint maximumBytes() const;

private:
    struct Private;
    friend struct Private;
    QSharedDataPointer<Private> mPriv;
};

} // Tp

Q_DECLARE_METATYPE(Tp::AvatarData);
Q_DECLARE_METATYPE(Tp::AvatarSpec);

#endif
