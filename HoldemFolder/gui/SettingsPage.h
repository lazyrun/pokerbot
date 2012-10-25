#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

//#include <Phonon>

class SettingsPage : public QGroupBox
{
Q_OBJECT
public:
   SettingsPage(QWidget * parent = 0);
   QVariant value() const;
   void setValue(const QVariant & value);
protected slots:
   void testTurn();
   void testFold();
   void testCheck();
   void turnSwitch(bool);
   void foldSwitch(bool);
   void checkSwitch(bool);
protected:
   void setupUi();
protected:
   //Phonon::AudioOutput * audioOutput_;
   //Phonon::MediaObject * mediaObject_;
   
   QToolButton * btnTurn_;
   QToolButton * btnFold_;
   QToolButton * btnCheck_;
   QCheckBox * chkTurn_;
   QCheckBox * chkFold_;
   QCheckBox * chkCheck_;
   QCheckBox * chkKeep_;
   QCheckBox * chkVisual_;
   QCheckBox * chkShowFolded_;
   QCheckBox * chkAdvisor_;
   QDoubleSpinBox * spnInterval_;
   QComboBox * cmbLang_;
   QComboBox * cmbRoom_;
};

#endif
