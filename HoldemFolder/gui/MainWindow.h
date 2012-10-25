#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class SpectrumEditor;
class SettingsPage;

class MainWindow : public QDialog
{
   Q_OBJECT
public:
   MainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);

public slots:
   void handleMessage(const QString & msg);
   void setSize();
protected slots:
   void listToggled(QAction * act);
   
protected:
   //Функции
   void setMainWidget();
   void showEvent(QShowEvent *);
   void closeEvent(QCloseEvent *);
protected slots:   
   void saveConf();
   void loadConf();
   void cancel();
   void unlock();
   void startPlay();
protected:
   SpectrumEditor * spectrum_;
   SettingsPage * settingsPage_;
   QActionGroup * actionGroup_;
   QToolBar * listBar_;
   bool firstShow_;
   QStackedWidget * stacked_;
   QSettings * settings_;
   QPushButton * btnSave_;
   QTranslator * appTranslator_;
   QString addr_;
   QString addr_ru_;
};

#endif

