#ifndef SPECTRUMEDITOR_H
#define SPECTRUMEDITOR_H

#include "RangesData.h"

class MatrixWidget;
class RangeEditor;
class ParamWidget;


enum RangeType 
{
   RT_None   = -1,
   RT_Range  = 0,
   RT_Params = 1
};

class SpectrumEditor : public QGroupBox
{
Q_OBJECT
public:
   SpectrumEditor(QWidget * parent = 0);
   QString cardRange() const;
   void setCardRange(const QString & range);
protected:
   void setupUi();
   void initFavoriteMenu();
protected slots:
   void barToggled(QAction * act);
   void rangesDefined(const QList<Range *> & ranges);
   void rangeSelected();
   void noRanges();
protected:
   MatrixWidget * matrix_;
   RangeEditor * rangeEditor_;
   QMenu * favMenu_;
   QMap<QAction *, Range *> rangeMap_;
};

class RangeModel;

class RangeEditor : public QDialog
{
Q_OBJECT
public:
   RangeEditor(QWidget * parent = 0);
   QString currentMatrix() const;
signals:
   void rangeSelected(const QString & range);
   void rangesDefined(const QList<Range *> & ranges);
protected slots:
   void accept();
   void reject();
   void addRange();
   void delRange();
   void selPairs();
   void clearSel();
   void switchStacked(const QModelIndex &, const QModelIndex &);
   void fill(const Range * rng);
   void save(Range * rng);
   void changeCurrentName(const QString & name);
protected:
   void hideEvent(QHideEvent * ev);
   void updateMatrix();
protected:
   QAction * delAct_;
   QAction * pairsAct_;
   QAction * clearAct_;
   QComboBox * cmb_;
   QTreeView * tree_;
   QStackedWidget * stacked_;
   MatrixWidget * matrix_;
   ParamWidget * paramWidget_;
   RangeModel * rangeModel_;
};

class ParamWidget : public QWidget
{
   Q_OBJECT
public:
   ParamWidget(QWidget * parent);
   void setParam(const Range * rng);
   void getParam(Range * rng);
signals:
   void nameChanged(const QString & txt);
protected:
   QLineEdit * edtName_;
   QComboBox * cmbModif_;
   QComboBox * cmbKey_;
};

class RangeItem
{
public:
   RangeItem(const QString & data,
                RangeType type,
                RangeItem * parent = 0)
   {
      itemData = data; 
      parentItem = parent; 
      itemType = type; 
   }
   ~RangeItem() 
   { 
      qDeleteAll(childItems); 
   }

   void appendChild(RangeItem * child) { childItems.append(child); }
   void removeChild(RangeItem * child) { childItems.removeOne(child); }

   RangeItem *child(int row) { return childItems.value(row); }
   int childCount() const { return childItems.count(); }
    
   QString data() const {return itemData; } 
   void setData(const QString & data) { itemData = data; }

   int row() const
   {
      if (parentItem)
         return parentItem->childItems.indexOf(const_cast<RangeItem*>(this));
      return 0;
   }
    
   RangeItem *parent() { return parentItem; }
   RangeType type() const { return itemType; }
   void setType(RangeType type) { itemType = type; }

private:
   QList<RangeItem*> childItems;
   QString itemData;
   RangeType itemType;
   RangeItem *parentItem;
};

class RangeModel : public QAbstractItemModel
{
   Q_OBJECT
public:
   RangeModel(QObject *parent = 0);
   ~RangeModel();

   QModelIndex addRange();
   void delRange(const QModelIndex & idx);
   QVariant data(const QModelIndex & index, int role) const;
   void fill(const QModelIndex & index);
   void save(const QModelIndex & index);
   Range * range(const QModelIndex & index) const;
   void changeName(const QModelIndex & index, const QString & name);
   QList<Range *> ranges() const;
   int rowCount(const QModelIndex &parent = QModelIndex()) const;
   QModelIndex index(int row, int column,
                     const QModelIndex &parent = QModelIndex()) const;

signals:
   void addedIndex(const QModelIndex & idx);
   void needFill(const Range * rng);
   void needSave(Range * rng);
protected:   
   
   Qt::ItemFlags flags(const QModelIndex &index) const;

   QModelIndex parent(const QModelIndex &index) const;

   int columnCount(const QModelIndex &parent = QModelIndex()) const;

   QVariant headerData(int section, Qt::Orientation orientation, 
      int role = Qt::DisplayRole) const;
protected:
   RangeItem * getItem(const QModelIndex &index) const;
   void setupModelData(RangeItem * parent);
   RangeItem * rootItem;
   
   QList<Range *> rangeList_;
   QMap<RangeItem *, Range *> rangeMap_;
};

#endif
