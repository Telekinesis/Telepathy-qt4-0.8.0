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

#ifndef _TelepathyQt4_feature_h_HEADER_GUARD_
#define _TelepathyQt4_feature_h_HEADER_GUARD_

#ifndef IN_TELEPATHY_QT4_HEADER
#error IN_TELEPATHY_QT4_HEADER
#endif

#include <TelepathyQt4/Global>

#include <QMetaType>
#include <QPair>
#include <QSet>
#include <QSharedDataPointer>
#include <QString>

namespace Tp
{

class TELEPATHY_QT4_EXPORT Feature : public QPair<QString, uint>
{
public:
    Feature();
    Feature(const QString &className, uint id, bool critical = false);
    Feature(const Feature &other);
    ~Feature();

    bool isValid() const { return mPriv.constData() != 0; }

    Feature &operator=(const Feature &other);

    bool isCritical() const;

private:
    struct Private;
    friend struct Private;
    QSharedDataPointer<Private> mPriv;
};

class TELEPATHY_QT4_EXPORT Features : public QSet<Feature>
{
public:
    Features() { }
    Features(const Feature &feature) { insert(feature); }
    Features(const QSet<Feature> &s) : QSet<Feature>(s) { }
};

inline Features operator|(const Feature &feature1, const Feature &feature2)
{
    return Features() << feature1 << feature2;
}

inline Features operator|(const Features &features, const Feature &feature)
{
    return Features(features) << feature;
}

inline uint qHash(const Features &features)
{
    int ret = 0;
    Q_FOREACH (const Feature &feature, features) {
        int h = qHash(feature);
        ret ^= h;
    }
    return ret;
}

} // Tp

Q_DECLARE_METATYPE(Tp::Feature);
Q_DECLARE_METATYPE(Tp::Features);

#endif
