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
#include "convolutionfilter.h"
#include <QtGui/QImage>


ConvolutionFilter::ConvolutionFilter()
{
    m_borderPolicy = ConvolutionFilter::Extend;
    m_channels = ConvolutionFilter::RGB;
}

void ConvolutionFilter::addKernel(   const QtConvolutionKernelMatrix &kernelMatrix, 
                                        FilterChannels channels, FilterBorderPolicy borderPolicy,
                                        int divisor /*= 0*/, int bias /*= 0*/)
{
    KernelMatrixData kernel;
    kernel.matrix = kernelMatrix;
    // If the divisor is 0, calculate it by summing all elements in the matrix
    if (divisor == 0) {
        int i;
        for (i = 0; i < kernelMatrix.columnCount()*kernelMatrix.rowCount(); i++) {
            divisor+=kernelMatrix.at(i);
        }
    }
    kernel.divisor = divisor;
    kernel.bias = bias;
    m_kernels.append(kernel);
    m_channels = channels;
    m_borderPolicy = borderPolicy;
}

bool ConvolutionFilter::setOption(int option, const QVariant &value)
{
    bool bOK = true;
    switch (option) {
        case QtImageFilter::FilterChannels:
        bOK = setChannels(value.toString());
        break;

        case QtImageFilter::FilterBorderPolicy:
        bOK = setBorderPolicy(value.toString());
        break;

        case QtImageFilter::ConvolutionDivisor:
        {
            int divisor = value.toInt(&bOK);
            if (bOK) {
                for (int i = 0; i < m_kernels.count(); ++i) {
                    m_kernels[i].divisor = divisor;
                }
            }
        }
        break;

        case QtImageFilter::ConvolutionBias:
        {
            int bias = value.toInt(&bOK);
            if (bOK) {
                for (int i = 0; i < m_kernels.count(); ++i) {
                    m_kernels[i].bias = bias;
                }
            }
        }
        break;
        case QtImageFilter::ConvolutionKernelMatrix:
        {
            if (qVariantCanConvert<QtConvolutionKernelMatrix>(value)) {
                QtConvolutionKernelMatrix kernel = qVariantValue<QtConvolutionKernelMatrix>(value);
                addKernel(kernel, m_channels, m_borderPolicy);
            }
        }
        break;

        default:
        bOK = false;
        break;
    }
    return bOK;
}

QVariant ConvolutionFilter::option(int option) const
{
    QVariant value;
    switch (option) {
        case QtImageFilter::FilterChannels:
            value = getChannels();
            break;
        case QtImageFilter::FilterBorderPolicy:
            value = getBorderPolicy();
            break;
        case QtImageFilter::ConvolutionDivisor:
            if (m_kernels.count()) value = m_kernels[0].divisor;    //### go through all and sum up?
            break;
        case QtImageFilter::ConvolutionBias:
            if (m_kernels.count()) value = m_kernels[0].bias;       //### same here
            break;
        case QtImageFilter::ConvolutionKernelMatrix:
            if (m_kernels.size()) {
                qVariantSetValue<QtConvolutionKernelMatrix>(value, m_kernels[0].matrix);
            }
            break;
        default:
            break;
    }
    return value;
}

bool ConvolutionFilter::supportsOption(int option) const
{
    switch (option) {
    case QtImageFilter::FilterChannels:
    case QtImageFilter::FilterBorderPolicy:
    case QtImageFilter::ConvolutionDivisor:
    case QtImageFilter::ConvolutionBias:
    case QtImageFilter::ConvolutionKernelMatrix:
        return true;
    default:
        break;
    }
    return false;
}

QImage ConvolutionFilter::apply(const QImage &image, const QRect& clipRect /*= QRect()*/ ) const
{
    return convolve(image, clipRect);
}

bool ConvolutionFilter::setChannels(const QString &rgba)
{
    bool bOK = true;
    int rgbaCount[4] = {0,0,0,0};
    // We don't want to accept multiple number of channels (e.g. "rrgbaaa")
    // (That might break the interface for future changes)
    int i;
    for (i = 0; i < rgba.length() && bOK; i++) {
        int index = -1;
        switch (rgba.at(i).toLatin1()) {
            case 'r':
            index = 0;
            break;
            case 'g':
            index = 1;
            break;
            case 'b':
            index = 2;
            break;
            case 'a':
            index = 3;
            break;
        }
        if (index != -1 && ++rgbaCount[index] > 1) bOK = false;
    }
    if (bOK)
    {
        FilterChannels channels;
        if (rgba.contains(QChar('r'))) channels |= ConvolutionFilter::Red;
        if (rgba.contains(QChar('g'))) channels |= ConvolutionFilter::Green;
        if (rgba.contains(QChar('b'))) channels |= ConvolutionFilter::Blue;
        if (rgba.contains(QChar('a'))) channels |= ConvolutionFilter::Alpha;
        m_channels = channels;
    }
    return bOK;
}

QString ConvolutionFilter::getChannels() const
{
    QByteArray chan;
    if (m_channels & ConvolutionFilter::Red)
        chan.append('r');
    if (m_channels & ConvolutionFilter::Green)
        chan.append('g');
    if (m_channels & ConvolutionFilter::Blue)
        chan.append('b');
    if (m_channels & ConvolutionFilter::Alpha)
        chan.append('a');
    return chan;
}

bool ConvolutionFilter::setBorderPolicy(const QString &borderPolicy)
{
    bool bOK = true;
    if (borderPolicy.toLower() == "mirror") {
        m_borderPolicy = ConvolutionFilter::Mirror;
    }else if (borderPolicy.toLower() == "extend") {
        m_borderPolicy = ConvolutionFilter::Extend;
    }else if (borderPolicy.toLower() == "wrap") {
        m_borderPolicy = ConvolutionFilter::Wrap;
    }else{
        bOK = false;
    }
    return bOK;
}

QString ConvolutionFilter::getBorderPolicy() const
{
    switch (m_borderPolicy) {
    case ConvolutionFilter::Mirror:
        return QLatin1String("mirror");
        break;
    case ConvolutionFilter::Extend:
        return QLatin1String("mirror");
        break;
    case ConvolutionFilter::Wrap:
        return QLatin1String("wrap");
        break;
    default:
        break;
    }
    Q_ASSERT(!"ConvolutionFilter is in a inconsistent state");
    return QString();
}

ConvolutionFilter::~ConvolutionFilter()
{

}

QImage ConvolutionFilter::convolve(const QImage &img, const QRect& clipRect) const
{
    int top = 0;
    int bottom = img.height();
    int left = 0;
    int right = img.width();

    if (!clipRect.isNull()) {
        // If we have a cliprect, set our coordinates to our cliprect
        // and make sure it is within the boundaries of the image
        top = qMax(top, clipRect.top());
        bottom = qMin(bottom, clipRect.bottom());
        left = qMax(left, clipRect.left());
        right = qMin(right, clipRect.right());
    }


    QImage::Format fmt = img.format();
    QImage resultImg = img.convertToFormat(QImage::Format_ARGB32);

    int x;
    int y;

    for (int i = 0; i < m_kernels.count(); ++i) {
        QImage normalizedImg = resultImg;
        KernelMatrixData data = m_kernels[i];
        QtConvolutionKernelMatrix kernel = data.matrix;
        int divisor = data.divisor;
        int bias = data.bias;

        for (y = top; y < bottom; y++) {
            for (x = left; x < right; x++) {
                resultImg.setPixel( x, y, convolvePixelRGBA( normalizedImg, x, y, kernel.data(), 
                                    kernel.rowCount(), kernel.columnCount(), divisor, bias) );
            }
        }
    }
    if (resultImg.format() != fmt) {
        resultImg = resultImg.convertToFormat(fmt);
    }
    return resultImg;
}

QRgb ConvolutionFilter::convolvePixelRGBA(const QImage &img, int x, int y, const int *kernelMatrix, int kernelRows, int kernelColumns, 
                                                int divisor, int bias) const
{
    int i;
    int j;
    int rows = kernelRows;
    int cols = kernelColumns;

    int c_offset = x - kernelColumns / 2;
    int r_offset = y - kernelRows / 2;

    int sumRed   = 0;
    int sumGreen = 0;
    int sumBlue  = 0;
    int sumAlpha = 0;

    if (!(m_channels & ConvolutionFilter::Red)  ) sumRed   = qRed (  img.pixel(x, y));
    if (!(m_channels & ConvolutionFilter::Green)) sumGreen = qGreen( img.pixel(x, y));
    if (!(m_channels & ConvolutionFilter::Blue )) sumBlue  = qBlue ( img.pixel(x, y));
    if (!(m_channels & ConvolutionFilter::Alpha)) sumAlpha = qAlpha( img.pixel(x, y));


    for (i = 0; i < cols; i++) {
        for (j = 0; j < rows; j++) {
            int srcpix_x = c_offset + j;
            int srcpix_y = r_offset + i;

            if (srcpix_x < 0) {
                switch (m_borderPolicy) {
                    case ConvolutionFilter::Extend:
                    srcpix_x = 0;
                    break;
                    case ConvolutionFilter::Mirror:
                    srcpix_x = -srcpix_x;
                    srcpix_x %= img.width();
                    break;
                    case ConvolutionFilter::Wrap:
                    // In case of a large kernel and a small image, make sure we are in the bounds of the image coords
                    while (srcpix_x < 0) {
                        srcpix_x += img.width();
                    }
                    break;
                }
            }else if (srcpix_x >= img.width()) {
                switch (m_borderPolicy) {
                    case ConvolutionFilter::Extend:
                    srcpix_x = img.width() - 1;
                    break;
                    case ConvolutionFilter::Mirror:
                    srcpix_x %= img.width();
                    srcpix_x = img.width() - 1 - srcpix_x;
                    break;
                    case ConvolutionFilter::Wrap:
                    srcpix_x %= img.width();
                    break;
                }
            }

            if (srcpix_y < 0) {
                switch (m_borderPolicy) {
                    case ConvolutionFilter::Extend:
                    srcpix_y = 0;
                    break;
                    case ConvolutionFilter::Mirror:
                    srcpix_y = -srcpix_y;
                    srcpix_y %= img.height();
                    break;
                    case ConvolutionFilter::Wrap:
                    // In case of a large kernel and a small image, make sure we are in the bounds of the image coords
                    while (srcpix_y < 0) {
                        srcpix_y += img.height();
                    }
                    break;
                }
            }else if (srcpix_y >= img.height()) {
                switch (m_borderPolicy) {
                    case ConvolutionFilter::Extend:
                    srcpix_y = img.height() - 1;
                    break;
                    case ConvolutionFilter::Mirror:
                    srcpix_y %= img.height();
                    srcpix_y = img.height() - 1 - srcpix_y;
                    break;
                    case ConvolutionFilter::Wrap:
                    srcpix_y %= img.height();
                    break;
                }
            }

            QRgb rgb = img.pixel(srcpix_x, srcpix_y);
            int weight = kernelMatrix[i * rows + j];
            if (m_channels & ConvolutionFilter::Red)    sumRed   += qRed(rgb)   * weight;
            if (m_channels & ConvolutionFilter::Green)  sumGreen += qGreen(rgb) * weight;
            if (m_channels & ConvolutionFilter::Blue)   sumBlue  += qBlue(rgb)  * weight;
            if (m_channels & ConvolutionFilter::Alpha)  sumAlpha += qAlpha(rgb) * weight;
        }
    }

    if (m_channels & ConvolutionFilter::Red)
    {
        if (divisor) sumRed/=divisor;
        sumRed = qBound(0, sumRed + bias, 255);
    }

    if (m_channels & ConvolutionFilter::Green)
    {
        if (divisor) sumGreen/=divisor;
        sumGreen = qBound(0, sumGreen + bias, 255);
    }

    if (m_channels & ConvolutionFilter::Blue)
    {
        if (divisor) sumBlue/=divisor;
        sumBlue = qBound(0, sumBlue + bias, 255);
    }

    if (m_channels & ConvolutionFilter::Alpha)
    {
        if (divisor) sumAlpha/=divisor;
        sumAlpha = qBound(0, sumAlpha + bias, 255);
    }

    return qRgba(sumRed, sumGreen, sumBlue, sumAlpha);
}
