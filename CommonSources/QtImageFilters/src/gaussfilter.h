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
#ifndef GAUSSBLURFILTER_H
#define GAUSSBLURFILTER_H

#include <math.h>
#include "convolutionfilter.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

class GaussBlurFilter : public ConvolutionFilter
{
    public:
        GaussBlurFilter() : ConvolutionFilter()
        {
            m_radius = 4.0;
        }

        ////
        // INHERITED FROM ConvolutionFilter
        ////
        QVariant option(int option) const;

        bool setOption(int option, const QVariant &value)
        {
            bool ok = true;
            switch (option) {
                case QtImageFilter::Radius:
                {
                    double radius = value.toDouble(&ok);
                    if (ok) m_radius = radius;
                }
                break;

                default:
                    ok = ConvolutionFilter::setOption(option, value);
                break;
            }
            return ok;
        }

        bool supportsOption(int option) const
        {
            if (option == QtImageFilter::Radius) return true;
            return ConvolutionFilter::supportsOption(option);
        }

	QImage apply(const QImage &image, const QRect& clipRect ) const;
        
	QString name() const { return QLatin1String("GaussBlur"); }
        QString description() const { return QObject::tr("A gaussian blur filter", "GaussBlurFilter"); }

    private:
        qreal m_radius;
};


static qreal Gauss2DFunction(int x, int y, qreal deviance)
{
    /**
    * A 2-D gaussian distribution has the shape:
    *
    *               1         -(x*x + y*y)/(2*dev*dev)
    * G(x,y) = ---------- * e^
    *          2*PI*dev^2
    *
    * see http://www.cee.hw.ac.uk/hipr/html/gsmooth.html for a more readable version
    */
    return exp(-(x*x + y*y)/(2*deviance*deviance))/(2*M_PI*deviance*deviance);
}

QVariant GaussBlurFilter::option(int option) const
{
    if (option == QtImageFilter::Radius) return true;
    return ConvolutionFilter::option(option);
}

QImage GaussBlurFilter::apply(const QImage &image, const QRect& clipRect ) const
{
    bool ok = true;
    if (m_radius > 0.0) {
        int uRadius = (int)ceil(m_radius);

        double deviance = sqrt(-m_radius*m_radius/(2*log(1/255.0)));
        QtMatrix<double> matLeft(2 * uRadius + 1, 1);

        for (int x = -uRadius; x <=uRadius; x++) {
            matLeft.setData(uRadius + x, 0, Gauss2DFunction(x, 0, deviance));
        }
        double scalar = matLeft.at(uRadius, 0);
        matLeft*=(255.0/scalar);
        QtMatrix<double> matRight = matLeft.transposed();
        
        QtConvolutionKernelMatrix integerMatrixLeft = convertMatrixBasetype<int,double>(matLeft);
        QtConvolutionKernelMatrix integerMatrixRight = convertMatrixBasetype<int,double>(matRight);
        
        GaussBlurFilter *localThis = const_cast<GaussBlurFilter*>(this);
        localThis->addKernel(integerMatrixLeft, m_channels, m_borderPolicy);
        localThis->addKernel(integerMatrixRight, m_channels, m_borderPolicy);
    } else {
        ok = false;
    }
    return ConvolutionFilter::apply(image, clipRect);
}

#endif //GAUSSBLURFILTER_H

