/********************************************************************************
** Form generated from reading UI file 'imagetoolwidget.ui'
**
** Created: Thu 10. Nov 19:37:46 2011
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGETOOLWIDGET_H
#define UI_IMAGETOOLWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QPushButton *LoadButton;
    QPushButton *ReloadButton;
    QPushButton *FilterButton;
    QComboBox *FiltersCombo;
    QGroupBox *gbBorderPolicy;
    QVBoxLayout *vboxLayout1;
    QRadioButton *rbExtend;
    QRadioButton *rbMirror;
    QRadioButton *rbWrap;
    QGroupBox *gbChannels;
    QVBoxLayout *vboxLayout2;
    QCheckBox *ckRed;
    QCheckBox *ckGreen;
    QCheckBox *ckBlue;
    QCheckBox *ckAlpha;
    QGroupBox *gbMirror;
    QVBoxLayout *vboxLayout3;
    QCheckBox *ckHorizontal;
    QCheckBox *ckVertical;
    QSpacerItem *spacerItem;
    QLabel *PixmapLabel;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QString::fromUtf8("Form"));
        Form->resize(710, 478);
        hboxLayout = new QHBoxLayout(Form);
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        hboxLayout->setContentsMargins(9, 9, 9, 9);
#endif
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        vboxLayout = new QVBoxLayout();
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setContentsMargins(0, 0, 0, 0);
#endif
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        LoadButton = new QPushButton(Form);
        LoadButton->setObjectName(QString::fromUtf8("LoadButton"));

        vboxLayout->addWidget(LoadButton);

        ReloadButton = new QPushButton(Form);
        ReloadButton->setObjectName(QString::fromUtf8("ReloadButton"));

        vboxLayout->addWidget(ReloadButton);

        FilterButton = new QPushButton(Form);
        FilterButton->setObjectName(QString::fromUtf8("FilterButton"));

        vboxLayout->addWidget(FilterButton);

        FiltersCombo = new QComboBox(Form);
        FiltersCombo->setObjectName(QString::fromUtf8("FiltersCombo"));

        vboxLayout->addWidget(FiltersCombo);

        gbBorderPolicy = new QGroupBox(Form);
        gbBorderPolicy->setObjectName(QString::fromUtf8("gbBorderPolicy"));
        QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(1), static_cast<QSizePolicy::Policy>(5));
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(gbBorderPolicy->sizePolicy().hasHeightForWidth());
        gbBorderPolicy->setSizePolicy(sizePolicy);
        vboxLayout1 = new QVBoxLayout(gbBorderPolicy);
#ifndef Q_OS_MAC
        vboxLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout1->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        rbExtend = new QRadioButton(gbBorderPolicy);
        rbExtend->setObjectName(QString::fromUtf8("rbExtend"));
        rbExtend->setChecked(true);

        vboxLayout1->addWidget(rbExtend);

        rbMirror = new QRadioButton(gbBorderPolicy);
        rbMirror->setObjectName(QString::fromUtf8("rbMirror"));

        vboxLayout1->addWidget(rbMirror);

        rbWrap = new QRadioButton(gbBorderPolicy);
        rbWrap->setObjectName(QString::fromUtf8("rbWrap"));

        vboxLayout1->addWidget(rbWrap);


        vboxLayout->addWidget(gbBorderPolicy);

        gbChannels = new QGroupBox(Form);
        gbChannels->setObjectName(QString::fromUtf8("gbChannels"));
        sizePolicy.setHeightForWidth(gbChannels->sizePolicy().hasHeightForWidth());
        gbChannels->setSizePolicy(sizePolicy);
        vboxLayout2 = new QVBoxLayout(gbChannels);
#ifndef Q_OS_MAC
        vboxLayout2->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout2->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        ckRed = new QCheckBox(gbChannels);
        ckRed->setObjectName(QString::fromUtf8("ckRed"));
        ckRed->setChecked(true);

        vboxLayout2->addWidget(ckRed);

        ckGreen = new QCheckBox(gbChannels);
        ckGreen->setObjectName(QString::fromUtf8("ckGreen"));
        ckGreen->setChecked(true);

        vboxLayout2->addWidget(ckGreen);

        ckBlue = new QCheckBox(gbChannels);
        ckBlue->setObjectName(QString::fromUtf8("ckBlue"));
        ckBlue->setChecked(true);

        vboxLayout2->addWidget(ckBlue);

        ckAlpha = new QCheckBox(gbChannels);
        ckAlpha->setObjectName(QString::fromUtf8("ckAlpha"));

        vboxLayout2->addWidget(ckAlpha);


        vboxLayout->addWidget(gbChannels);

        gbMirror = new QGroupBox(Form);
        gbMirror->setObjectName(QString::fromUtf8("gbMirror"));
        vboxLayout3 = new QVBoxLayout(gbMirror);
#ifndef Q_OS_MAC
        vboxLayout3->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout3->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
        ckHorizontal = new QCheckBox(gbMirror);
        ckHorizontal->setObjectName(QString::fromUtf8("ckHorizontal"));
        ckHorizontal->setChecked(true);

        vboxLayout3->addWidget(ckHorizontal);

        ckVertical = new QCheckBox(gbMirror);
        ckVertical->setObjectName(QString::fromUtf8("ckVertical"));

        vboxLayout3->addWidget(ckVertical);


        vboxLayout->addWidget(gbMirror);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout->addItem(spacerItem);


        hboxLayout->addLayout(vboxLayout);

        PixmapLabel = new QLabel(Form);
        PixmapLabel->setObjectName(QString::fromUtf8("PixmapLabel"));
        QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(PixmapLabel->sizePolicy().hasHeightForWidth());
        PixmapLabel->setSizePolicy(sizePolicy1);
        PixmapLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        hboxLayout->addWidget(PixmapLabel);


        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Image Filter Tool", 0, QApplication::UnicodeUTF8));
        LoadButton->setText(QApplication::translate("Form", "Load", 0, QApplication::UnicodeUTF8));
        ReloadButton->setText(QApplication::translate("Form", "Reload", 0, QApplication::UnicodeUTF8));
        FilterButton->setText(QApplication::translate("Form", "Filter", 0, QApplication::UnicodeUTF8));
        gbBorderPolicy->setTitle(QApplication::translate("Form", "Border policy", 0, QApplication::UnicodeUTF8));
        rbExtend->setText(QApplication::translate("Form", "Extend", 0, QApplication::UnicodeUTF8));
        rbMirror->setText(QApplication::translate("Form", "Mirror", 0, QApplication::UnicodeUTF8));
        rbWrap->setText(QApplication::translate("Form", "Wrap", 0, QApplication::UnicodeUTF8));
        gbChannels->setTitle(QApplication::translate("Form", "Channels", 0, QApplication::UnicodeUTF8));
        ckRed->setText(QApplication::translate("Form", "Red", 0, QApplication::UnicodeUTF8));
        ckGreen->setText(QApplication::translate("Form", "Green", 0, QApplication::UnicodeUTF8));
        ckBlue->setText(QApplication::translate("Form", "Blue", 0, QApplication::UnicodeUTF8));
        ckAlpha->setText(QApplication::translate("Form", "Alpha", 0, QApplication::UnicodeUTF8));
        gbMirror->setTitle(QApplication::translate("Form", "Mirror options", 0, QApplication::UnicodeUTF8));
        ckHorizontal->setText(QApplication::translate("Form", "Horizontal", 0, QApplication::UnicodeUTF8));
        ckVertical->setText(QApplication::translate("Form", "Vertical", 0, QApplication::UnicodeUTF8));
        PixmapLabel->setText(QApplication::translate("Form", "Image...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGETOOLWIDGET_H
