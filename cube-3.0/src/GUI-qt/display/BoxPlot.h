/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef BOXPLOT_H
#define BOXPLOT_H

#include <QWidget>
#include <QPainter>
#include <QString>

#include <utility>
#include <vector>
#include <string>
#include "PrecisionWidget.h"

struct Item
{
    Item(std::string const &theName, int theCount,
         double theSum, double theMean, double theMin,
         double theQ25, double theMedian, double theQ75,
         double theMax, double theVariance);
    Item(std::ifstream &theFile);
    Item() {count = 0;}
    QStringList Print(PrecisionWidget *prec) const;
    std::string name;
    double sum, mean, min, q25, median, q75, max, variance;
    int count;
    bool statInformation;
    bool        valid_values;
    QStringList data_status_description;
    bool isStatisticalInformationValid();
};

#include "Statistics.h"
#include "StatisticInfo.h"

class BoxPlot : public QWidget
{
    public:
        BoxPlot(QWidget *parent, PrecisionWidget *precision);
        void Add(Item const &theItem);
        void Set(Item const &theItem);
        void Set(Item const &theItem, Item const &absItem, double start);
        void Load(std::string const &fileName);
        void Reset();
        void ResetView();
        void SetMinMaxValues(double newMinVal, double newMaxVal);

   private:
        virtual void paintEvent(QPaintEvent *event);
        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseReleaseEvent(QMouseEvent *event);

        double GetValue(int koord) const;
        void DrawLeftLegend(QPainter &painter);
        void DrawRightLegend(QPainter &painter);
        void DrawLowerLegend(QPainter &painter);
        void ResetMinMaxValues();
        void AdjustMinMaxValues();
        void DrawBoxPlots(QPainter &painter);
        int GetCanvasWidth() const;
        int GetCanvasHeight() const;
        int GetLeftBorderWidth() const;
        int GetLowerBorderHeight() const;
        int GetUpperBorderHeight() const;
        int GetRightBorderWidth() const;
        int GetTextWidth(std::string const &theText) const;
        int GetTextHeight(std::string const &theText) const;
        int GetTextWidth(QString const &theText) const;
        int GetTextHeight(QString const &theText) const;
        Item const* GetClickedItem(QMouseEvent *event) const;
        void ShowStatisticsTooltip(Item const *item);
        void ShowStatistics(Item const *item);
        double GetMinValue() const;
        double GetMaxValue() const;
        void DrawMark(QPainter &painter, int x, double value, double label, QList<int> &oldPos);

        int tickWidth; // width of the label ticks
        int borderPad; // padding around the boxplots
        double start;  // the minimum value, the boxplot starts with (default 0.0)
        static int const digits = 6;
        static int const numberOfAxisEntries = 5;
        std::vector<Item> items;
        Item absolute;
        double minimumValue, maximumValue;
        std::pair<int, int> firstPosition;
        PrecisionWidget *prec;
        StatisticInfo *statisticInfo;
};

#endif

