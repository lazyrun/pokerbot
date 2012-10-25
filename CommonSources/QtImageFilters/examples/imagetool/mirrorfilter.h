/****************************************************************************
**
** Copyright (C) 2003-2006 Trolltech ASA. All rights reserved.
**
** This file is part of a Qt Solutions component.
**
** Licensees holding valid Qt Solutions licenses may use this file in
** accordance with the Qt Solutions License Agreement provided with the
** Software.
**
** See http://www.trolltech.com/products/qt/addon/solutions/ 
** or email sales@trolltech.com for information about Qt Solutions
** License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef MIRRORFILTER_H
#define MIRRORFILTER_H

#include "qtimagefilter.h"

class MirrorFilter : public QtImageFilter {
public:
    enum MirrorFilterOption {
        MirrorHorizontal = UserOption,
        MirrorVertical
    };

    MirrorFilter()
    {
        m_mirrorVertical = false;
        m_mirrorHorizontal = true;
    }

    QImage apply(const QImage &img, const QRect& clipRect /*= QRect()*/ ) const
    {
        Q_UNUSED(clipRect);
        return img.mirrored(m_mirrorHorizontal, m_mirrorVertical);
    }

    QString name() const { return QLatin1String("Mirror"); }

    QVariant option(int filteroption) const
    {
        switch ((MirrorFilterOption)filteroption) {
        case MirrorVertical:
            return QVariant(m_mirrorVertical);
        break;
        case MirrorHorizontal:
            return QVariant(m_mirrorHorizontal);
        break;
        default:
            break;
        }
        return QVariant();
    }

    bool setOption(int filteroption, const QVariant &value)
    {
        switch ((MirrorFilterOption)filteroption) {
            case MirrorVertical:
                m_mirrorVertical = value.toBool();
            break;
            case MirrorHorizontal:
                m_mirrorHorizontal = value.toBool();
            break;
            default:
                break;
        }
        return true;
    }

    bool supportsOption(int option) const
    {
        bool supports = false;
        switch ((MirrorFilterOption)option) {
            case MirrorVertical:
            case MirrorHorizontal:
                supports = true;
            break;
            default:
                break;
        }
        return supports;
    }

    QString description() const { return QObject::tr("A simple mirroring filter."); }

private:
    bool m_mirrorVertical;
    bool m_mirrorHorizontal;
};
#endif  /* MIRRORFILTER_H */
