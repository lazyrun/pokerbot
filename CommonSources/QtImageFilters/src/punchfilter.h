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
#ifndef PUNCHFILTER_H
#define PUNCHFILTER_H

#include "qtimagefilter.h"

class PunchFilter : public QtImageFilter {
public:
    PunchFilter();
    ////
    // INHERITED FROM QtImageFilter
    ////
    QVariant option(int filteroption) const;
    bool setOption(int option, const QVariant &value);
    bool supportsOption(int option) const;
    QImage apply(const QImage &img, const QRect& clipRect = QRect() ) const;
    QString name() const { return QLatin1String("Punch"); }
    QString description() const { return QObject::tr("A parametrized circular pinch/punch filter", "PunchFilter"); }

private:
    bool Punch(const QImage &img, QImage *outputImage, const QRect &clipRect = QRect() ) const;

private:
    double      m_Radius;
    QPointF     m_Center;
    double      m_Force;
};
#endif  /* PUNCHFILTER_H */
