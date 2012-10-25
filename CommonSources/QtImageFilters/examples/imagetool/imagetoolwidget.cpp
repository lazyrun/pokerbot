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
#include <QtGui/QImageReader>
#include <QtGui/QFileDialog>
#include <QtGui/QImage>
#include <QtGui/QMessageBox>

#include "imagetoolwidget.h"
#include "qtimagefilter.h"
#include "mirrorfilter.h"

ImageToolWidget::ImageToolWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    // Register our mirror filter.
    qtRegisterImageFilter<MirrorFilter>(QLatin1String("MirrorFilter"));

    // Iterate through all registered filters and append them to the vector
    QStringList filters = QtImageFilterFactory::imageFilterList();
    for (int i = 0; i < filters.count(); ++i) {
        m_imageFilters += QtImageFilterFactory::createImageFilter(filters[i]);
    }

    for (int i = 0; i < m_imageFilters.count(); ++i) {
        ui.FiltersCombo->addItem(m_imageFilters[i]->name());
    }
    
    ui.FiltersCombo->insertItem(0, QLatin1String("--Choose filter--"));

    m_imageFilters.prepend((QtImageFilter*)0);

    QObject::connect(ui.LoadButton, SIGNAL(clicked()), this, SLOT(loadImage()));
    QObject::connect(ui.ReloadButton, SIGNAL(clicked()), this, SLOT(reloadImage()));
    QObject::connect(ui.FilterButton, SIGNAL(clicked()), this, SLOT(filterImage()));
    QObject::connect(ui.FiltersCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(filterIndexChanged(int)));

    ui.FiltersCombo->setCurrentIndex(0);
    m_currentFilename = QLatin1String("images/qtlogo.png");
    reloadImage();
}

ImageToolWidget::~ImageToolWidget()
{

}

void ImageToolWidget::loadImage()
{
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    QString strFormats;
    int i;
    for (i = 0; i < formats.size(); i++) {
        if (i != 0) strFormats+=" ";
        strFormats += QString("*.") + formats.at(i);
    }
    strFormats = "Images (" + strFormats + ")";
    QString s = QFileDialog::getOpenFileName(
            this,
            "Choose a file",
            ".",
            strFormats);
    QImage img;
    if(img.load(s)) {
        m_currentFilename = s;
        QPixmap pixmap = QPixmap::fromImage(img);
        setPixmapAndResize(pixmap);
    }
}

void ImageToolWidget::reloadImage()
{
    QImage img;
    if(img.load(m_currentFilename)) {
        QPixmap pixmap = QPixmap::fromImage(img);
        setPixmapAndResize(pixmap);
    }
}

void ImageToolWidget::filterIndexChanged(int index)
{
    if (index == 0 || index >= m_imageFilters.count()) {
        ui.FilterButton->setToolTip(QLatin1String("No image filter chosen"));
        ui.FilterButton->setEnabled(false);
        ui.gbBorderPolicy->setVisible( false );
        ui.gbChannels->setVisible( false );
        ui.gbMirror->setVisible( false );
    } else {
        QtImageFilter *filter = m_imageFilters[index];
        ui.FilterButton->setToolTip(filter->description());
        ui.FilterButton->setEnabled(true);
        ui.gbBorderPolicy->setVisible( filter->supportsOption(QtImageFilter::FilterBorderPolicy)  );
        ui.gbChannels->setVisible( filter->supportsOption(QtImageFilter::FilterChannels)  );
        ui.gbMirror->setVisible( filter->supportsOption(MirrorFilter::MirrorHorizontal) || filter->supportsOption(MirrorFilter::MirrorVertical) );
    }
}

void ImageToolWidget::filterImage()
{
    if (ui.PixmapLabel->pixmap() == 0) {
        QMessageBox::information(this, "QImageTool", "Sorry, you must load an image first\n");
    } else {
        setCursor(Qt::WaitCursor);
        QImage imgToFilter = ui.PixmapLabel->pixmap()->toImage();
        QtImageFilter *filter = m_imageFilters[ui.FiltersCombo->currentIndex()];
        if (filter->name() == "Punch") {
            filter->setOption(QtImageFilter::Radius, qMin(imgToFilter.width(), imgToFilter.height())/2);
            filter->setOption(QtImageFilter::Center, QPointF(imgToFilter.width()/2.0,imgToFilter.height()/2.0));
            filter->setOption(QtImageFilter::Force, 0.5);
        }else if (filter->name() == "ConvolutionFilter") {
            // A simple mean filter just to demonstrate that we can add our own kernels.
            static int kernelElements[9] = 
            {    1,  1,  1,
                 1,  1,  1,
                 1,  1,  1 };
            QtConvolutionKernelMatrix kernel(kernelElements, 3, 3);
            QVariant value;
            qVariantSetValue<QtConvolutionKernelMatrix>(value, kernel);
            filter->setOption(QtImageFilter::ConvolutionKernelMatrix, value);

        }
        if (filter->supportsOption(MirrorFilter::MirrorHorizontal))
            filter->setOption(MirrorFilter::MirrorHorizontal, ui.ckHorizontal->isChecked() );
        if (filter->supportsOption(MirrorFilter::MirrorVertical))
            filter->setOption(MirrorFilter::MirrorVertical, ui.ckVertical->isChecked());

        if (filter->supportsOption(QtImageFilter::FilterChannels)) {
            QString rgba = ui.ckRed->isChecked() ? "r" : "";
            rgba+= ui.ckGreen->isChecked() ? "g" : "";
            rgba+= ui.ckBlue->isChecked() ? "b" : "";
            rgba+= ui.ckAlpha->isChecked() ? "a" : "";
            filter->setOption(QtImageFilter::FilterChannels, rgba);
        }

        if (filter->supportsOption(QtImageFilter::FilterBorderPolicy)) {
            QString borderPolicy;
            if (ui.rbExtend->isChecked()) borderPolicy = "Extend";
            else if (ui.rbMirror->isChecked()) borderPolicy = "Mirror";
            else borderPolicy = "Wrap";
            filter->setOption(QtImageFilter::FilterBorderPolicy, borderPolicy);
        }

        imgToFilter = filter->apply(imgToFilter);
        setCursor(Qt::ArrowCursor);
        setPixmapAndResize(QPixmap::fromImage(imgToFilter));
    }
}

void ImageToolWidget::setPixmapAndResize(const QPixmap &pixmap)
{
    ui.PixmapLabel->setPixmap(pixmap);
    ui.PixmapLabel->resize(pixmap.size());
}
