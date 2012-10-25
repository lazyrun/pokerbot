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
#ifndef CONVOLUTIONFILTER_H
#define CONVOLUTIONFILTER_H
#include <QtCore/QFlags>
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QVariant>

#include "qtimagefilter.h"

class QImage;

struct KernelMatrixData {
    QtConvolutionKernelMatrix matrix;
    int divisor;
    int bias;
};

class ConvolutionFilter : public QtImageFilter {
public:
    typedef enum {      // This enum specifies in what alternative way pixels should be selected if a pixel 
                        // is outside the image bounds.
        Extend = 1,     // Will pick the closest border pixel, (the border pixels defines what is outside the image)
                        // This is the default.
        Mirror = 2,     // The image is mirrored.
        Wrap            // The image is wrapped. (Good for tiled images.)
    } FilterBorderPolicy;
    
    enum FilterChannelFlag {
        Red   = 0x01,
        Green = 0x02,
        Blue  = 0x04,
        Alpha = 0x08,
        RGB   = Red | Green | Blue,
        RGBA  = RGB | Alpha
    };
    Q_DECLARE_FLAGS(FilterChannels, FilterChannelFlag)

    ConvolutionFilter();
    void addKernel(  const QtConvolutionKernelMatrix &kernelMatrix,
                FilterChannels channels, FilterBorderPolicy borderPolicy,
                int divisor = 0, int bias = 0);

    ////
    // INHERITED FROM QtImageFilter
    ////
    QVariant option(int option) const;
    bool setOption(int option, const QVariant &value);
    bool supportsOption(int option) const;
    QImage apply(const QImage &image, const QRect& clipRect = QRect() ) const;
    QString name() const { return m_name; }
    QString description() const { return m_description; }
    ~ConvolutionFilter();


    void setName(const QString &name){
        m_name = name;
    }

    void setDescription(const QString &desc)
    {
        m_description = desc;
    }

protected:
    FilterChannels m_channels;
    FilterBorderPolicy m_borderPolicy;
    QVector<KernelMatrixData> m_kernels;

private:

    bool setChannels(const QString &rgba);
    QString getChannels() const;
    bool setBorderPolicy(const QString &borderPolicy);
    QString getBorderPolicy() const;

    QImage convolve(const QImage &img, const QRect& clipRect) const;
    QRgb convolvePixelRGBA(   const QImage &img, int x, int y, const int *kernelMatrix, int kernelRows, int kernelColumns, 
                                    int divisor, int bias) const;

private:
    QString  m_name;
    QString     m_description;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(ConvolutionFilter::FilterChannels)
#endif //CONVOLUTIONFILTER_H

