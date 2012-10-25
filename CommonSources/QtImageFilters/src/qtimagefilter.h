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
#ifndef QTIMAGEFILTER_H
#define QTIMAGEFILTER_H
#include <QtGui/QImage>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include "qtmatrix.h"

struct  QtImageFilter {
    
    enum FilterOption {
        FilterChannels = 1,
        FilterBorderPolicy,
        ConvolutionDivisor,
        ConvolutionBias,
        ConvolutionKernelMatrix,
        Radius,
        Center,
        Force,
        UserOption = 0x100
    };

    virtual QVariant option(int filteroption) const;    
    
    virtual bool setOption(int filteroption, const QVariant &value);

    virtual bool supportsOption(int filteroption) const;

    virtual QImage apply(const QImage &img, const QRect& clipRect = QRect() ) const = 0;

    virtual QString name() const = 0;

    virtual QString description() const;

    virtual ~QtImageFilter() {}
};

struct QtImageFilterFactory {
    typedef QtImageFilter* (*ImageFilterFactoryFunction)(void);
    static QtImageFilter *createImageFilter(const QString &name);

    static void registerImageFilter(const QString &name, ImageFilterFactoryFunction func);
    
    static QStringList imageFilterList();

#if !defined(QT_NO_MEMBER_TEMPLATES)
    template<typename T>
    static QtImageFilter *automaticFilterCreator()
    {
        return new T;
    }

    template<typename T>
    static void registerImageFilter(const QString &name)
    {
        registerImageFilter(name, &automaticFilterCreator<T>);
    }
#endif

};

// For compilers with broken template member function support and for those that want to keep
// portability high.
template<typename T>
QtImageFilter *qtAutomaticFilterCreator(const T * /*dummy*/ = 0)  { return new T;}

template<typename T>
void qtRegisterImageFilter(const QString &name, T * /*dummy*/ = 0)
{
    typedef QtImageFilter* (* ConstructPtr)(const T*);
    ConstructPtr cptr = qtAutomaticFilterCreator<T>;
    QtImageFilterFactory::registerImageFilter(name, 
                reinterpret_cast<QtImageFilterFactory::ImageFilterFactoryFunction>(cptr));
}

typedef QtMatrix<int> QtConvolutionKernelMatrix;
Q_DECLARE_METATYPE(QtConvolutionKernelMatrix)


#endif // QTIMAGEFILTER_H
