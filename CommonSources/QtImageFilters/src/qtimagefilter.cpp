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
#include "qtimagefilter.h"
#include <QtCore/QObject>
/*!
    \class QtImageFilter

    \brief The QtImageFilter class is an abstract interface for
    image filter implementations.

    QtImageFilter is part of the QtImageFilters solution which by
    default supports many popular image filters, among these are
    gaussian blurring, embossing (see image below), sharpening, defocus,
    punch/pinch and convolution filters.

    \image qtlogo_emboss.png

    The documentation for the
    QtImageFilterFactory::createImageFilter() function provides a
    complete list of all the supported image filters.

    An image filter can be configured by setting various options. The
    options are set by associating a value from the FilterOption enum
    with a QVariant value using the setOption() function. The option()
    function returns the variant value of the given FilterOption
    value, while the supportsOption() function determines whether the
    given FilterOption value is supported by this image filter.

    The image filter's name and description can be retrieved using the
    name() and description() functions respectively.The apply()
    function is used by the QtImageFilters framework to apply the image
    filters.

    Developers can extend the framework with their own custom image
    filters by deriving from the QtImageFilter class. For example, the
    code below implements a simple mirror filter:

    \quotefromfile ../examples/imagetool/mirrorfilter.h
    \skipto class MirrorFilter
    \printuntil /^\};/

    A custom image filter must implement the pure virtual apply() and
    name() functions. In addition, the custom mirror filter above
    reimplements the option(), setOption() and supportsOption()
    functions inherited from the QtImageFilter class. Custom image
    filters can be configured just like any other image filter. In
    addition, the mirror filter provides a description of itself by
    reimplementing the description() function.

    Finally, our custom filter implements a MirrorFilterOption enum
    and a couple of private variables.

    Before it can be used, a custom image filter must be registered
    using the QtImageFilterFactory class. Once it is registered, the
    filter can be created just like any other image filter, using the
    QtImageFilterFactory::createImageFilter() function.

    \sa QtImageFilterFactory
*/

/*!
    \enum QtImageFilter::FilterOption

    This enum describes the various configuring options that can be
    set on a filter.

    \value FilterChannels The color channel(s) to perform the
    filtering on. A string consisting of zero or one entity of each of
    the characters r,g,b,a.

    \value FilterBorderPolicy Describes the behavior of the
    convolution when the convolution stencil is not within the bounds
    of the image. For example, if the convolution stencil is a 5x5
    grid, and its central element is positioned at the image
    coordinates (0, 0), then the topleft element of the stencil will
    reference the image coordinates (-2, -2), and the bottom-right
    element of the stencil will reference the image coordinates (2,
    2).  For this specific case the 2 left-most columns and 2 top-most
    rows in the stencil grid will be references to pixels that are
    outside the image coordinates. This option specifies how the
    filter should behave in these special conditions:

    \list

        \o If the policy is set to "mirror", the pixels is chosen in
        such a way that the image is considered to be mirrored around
        its edges.

        \o If the policy is set to "extend", the pixel value is chosen
        in such a way that the border colors of the image is
        considered to be extended infinitely.

        \o If the policy is set to "wrap", the pixel value is chosen
        in such a way that the image is considered to be wrapped. This
        is optimal if you want to perform filtering on tiled images.

    \endlist

    \value ConvolutionDivisor An integer number.
    \value ConvolutionBias An integer number.
    \value ConvolutionKernelMatrix A QtConvolutionKernelMatrix matrix.
    \value Radius A double number.
    \value Force A double number.
    \value Center A QPointF object.
    \value UserOption The first option that can be used for
    application-specific purposes.

    \sa option(), setOption(), supportsOption()
*/


/*!
    Retrieves the variant value of the given \a filteroption.

    \sa setOption(), supportsOption(), FilterOption
 */
QVariant QtImageFilter::option(int filteroption) const
{
    Q_UNUSED(filteroption);
    return QVariant();
}

/*!
    Sets the given \a filteroption on this image filter, and returns
    true if the configuring succeeds.

    The possible values for the \a value parameter depends on the given
    filter option. If \a value contain an illegal value, or if the
    filter does not support the given \a filteroption, the function
    returns false.

    \sa option(), supportsOption(), FilterOption
*/
bool QtImageFilter::setOption(int filteroption, const QVariant &value)
{
    Q_UNUSED(filteroption);
    Q_UNUSED(value);
    return false;
}

/*!
    Returns true if the current image filter supports the given \a
    filteroption, otherwise returns false.

    Reimplement this function to reflect the options that the image
    filter supports. The default implementation returns false.

    \sa setOption(), FilterOption
*/
bool QtImageFilter::supportsOption(int filteroption) const
{
    Q_UNUSED(filteroption);
    return false;
}

/*!
    \fn QImage QtImageFilter::apply(const QImage &image, const QRect& clipRectangle = QRect() ) const

    This function is used by the QtImageFilters framework to apply the
    filter on the given \a image, returning the resulting filtered
    image. The \a clipRectangle parameter delimits the area that is
    filtered.

    This is a pure virtual function that must be implemented in
    derived classes. The format of the returned image should be the
    same as the input image format (unless documented otherwise).
*/

/*!
    \fn QString QtImageFilter::name() const

    Returns the name of the filter which also is an unique identifier.

    This is a pure virtual function that must be implemented in
    derived classes.

    \sa description()
*/

/*!
    Returns a brief description of the image filter, usually only
    one sentence.

    \sa name()
*/
QString QtImageFilter::description() const
{
    return QObject::tr("The filter has no description.", "QtImageFilter");
}

/*!
    \fn QtImageFilter::~QtImageFilter()

    Destroys this image filter.
*/


