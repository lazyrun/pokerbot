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
#include <QtGui>
#include <QtCore/QVector>
#include "qtimagefilter.h"
#include "ui_imagetoolwidget.h"

class ImageToolWidget : public QWidget {
    Q_OBJECT
    public:
        ImageToolWidget(QWidget *parent = 0);
        virtual ~ImageToolWidget();
    public slots:
        void loadImage();
        void reloadImage();
        void filterImage();
        void filterIndexChanged(int index);

    private:
        void setPixmapAndResize(const QPixmap &pixmap);
        QString m_currentFilename;
        QVector<QtImageFilter*>  m_imageFilters;
        Ui::Form ui;

};

