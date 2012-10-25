#include "MatrixWidget.h"
#include "SpectrumEditor.h"

SpectrumEditor::SpectrumEditor(QWidget * parent)
: QGroupBox(parent)
{
   qRegisterMetaType<Range>("Range");
   qRegisterMetaTypeStreamOperators<Range>("Range");

   setupUi();
}

void SpectrumEditor::setupUi()
{
   QVBoxLayout * vb = new QVBoxLayout;
   vb->setSizeConstraint(QLayout::SetMinAndMaxSize);

	QLabel * lblCaption = new QLabel(this);
	lblCaption->setMinimumHeight(30);
   lblCaption->setText("<h3><center>" + tr("Hole Cards Range") + "</center></h3>");
   lblCaption->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
   lblCaption->setStyleSheet("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #d9ebfb, stop: 0.4 #a1d4fb,stop: 0.5 #a1d4fb, stop: 1.0 #d9ebfb); background-position: left top; border-style: solid; border-width: 1px; border-color: silver; border-radius: 3px; color: black;");
   
   QLabel * lbl 
      = new QLabel(tr("Specify the playing hole cards "
                      "(all other cards will be folded):"), this);
   lbl->setContentsMargins(6, 6, 6, 6);
   lbl->setWordWrap(true);

   matrix_ = new MatrixWidget(this);
   
   vb->addWidget(lblCaption);
   vb->addWidget(lbl);
   
   QAction * actPair = new QAction(QIcon(":/images/pair.png"),
      tr("Select all pairs 22+"), this);
   actPair->setData("Pairs");

   QAction * actOften = new QAction(QIcon(":/images/care.png"),
      tr("Select the frequently used range"), this);
   actOften->setData("Often");

   QAction * actFavorite = new QAction(QIcon(":/images/favorite.png"),
      tr("Select a favorite range"), this);
   actFavorite->setData("Favorite");
   favMenu_ = new QMenu(this);
   actFavorite->setMenu(favMenu_);

   QFont fn = font();
   fn.setPointSize(10);
   favMenu_->setFont(fn);

   initFavoriteMenu();

   rangeEditor_ = new RangeEditor(this);
   //connect(rangeEditor_, SIGNAL(rangeSelected(const QString &)), 
   //   this, SLOT(rangeSelected(const QString &)));

   connect(rangeEditor_, SIGNAL(rangesDefined(const QList<Range *> &)), 
      this, SLOT(rangesDefined(const QList<Range *> &)));
   
   QAction * actEdit = new QAction(QIcon(":/images/edit.png"),
      tr("Edit favorite ranges"), this);
   actEdit->setData("Edit");

   QAction * actClear = new QAction(QIcon(":/images/clear.png"),
      tr("Clear selection"), this);
   actClear->setData("Clear");

   QToolBar * bar = new QToolBar(this);
   bar->setIconSize(QSize(32, 32));
   bar->setAllowedAreas(Qt::NoToolBarArea);
   bar->setMovable(false);
   bar->setFloatable(false);
   bar->setStyleSheet(".QToolBar{border: none;}");
   bar->setToolButtonStyle(Qt::ToolButtonIconOnly);
   bar->setOrientation(Qt::Vertical);
   
   bar->addAction(actPair);
   bar->addAction(actOften);
   bar->addAction(actFavorite);
   bar->addAction(actEdit);
   bar->addAction(actClear);
   connect(bar, SIGNAL(actionTriggered(QAction *)),
           this, SLOT(barToggled(QAction *)));
   
   QHBoxLayout * hb = new QHBoxLayout;
   hb->setMargin(0);
   hb->addWidget(matrix_);
   hb->addWidget(bar);
   vb->addLayout(hb);
   
   vb->addStretch(1);
   setLayout(vb);
}

void SpectrumEditor::initFavoriteMenu()
{
   //читаем данные с реестра
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Ranges");

   //если ветки нет - значит первый запуск 
   //и надо сгенрить демо диапазоны
   QSettings csettings(regKey, "Config");
   if (!csettings.contains("Settings"))
   {
      Range rngFL;
      rngFL.name = "Fixed Limit Strategy";
      rngFL.modifiers = "Shift";
      rngFL.hotkey = "F1";
      rngFL.matrix = "22+, AT+, KT+, QT+, JT+, A2s+, 98s, 87s, K9s, T9s";
      QVariant varRngFL = QVariant::fromValue<Range>(rngFL);
      settings.setValue("Range 0", varRngFL);

      Range rngNL;
      rngNL.name = "No Limit MSS Strategy";
      rngNL.modifiers = "Shift";
      rngNL.hotkey = "F2";
      rngNL.matrix = "77+, AT+, A9s, KQ+";
      QVariant varRngNL = QVariant::fromValue<Range>(rngNL);
      settings.setValue("Range 1", varRngNL);
   }
   
   QStringList keys = settings.allKeys();
   foreach (QString key, keys)
   {
      QVariant varRng = settings.value(key);
      Range vrng = qvariant_cast<Range>(varRng);

      Range * rng = new Range;
      rng->name = vrng.name;
      rng->modifiers = vrng.modifiers;
      rng->hotkey = vrng.hotkey;
      rng->matrix = vrng.matrix;

      QAction * act = favMenu_->addAction(rng->name, this, SLOT(rangeSelected()));
      if (!rng->modifiers.isEmpty() && !rng->hotkey.isEmpty())
         act->setShortcut(QKeySequence(rng->modifiers + "+" + rng->hotkey));
      rangeMap_[act] = rng;
   }
   if (keys.count() == 0)
   {
      favMenu_->addAction(tr("No Ranges"), this, SLOT(noRanges()));
   }

}

void SpectrumEditor::rangesDefined(const QList<Range *> & ranges)
{
   QList<QAction *> acts = favMenu_->actions();
   foreach (QAction * act, acts)
   {
      favMenu_->removeAction(act);
   }
   foreach (Range * rng, ranges)
   {
      QAction * act = favMenu_->addAction(rng->name, this, SLOT(rangeSelected()));
      if (!rng->modifiers.isEmpty() && !rng->hotkey.isEmpty())
         act->setShortcut(QKeySequence(rng->modifiers + "+" + rng->hotkey));
      rangeMap_[act] = rng;
   }
   if (ranges.count() == 0)
   {
      favMenu_->addAction(tr("No Ranges"), this, SLOT(noRanges()));
   }
}

void SpectrumEditor::noRanges()
{
   rangeEditor_->exec();
   if (rangeEditor_->result() == QDialog::Accepted)
   {
      QString cmatr = rangeEditor_->currentMatrix();
      matrix_->clear();
      matrix_->selectRange(cmatr);
   }
}

void SpectrumEditor::rangeSelected()
{
   QAction * act = qobject_cast<QAction *>(sender());
   if (act)
   {
      Range * rng = rangeMap_[act];
      matrix_->clear();
      matrix_->selectRange(rng->matrix);
   }
}

QString SpectrumEditor::cardRange() const
{
   return matrix_->cardRange();
}

void SpectrumEditor::setCardRange(const QString & range)
{
   matrix_->setCardRange(range);
}

void SpectrumEditor::barToggled(QAction * act)
{
   QString id = act->data().toString();
   if (id == "Pairs")
   {
      //select pairs
      matrix_->clear();
      matrix_->selectRange("22+");
   }
   else if (id == "Clear")
   {
      matrix_->clear();
   }
   else if (id == "Often")
   {
      matrix_->clear();
      matrix_->selectRange("22, 33, 44, 55, 66, 77, 88, 99, A7o, A7s, A8o, A8s, A9o, A9s, AA, AJo, AJs, AKo, AKs, AQo, AQs, ATo, ATs, JJ, JTo, JTs, KJo, KJs, KK, KQo, KQs, KTo, KTs, QJo, QJs, QQ, T9o, T9s, TT, 98, 87");
   }
   else if (id == "Edit")
   {
      rangeEditor_->exec();
      if (rangeEditor_->result() == QDialog::Accepted)
      {
         QString cmatr = rangeEditor_->currentMatrix();
         matrix_->clear();
         matrix_->selectRange(cmatr);
      }
   }
   else if (id == "Favorite")
   {
      QList<QWidget *> lw = act->associatedWidgets();
      foreach (QWidget * w, lw)
      {
         QToolButton * btn = qobject_cast<QToolButton * >(w);
         if (btn)
         {
            btn->showMenu();
            break;
         }
      }
   }
}

//
// RangeEditor
//
RangeEditor::RangeEditor(QWidget * parent)
: QDialog(parent, Qt::Dialog | Qt::WindowSystemMenuHint)
{
   setWindowTitle(tr("Range Editor"));
   QFont fn = font();
   fn.setPointSize(10);
   setFont(fn);

   QVBoxLayout * vb = new QVBoxLayout;
   QGroupBox * gbox = new QGroupBox(this);
   gbox->setLayout(vb);

   tree_ = new QTreeView(this);
   rangeModel_ = new RangeModel(this);

   tree_->setModel(rangeModel_);
   tree_->expandAll();

   connect(rangeModel_, SIGNAL(needFill(const Range *)),
      this, SLOT(fill(const Range *)));

   connect(rangeModel_, SIGNAL(needSave(Range *)),
      this, SLOT(save(Range *)));

   connect(tree_->selectionModel(), 
      SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
      this, SLOT(switchStacked(const QModelIndex &, const QModelIndex &)));

   QVBoxLayout * tvb = new QVBoxLayout;
   tvb->setMargin(3);
   QPushButton * btnActions = new QPushButton(tr("Range Actions"), this);
   QMenu * mnuActions = new QMenu(this);
   mnuActions->setFont(fn);
   mnuActions->addAction(tr("&Add New Range"), 
      this, SLOT(addRange()), QKeySequence("Ctrl+A"));
   delAct_ = mnuActions->addAction(tr("&Delete Current Range"), 
      this, SLOT(delRange()), QKeySequence("Ctrl+Del"));
   mnuActions->addSeparator();
   pairsAct_ = mnuActions->addAction(tr("&Select All Pairs"), 
      this, SLOT(selPairs()), QKeySequence("Ctrl+P"));
   clearAct_ = mnuActions->addAction(tr("&Clear Selection"), 
      this, SLOT(clearSel()), QKeySequence("Ctrl+C"));
   btnActions->setMenu(mnuActions);
   
   tvb->addWidget(tree_);
   tvb->addWidget(btnActions);

   matrix_  = new MatrixWidget(this);
   paramWidget_ = new ParamWidget(this);
   connect(paramWidget_, SIGNAL(nameChanged(const QString &)), 
      this, SLOT(changeCurrentName(const QString &)));

   stacked_ = new QStackedWidget(this);
   stacked_->addWidget(matrix_);
   stacked_->addWidget(paramWidget_);

   QHBoxLayout * hb = new QHBoxLayout;
   hb->addLayout(tvb);
   hb->addWidget(stacked_);

   vb->addLayout(hb);
   
   QDialogButtonBox * box = new QDialogButtonBox(this);
   box->addButton(" " + tr("&Apply") + " ", QDialogButtonBox::AcceptRole);
   box->addButton(" " + tr("&Close") + " ", QDialogButtonBox::RejectRole);
   connect(box, SIGNAL(accepted()), this, SLOT(accept()));
   connect(box, SIGNAL(rejected()), this, SLOT(reject()));

   QVBoxLayout * main = new QVBoxLayout;
   main->setSizeConstraint(QLayout::SetFixedSize);
   main->addWidget(gbox);
   main->addWidget(box);
   setLayout(main);
   
   tree_->setCurrentIndex(rangeModel_->index(0, 0));
   updateMatrix();
}

void RangeEditor::changeCurrentName(const QString & name)
{
   QModelIndex idx = tree_->currentIndex();
   rangeModel_->changeName(idx, name);
}

QString RangeEditor::currentMatrix() const
{
   return matrix_->cardRange();
}

void RangeEditor::accept()
{
   hide();
   //qApp->processEvents();
   //emit rangeSelected(cmb_->currentText());
   QDialog::accept();
}

void RangeEditor::reject()
{
   hide();
   //qApp->processEvents();
   QDialog::reject();
}

void RangeEditor::hideEvent(QHideEvent *)
{
   QModelIndex idx = tree_->currentIndex();
   rangeModel_->save(idx);
   //сохраняем изменения
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Ranges");
   settings.clear();
   
   //для каждого rng
   QList<Range *> ranges = rangeModel_->ranges();
   int cnt = 0;
   foreach (Range * rng, ranges)
   {
      QVariant varRng = QVariant::fromValue<Range>(*rng);
      settings.setValue(QString("Range %1").arg(cnt++), varRng);
   }
   emit rangesDefined(ranges);
}

void RangeEditor::fill(const Range * rng)
{
   matrix_->clear();
   matrix_->selectRange(rng->matrix);
   paramWidget_->setParam(rng);
}

void RangeEditor::save(Range * rng)
{
   if (!rng)
      return;
   rng->matrix = matrix_->cardRange();
   paramWidget_->getParam(rng);
}

void RangeEditor::switchStacked(const QModelIndex & curr, const QModelIndex & prev)
{
   //сохранить изменения в прежнем виде
   rangeModel_->save(prev);

   //заполнить данными новый вид
   rangeModel_->fill(curr);
   const Range * rng = rangeModel_->range(curr);

   paramWidget_->setParam(rng);
   int type = rangeModel_->data(curr, Qt::UserRole + 1).toInt();
   stacked_->setCurrentIndex(type);
}

void RangeEditor::addRange()
{
   QModelIndex idx = rangeModel_->addRange();
   tree_->setCurrentIndex(idx);
   tree_->expand(idx);
   updateMatrix();
}

void RangeEditor::delRange()
{
   QModelIndex idx = tree_->currentIndex();
   QModelIndex newidx = idx;
   if (idx.parent() != tree_->rootIndex())
   {
      newidx = idx.parent();
   }
   int crow = newidx.row();
   rangeModel_->delRange(idx);
   newidx = rangeModel_->index(crow - 1, 0, tree_->rootIndex());
   if (!newidx.isValid())
      newidx = rangeModel_->index(crow, 0, tree_->rootIndex());
   tree_->setCurrentIndex(newidx);
   tree_->expand(newidx);

   updateMatrix();
}

void RangeEditor::updateMatrix()
{
   int rc = rangeModel_->rowCount();
   if (rc == 0)
   {
      matrix_->clear();
      matrix_->setEnabled(false);
      //заблокировать кнопки
      delAct_->setEnabled(false);
      pairsAct_->setEnabled(false);
      clearAct_->setEnabled(false);
   }
   else
   {
      matrix_->setEnabled(true);
      delAct_->setEnabled(true);
      pairsAct_->setEnabled(true);
      clearAct_->setEnabled(true);
   }
}

void RangeEditor::selPairs()
{
   matrix_->selectRange("22+");
}

void RangeEditor::clearSel()
{
   matrix_->clear();
}

//
// RangeModel
//
RangeModel::RangeModel(QObject *parent)
   : QAbstractItemModel(parent)
{
   rootItem = new RangeItem("", RT_None);
   setupModelData(rootItem);
}

RangeModel::~RangeModel()
{
   if (rootItem != NULL) 
      delete rootItem;
}

int RangeModel::columnCount(const QModelIndex &) const
{
   return 1;
}

int RangeModel::rowCount(const QModelIndex &parent) const
{
    RangeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<RangeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void RangeModel::changeName(const QModelIndex & index, const QString & name)
{
   RangeItem * item = getItem(index);
   Range * rng = rangeMap_.value(item);
   rng->name = name;
   if (item->type() == RT_Params)
      item = item->parent();
   item->setData(name);
   emit dataChanged(index.parent(), index);
}

QVariant RangeModel::data(const QModelIndex &index, int role) const
{
   if (!index.isValid())
      return QVariant();

   if (role != Qt::DisplayRole    &&
       role != Qt::DecorationRole && 
       role != Qt::SizeHintRole && 
       role != Qt::FontRole && 
       role != Qt::UserRole + 1)
      return QVariant();

   if (role == Qt::UserRole + 1)
   {
      RangeItem *item = static_cast<RangeItem*>(index.internalPointer());
      return item->type();
   }

   if (role == Qt::FontRole)
   {
      QFont fn = qApp->font();
      fn.setPointSize(10);
      return fn;
   }

   if (role == Qt::DecorationRole)
   {
      RangeItem *item = static_cast<RangeItem*>(index.internalPointer());
      if (item->type() == RT_Range)
         return qVariantFromValue(QIcon(":/images/cards.png"));
      else if (item->type() == RT_Params)
         return qVariantFromValue(QIcon(":/images/pref.png"));
      
      return QVariant();
   }


   if (role == Qt::DisplayRole)
   {
      RangeItem *item = static_cast<RangeItem*>(index.internalPointer());
      return item->data();
   }
    
   if (role == Qt::SizeHintRole)
   {
      return QSize(200, 24);
   }

   return QVariant();
}

Qt::ItemFlags RangeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
   
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex RangeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    RangeItem * parentItem = getItem(parent);

    RangeItem * childItem = parentItem->child(row);

    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex RangeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    RangeItem *childItem = static_cast<RangeItem*>(index.internalPointer());
    RangeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}


RangeItem * RangeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) 
    {
        RangeItem *item = static_cast<RangeItem*>(index.internalPointer());
        if (item)
           return item;
    }
    return rootItem;
}

QVariant RangeModel::headerData(int section, Qt::Orientation orientation, 
   int role) const
{
   section;
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
   {
      return tr("Ranges");
   }
   
   if (role == Qt::TextAlignmentRole) 
   {
      return Qt::AlignCenter;   
   }

   if (role == Qt::FontRole) 
   {
      QFont fn = qApp->font();
      fn.setPointSize(10);
      return fn;
   }

   return QVariant();
}

void RangeModel::setupModelData(RangeItem * parent)
{
   parent;
   //читаем данные с реестра
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Ranges");
   
   QStringList keys = settings.allKeys();
   foreach (QString key, keys)
   {
      QVariant varRng = settings.value(key);
      Range vrng = qvariant_cast<Range>(varRng);

      Range * rng = new Range;
      rng->name = vrng.name;
      rng->modifiers = vrng.modifiers;
      rng->hotkey = vrng.hotkey;
      rng->matrix = vrng.matrix;

      RangeItem * newRange = new RangeItem(rng->name, RT_Range, rootItem);
      rootItem->appendChild(newRange);
      RangeItem * paramRange = new RangeItem(tr("Parameters"), RT_Params, newRange);
      newRange->appendChild(paramRange);
      
      rangeList_.append(rng);
      rangeMap_.insert(newRange,   rng);
      rangeMap_.insert(paramRange, rng);
   }
   
   emit layoutChanged();

   //RangeItem * daddy = parent;
   //
   ////первый родитель
   //Range * rngFL = new Range;
   //rngFL->name = "Fixed Limit";
   //rngFL->modifiers = "Ctrl+Shift";
   //rngFL->hotkey = "F1";
   //RangeItem * fl = new RangeItem(tr("Fixed Limit"), RT_Range, daddy);
   //daddy->appendChild(fl);
   //daddy = daddy->child(daddy->childCount() - 1);
   //RangeItem * flp = new RangeItem(tr("Parameters"), RT_Params, daddy);
   //daddy->appendChild(flp);
   //
   //Range * rngNL = new Range;
   //rngNL->name = "No Limit Thight Strategy";
   //rngNL->modifiers = "Ctrl+Shift";
   //rngNL->hotkey = "F1";
   //daddy = parent;
   //RangeItem * nl = new RangeItem(tr("No Limit Thight Strategy"), RT_Range, daddy);
   //daddy->appendChild(nl);
   //daddy = daddy->child(daddy->childCount() - 1);
   //RangeItem * nlp = new RangeItem(tr("Parameters"), RT_Params, daddy);
   //daddy->appendChild(nlp);

   //rangeList_.append(rngFL);
   //rangeList_.append(rngNL);
   //rangeMap_.insert(fl, rngFL);
   //rangeMap_.insert(flp, rngFL);
   //rangeMap_.insert(nl, rngNL);
   //rangeMap_.insert(nlp, rngNL);
}

QModelIndex RangeModel::addRange()
{
   Range * rng = new Range;
   rng->name = tr("New Range");
   rng->modifiers = "";
   rng->hotkey = "";

   RangeItem * newRange = new RangeItem(tr("New Range"), RT_Range, rootItem);
   rootItem->appendChild(newRange);
   RangeItem * paramRange = new RangeItem(tr("Parameters"), RT_Params, newRange);
   newRange->appendChild(paramRange);
   
   rangeList_.append(rng);
   rangeMap_.insert(newRange,   rng);
   rangeMap_.insert(paramRange, rng);

   emit layoutChanged();
   QModelIndex idx = index(rootItem->childCount() - 1, 0);
   emit addedIndex(idx);
   return idx;
}

void RangeModel::delRange(const QModelIndex & idx)
{
   RangeItem * item = getItem(idx);
   Range * rng = rangeMap_.value(item);
   rangeList_.removeOne(rng);

   if (item->type() == RT_Params)
      item = item->parent();
   rootItem->removeChild(item);
   emit layoutChanged();
}

void RangeModel::fill(const QModelIndex &index)
{
   if (!index.isValid())
      return;

   RangeItem * item = getItem(index);
   Range * rng = rangeMap_.value(item);
   emit needFill(rng);
}

void RangeModel::save(const QModelIndex &index)
{
   if (!index.isValid())
      return;
   RangeItem * item = getItem(index);
   Range * rng = rangeMap_.value(item);
   emit needSave(rng);
}

Range * RangeModel::range(const QModelIndex & index) const
{
   RangeItem * item = getItem(index);
   Range * rng = rangeMap_.value(item);
   return rng;
}

QList<Range *> RangeModel::ranges() const
{
   return rangeList_;
}

//
//ParamWidget
//
ParamWidget::ParamWidget(QWidget * parent)
: QWidget(parent)
{
   QVBoxLayout * vb = new QVBoxLayout;
   vb->setContentsMargins(0, 12, 6, 6);
   setLayout(vb);
   vb->setSpacing(3);
   QLabel * lblName = new QLabel(tr("Range &Name:"), this);
   edtName_ = new QLineEdit(this);
   lblName->setBuddy(edtName_);
   connect(edtName_, SIGNAL(textChanged(const QString &)),
      SIGNAL(nameChanged(const QString &)));
   
   QLabel * lblHotKey = new QLabel(tr("Assign a global &hotkey for this range:"), this);
   QHBoxLayout * hb = new QHBoxLayout;
   hb->setMargin(0);
   cmbModif_ = new QComboBox(this);
   cmbKey_ = new QComboBox(this);
   cmbModif_->addItem("");
   cmbModif_->addItem("Shift");
   cmbModif_->addItem("Ctrl");
   cmbModif_->addItem("Ctrl+Alt");
   cmbModif_->addItem("Alt+Shift");
   cmbModif_->addItem("Ctrl+Shift");

   cmbKey_->addItem("");
   cmbKey_->addItem("F1");
   cmbKey_->addItem("F2");
   cmbKey_->addItem("F3");
   cmbKey_->addItem("F4");
   cmbKey_->addItem("F5");
   cmbKey_->addItem("F6");
   cmbKey_->addItem("F7");
   cmbKey_->addItem("F8");
   cmbKey_->addItem("F9");
   cmbKey_->addItem("F10");
   cmbKey_->addItem("F11");
   cmbKey_->addItem("F12");
   cmbModif_->setCurrentIndex(0);
   cmbKey_->setCurrentIndex(0);

   cmbModif_->setMinimumWidth(100);
   cmbKey_->setMinimumWidth(100);

   lblHotKey->setBuddy(cmbModif_);
   hb->addWidget(cmbModif_);
   hb->addWidget(cmbKey_);
   hb->addStretch(1);

   vb->addWidget(lblName);
   vb->addWidget(edtName_);
   vb->addSpacerItem(new QSpacerItem(1, 6));
   vb->addWidget(lblHotKey);
   vb->addLayout(hb);

   vb->addStretch(1);
}

void ParamWidget::setParam(const Range * rng)
{
   if (!rng)
      return;
   edtName_->setText(rng->name);
   int idxModif = cmbModif_->findText(rng->modifiers);
   cmbModif_->setCurrentIndex(idxModif);
   int idxKey = cmbKey_->findText(rng->hotkey);
   cmbKey_->setCurrentIndex(idxKey);
}

void ParamWidget::getParam(Range * rng)
{
   rng->name = edtName_->text();
   rng->modifiers = cmbModif_->currentText();
   rng->hotkey = cmbKey_->currentText();
}
