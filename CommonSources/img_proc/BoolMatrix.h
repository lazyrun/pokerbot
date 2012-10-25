#ifndef BOOLMATRIX_H
#define BOOLMATRIX_H

typedef QList<QPoint> Border;

class BoolMatrix
{
public:
   BoolMatrix();
   //! копирующий конструктор
   BoolMatrix(const BoolMatrix & rhs);

   //! ћатрица сразу режетс€ по серому
   BoolMatrix(const QImage & img, uchar threshold);
   //! ѕодаетс€ черно бела€ картинка
   BoolMatrix(const QImage * img);
   //!
   BoolMatrix(const QImage & img, uchar grayThresh, uint minBright, uint maxBright);
   //! матрица создаетс€ но не заполн€етс€
   BoolMatrix(const QImage & img);
   //! матрица создаетс€ из строки
   BoolMatrix(const QString & str);
   //!
   ~BoolMatrix();

   QString toString() const;

   int operator-(const BoolMatrix & rhs);
   BoolMatrix & operator=(const BoolMatrix & rhs);

   int width()  const { return width_;  }
   int height() const { return height_; }

   //! «адать значение
   void set(int x, int y, bool value);
   //! ѕолучить значение
   bool at(int x, int y) const;
   //! бела€ точка
   bool isWhite(int x, int y) const;
   //! черна€ точка
   bool isBlack(int x, int y) const;
   //! залить область
   void fillRect(const QRect & rect, bool color);
   //! ѕоиск наиболее белой области
   /*!
   \param dim –азмерность области
   \return  оординаты области
   */
   QRect findMaxWhiteRect(int dim_x, int dim_y = -1) const;
   
   //! ѕоиск вертикальных границ
   /*!
   \param minSize ћинимальный размер границы
   \return —писок границ длины не меньше чем минимальна€
   */
   QList<Border> verticalBorders(int minSize) const;

   //! ѕоиск горизонтальных границ
   /*!
   \param minSize ћинимальный размер границы
   \return —писок границ длины не меньше чем минимальна€
   */
   QList<Border> horizontalBorders(int minSize) const;

   //!
   int minDiff(const BoolMatrix & rhs);
protected:
   int distForLeftBlack(const QPoint & pt) const;
protected:
   bool ** matrix_;
   bool * bits_;
   int width_;
   int height_;
};

#endif
