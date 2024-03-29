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

#ifndef _TelepathyQt4_location_info_h_HEADER_GUARD_
#define _TelepathyQt4_location_info_h_HEADER_GUARD_

#ifndef IN_TELEPATHY_QT4_HEADER
#error IN_TELEPATHY_QT4_HEADER
#endif

#include <TelepathyQt4/Global>

#include <QDateTime>
#include <QSharedDataPointer>
#include <QString>
#include <QVariantMap>

namespace Tp
{

class TELEPATHY_QT4_EXPORT LocationInfo
{
public:
    LocationInfo();
    LocationInfo(const QVariantMap &location);
    LocationInfo(const LocationInfo &other);
    virtual ~LocationInfo();

    bool isValid() const { return mPriv.constData() != 0; }

    LocationInfo &operator=(const LocationInfo &other);

    QString countryCode() const;
    QString country() const;
    QString region() const;
    QString locality() const;
    QString area() const;
    QString postalCode() const;
    QString street() const;

    QString building() const;
    QString floor() const;
    QString room() const;
    QString text() const;
    QString description() const;
    QString uri() const;

    QString language() const;

    double latitude() const;
    double longitude() const;
    double altitude() const;
    double accuracy() const;

    double speed() const;
    double bearing() const;

    QDateTime timestamp() const;

    QVariantMap allDetails() const;

private:
    friend class Contact;

    TELEPATHY_QT4_NO_EXPORT void updateData(const QVariantMap &location);

    struct Private;
    friend struct Private;
    QSharedDataPointer<Private> mPriv;
};

} // Tp

Q_DECLARE_METATYPE(Tp::LocationInfo);

#endif
