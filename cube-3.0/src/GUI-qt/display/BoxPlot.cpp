/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "BoxPlot.h"
#include "Pretty.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

#include <QFontMetrics>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QDialog>
#include <QString>
#include <QPen>
#include <QDebug>
#include <math.h>

using namespace std;

namespace
{
    class HeightCalculator
    {
        public:
            HeightCalculator(int offset, int height, double max, double min)
                : lowOffset(offset), clientHeight(height),
                  maxValue(max), minValue(min)
            {}
            int operator()(double val) const
            {
                return static_cast<int>(lowOffset - clientHeight /
                                (maxValue - minValue) * (Clip(val) - minValue));
            }
            enum Position {Inside, Above, Below};
            Position CalculatePosition(double value) const
            {
                if(value > maxValue)
                {
                    return Above;
                }
                if(value < minValue)
                {
                    return Below;
                }
                return Inside;
            }
            double Clip(double value) const
            {
                if(value > maxValue)
                {
                    return maxValue;
                }
                if(value < minValue)
                {
                    return minValue;
                }
                return value;
            }
        private:
            int lowOffset, clientHeight;
            double maxValue, minValue;
    };

   void ReadWhitespaces(istream &theFile)
   {
      char check = theFile.peek();
      while(check == ' ' || check == '\n')
      {
         theFile.ignore();
         check = theFile.peek();
      }
   }

   QString doubleLegendToString(double val) {
      QString labelText;
      if (val > 999 || val < 1e-2)
         labelText =  QString::number(val, 'e', 2);
      else
         labelText =  QString::number(val, 'f', 2);
      return labelText;
   }
}

Item::Item(std::string const &theName, int theCount,
    double theSum, double theMean, double theMin,
    double theQ25, double theMedian, double theQ75, double theMax,
    double theVariance)
    : name(theName), sum(theSum), mean(theMean), min(theMin), q25(theQ25),
      median(theMedian), q75(theQ75), max(theMax), variance(theVariance),
      count(theCount), statInformation(count >= 5), valid_values( true ), data_status_description()
{ 
    valid_values = isStatisticalInformationValid();
}

Item::Item(ifstream &theFile)
    : sum(0.0), mean(0.0), min(0.0), q25(0.0), median(0.0), q75(0.0), max(0.0),
      statInformation(false), valid_values( true ), data_status_description()
{
    theFile >> name;
    theFile >> count;
    ReadWhitespaces(theFile);
    if(theFile.peek() != '-')
    {
        theFile >> mean;
        theFile >> median;
        theFile >> min;
        theFile >> max;
        theFile >> sum;
        if(count >= 2)
        {
            theFile >> variance;
        }
        if(count >= 5)
        {
            theFile >> q25;
            theFile >> q75;
            statInformation = true;
        }
    }
    valid_values = isStatisticalInformationValid();
}

QStringList Item::Print(PrecisionWidget *prec) const
{
    return Statistics::StatisticalInformation(count, sum, mean, min,
                             q25, median, q75, max, variance).Print(name, prec);
}


/**
 * Performs a sequence of checks, whether statistical data looks meaningfull or not.
 */
bool
Item::isStatisticalInformationValid()
{
    data_status_description.clear();
    bool _return = true;
    if ( min > max )
    {
        data_status_description.append( "Min value is larger than Max." );
        _return = false;                   // the world is up side down
    }
    if ( mean > max && mean < min )
    {
        data_status_description.append( "Mean value is out of range [min, max]." );
        _return = false;
    } // mean is out of range
    if ( median > max && median < min )
    {
        data_status_description.append( "Median value is out of range [min, max]." );
        _return = false;
    } // median is out of range
    if ( max * count  < sum )
    {
        data_status_description.append( "Sum is larger than \"count * max\"." );
        _return = false;
    } // sum is bigger that count times max
    if ( ( min * count )  > sum )
    {
        data_status_description.append( "Sum is smaller than \"count * min\"." );
        _return = false;
    } // sum is lower that count times min
    
    if ( count >= 2)
    {
	if ( variance < 0 )
	{
	    data_status_description.append( "Variance is negative." );
	    _return = false;
	} // varianvve cannot be negativ
	if ( count >= 5 )
	{
	    if ( q25 > q75 )
	    {
		data_status_description.append( "25% quantile is larger than 75% quantile." );
		_return = false;
	    } // quantile 25% is bigger than 75%
	    if ( median > q75 )
	    {
		data_status_description.append( "Medium (50% quantile) is larger than 75% quantile." );
		_return = false;
	    } // definition of medium : 50% and it cannot be bigger than 75%
	    if ( median < q25 )
	    {
		data_status_description.append( "Medium (50% quantile) is smaller than 25% quantile." );
		_return = false;
	    } // definition of medium : 50% and it cannot be smaller  than 25%
	}
    }
    return _return;
}




BoxPlot::BoxPlot(QWidget *parent, PrecisionWidget *precision)
    : QWidget(parent), firstPosition(0,0), prec(precision)
{
   ResetMinMaxValues();
   statisticInfo = 0;
   start = 0.;
   borderPad = 10;
   tickWidth = 3;
   //setWindowTitle("BoxPlot");
   resize(300,450);
   if(parent != 0)
   {
      parent->adjustSize();
   }
}

void BoxPlot::Add(Item const &theItem)
{
    items.push_back(theItem);
    resize(150 + 80 * items.size(), 450);
    if(parentWidget() != 0)
    {
        parentWidget()->adjustSize();
    }
    ResetView();
}

/**
  set the data of a boxplot which consists only of one item
  existing items will be deleted
  @param theItem values of the boxplot
  @param abs the corresponding absoulte values
*/
void BoxPlot::Set(Item const &theItem,  Item const &absItem, double startValue)
{
   start = startValue;
   items.clear();
   items.push_back(theItem);
   absolute = absItem;
   ResetView();
}

void BoxPlot::Reset()
{
    items.clear();
    ResetView();
}

void BoxPlot::Load(string const &fileName)
{
    ifstream theFile(fileName.c_str());
    string buffer;
    getline(theFile, buffer);

    while(!theFile.eof() && !theFile.fail())
    {
        Item theItem(theFile);
        if(theItem.statInformation)
        {
            items.push_back(theItem);
        }
        ReadWhitespaces(theFile);
        char check = theFile.peek();
        while(check == '-' && !theFile.eof())
        {
            getline(theFile, buffer);
            ReadWhitespaces(theFile);
            check = theFile.peek();
        }
    }
    ResetMinMaxValues();
}

void BoxPlot::ResetMinMaxValues()
{
   minimumValue = start;
   if(items.size() == 0)
   {
      maximumValue = 0.0;
   }
   else
   {
      double maxVal = numeric_limits<double>::min();
      for(vector<Item>::const_iterator it = items.begin(); it != items.end(); ++it)
      {
         if(it->max > maxVal)
         {
            maxVal = it->max;
         }
      }
      maximumValue = maxVal;
   }
   AdjustMinMaxValues();
}

void BoxPlot::AdjustMinMaxValues()
{
    vector<double> values;
    Pretty(minimumValue, maximumValue, numberOfAxisEntries, values);
    minimumValue = *values.begin();
    maximumValue = *values.rbegin();
}

void BoxPlot::ResetView()
{
    ResetMinMaxValues();
    update();
}

//Calculates the value on the y-axis which belongs to a height of koord pixels on the axis
double BoxPlot::GetValue(int koord) const
{
    int lowOffset = GetCanvasHeight() - GetLowerBorderHeight();
    int clientHeight = lowOffset - GetUpperBorderHeight();
    return (lowOffset - koord) * (GetMaxValue() - GetMinValue())
                               / clientHeight + GetMinValue();
}

void BoxPlot::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (items.size() == 0) {
       painter.drawText(rect(), Qt::AlignCenter, "No values to display.");
       return;
    }
    if (fabs(maximumValue - minimumValue) < 1e-8) {
       maximumValue += 1.0; // create valid box
    }

   DrawLeftLegend(painter);
   DrawLowerLegend(painter);
   DrawRightLegend(painter);
   DrawBoxPlots(painter);
   painter.drawRect(GetLeftBorderWidth(), GetUpperBorderHeight(),
                    GetCanvasWidth() - GetRightBorderWidth() - GetLeftBorderWidth(),
                    GetCanvasHeight() - GetLowerBorderHeight() - GetUpperBorderHeight());
}

void BoxPlot::mousePressEvent(QMouseEvent *event)
{
    if(items.size() == 0)
    {
        return;
    }
    else
    {
        firstPosition.first = event->x();
        firstPosition.second = event->y();

        if (event->button() == Qt::RightButton) {
           Item const *clicked = GetClickedItem(event);
           if(clicked != 0) {
              ShowStatisticsTooltip(clicked);
           }
        }
     }
 }

void BoxPlot::mouseReleaseEvent(QMouseEvent *event)
{
    switch(event->button())
    {
        case Qt::LeftButton:
            if(items.size() == 0)
            {
                return;
            }
            else
            {
                double newMaxVal, newMinVal;
                if(firstPosition.second < event->y())
                {
                    newMaxVal = GetValue(firstPosition.second);
                    newMinVal = GetValue(event->y());
                }
                else
                {
                    newMinVal = GetValue(firstPosition.second);
                    newMaxVal = GetValue(event->y());
                }
                if(event->y()-firstPosition.second < 10)
                {
                    Item const *clicked = GetClickedItem(event);
                    if(clicked != 0) {
                        ShowStatistics(clicked);
                    }
                }
                else
                {
                    SetMinMaxValues(newMinVal, newMaxVal);
                }
            }
            break;
        case Qt::MidButton:
            ResetView();
            break;
        default:
            break;
    }
    if (statisticInfo) statisticInfo->close();
}

void BoxPlot::SetMinMaxValues(double newMinVal, double newMaxVal)
{
    if(newMaxVal < maximumValue)
    {
        maximumValue = newMaxVal;
    }
    if(newMinVal > minimumValue)
    {
        minimumValue = newMinVal;
    }
    AdjustMinMaxValues();
    update();
}

int BoxPlot::GetCanvasWidth() const
{
    return width();
}

int BoxPlot::GetCanvasHeight() const
{
    return height();
}

int BoxPlot::GetTextWidth(QString const &text) const
{
    QFontMetrics fm(font());
    return fm.width(text);
}

int BoxPlot::GetTextWidth(string const &text) const
{
    return GetTextWidth(QString::fromStdString(text));
}

int BoxPlot::GetTextHeight(QString const &) const
{
    QFontMetrics fm(font());
    return 3 * fm.ascent() / 2;
}

int BoxPlot::GetTextHeight(string const &text) const
{
    return GetTextHeight(QString::fromStdString(text));
}

int BoxPlot::GetLeftBorderWidth() const
{
   vector<double> values;
   Pretty(GetMinValue(), GetMaxValue(), numberOfAxisEntries, values);
   int maxWidth = 0;
   for(vector<double>::const_iterator it = values.begin(); it != values.end(); ++it)
   {
      maxWidth = max(maxWidth, GetTextWidth(QString::number(*it, 'g', 5)));
   }
   return maxWidth + 3*tickWidth + borderPad;
}

/**
  calculate height neede for DrawLowerLegend
  */
int BoxPlot::GetLowerBorderHeight() const
{
   if (items.size() == 1) { // only one boxplot item
      if (items.at(0).name.length() > 0) // has label
         return GetTextHeight(items.at(0).name);
      else
         return GetTextHeight(QString("|"));
   }

    string longestName;
    {
        unsigned int longestLength = 0;
        for(vector<Item>::const_iterator it = items.begin(); it != items.end(); ++it)
        {
            if(it->name.length() > longestLength)
            {
                longestLength = it->name.length();
                longestName = it->name;
            }
        }
    }
    int textHeight = GetTextHeight(longestName);
    int clientWidth = GetCanvasWidth() - GetRightBorderWidth()
                                       - GetLeftBorderWidth();
    int textWidth = GetTextWidth("00:" + longestName);
    int numberOfColumns = clientWidth / textWidth;
    int numberOfRows = (numberOfColumns == 0
	              ? 2
                      : (items.size() + numberOfColumns - 1) / numberOfColumns);
    return (2 + 2*numberOfRows) * textHeight;
}

int BoxPlot::GetUpperBorderHeight() const
{
    return GetCanvasHeight() / 20;
}

int BoxPlot::GetRightBorderWidth() const
{
   if (absolute.count == 0) // no right legend
      return GetCanvasWidth() / 20;
   else {
      int maxWidth = 0;
      double values[]= {absolute.median, absolute.mean, absolute.q25, absolute.q75, absolute.max, absolute.min};
      for(int i=0; i<6; i++)
      {
         maxWidth = max(maxWidth, GetTextWidth(doubleLegendToString(values[i])));
      }
      maxWidth += 3*tickWidth + borderPad;
      return maxWidth;
   }
}

double BoxPlot::GetMinValue() const
{
    return minimumValue;
}

double BoxPlot::GetMaxValue() const
{
    return maximumValue;
}

/** draws right legend with absolute values, if absolute values are set */
void BoxPlot::DrawRightLegend(QPainter &painter) {
   if (absolute.count == 0) return;

   int x = GetCanvasWidth() - GetRightBorderWidth();
   QList<int> oldPos;
   DrawMark(painter, x, items.at(0).median, absolute.median, oldPos);
   DrawMark(painter, x, items.at(0).mean,   absolute.mean, oldPos);
   DrawMark(painter, x, items.at(0).q25,    absolute.q25, oldPos);
   DrawMark(painter, x, items.at(0).q75,    absolute.q75, oldPos);
   DrawMark(painter, x, items.at(0).max,    absolute.max, oldPos);
   DrawMark(painter, x, items.at(0).min,    absolute.min, oldPos);
}

/** draws one right tick and corresponding label
    @param x x-position of the tick
    @param value current value which defindes the y position
    @param label absolute value corresponding to value, used as label text
    @param oldPos list of previous positions to avoid collisions
  */
void BoxPlot::DrawMark(QPainter &painter, int x, double value, double label, QList<int> &oldPos)
{
   int clientHeight = GetCanvasHeight() - GetLowerBorderHeight()
                      - GetUpperBorderHeight();

   int y = static_cast<int>(GetCanvasHeight() - GetLowerBorderHeight()
                            - clientHeight/(maximumValue - minimumValue) * (value - minimumValue));
   int h = GetTextHeight(QString("8"));

   for (int i=0; i<oldPos.size(); ++i) {
      if ((y+h > oldPos[i]) && (y < oldPos[i]+h)) { // collision with previous label => do not draw
         return;
      }
   }
   oldPos.append(y);

   painter.drawLine(x, y, x+tickWidth, y);
   painter.drawText(x + 3*tickWidth, y - h/3,
                    100, h, Qt::AlignLeft, doubleLegendToString(label));
}

void BoxPlot::DrawLeftLegend(QPainter &painter)
{
    int upperOffset = GetUpperBorderHeight();
    int clientHeight = GetCanvasHeight() - GetLowerBorderHeight()
                                         - upperOffset;
    int clientWidth = GetLeftBorderWidth();

    vector<double> values;
    Pretty(GetMinValue(), GetMaxValue(), numberOfAxisEntries, values);
    double minValue = *values.begin();
    double maxValue = *values.rbegin();

    int textHeight = GetTextHeight(QString("8"));

   for(vector<double>::const_iterator it = values.begin(); it != values.end(); ++it)
   {
      int currYPos = static_cast<int>(clientHeight + upperOffset
                                      - clientHeight/(maxValue - minValue) * (*it - minValue));
      painter.drawLine(clientWidth, currYPos, clientWidth-tickWidth, currYPos);
      QPen oldPen = painter.pen();
      QPen dashedPen(Qt::DashLine);
      dashedPen.setColor(Qt::cyan);
      painter.setPen(dashedPen);
      painter.drawLine(clientWidth, currYPos,
                       GetCanvasWidth() - GetRightBorderWidth(), currYPos);
      painter.setPen(oldPen);
      painter.drawText(0, currYPos - textHeight/3, clientWidth - 3*tickWidth,
                       textHeight, Qt::AlignRight,
                       QString::number(*it, 'g', 5));
   }
}

void BoxPlot::DrawLowerLegend(QPainter &painter)
{
    int leftOffset = GetLeftBorderWidth();
    int clientWidth = GetCanvasWidth() - GetRightBorderWidth() - leftOffset;
    int clientHeight = GetLowerBorderHeight();
    string longestName;
    {
        unsigned int longestLength = 0;
        for(vector<Item>::const_iterator it = items.begin(); it != items.end(); ++it)
        {
            if(it->name.length() > longestLength)
            {
                longestLength = it->name.length();
                longestName= it->name;
            }
        }
    }
    int textWidth = GetTextWidth("00:" + longestName) * 4 / 3;

    int numberOfEntries = items.size();
    int numberOfColumns = clientWidth / textWidth;
    if(numberOfColumns == 0)
    {
        return;
    }
    int numberOfRows = (numberOfEntries + numberOfColumns - 1) / numberOfColumns;

    int legendHeight = 2 * GetTextHeight(QString("1"));
    int descriptionHeight = clientHeight - legendHeight;

    int i = 0;
    for(vector<Item>::const_iterator it = items.begin(); it != items.end(); ++it, ++i)
    {
        int currXOffset = leftOffset + clientWidth / (numberOfEntries + 1) * (i + 1);
        /* no legend if only one item exist */
        if (items.size() > 1) {
           painter.drawLine(currXOffset, GetCanvasHeight() - clientHeight,
                            currXOffset, GetCanvasHeight() - clientHeight + legendHeight/4);
           painter.drawText(currXOffset - GetTextWidth(QString::number(i+1))/3,
                            GetCanvasHeight() - clientHeight + legendHeight*2/3,
                            QString::number(i+1));
        }
        /* no legend if text if empty */
        if (it->name.length() > 0) {
           painter.drawText(leftOffset + (i % numberOfColumns) * textWidth,
                            GetCanvasHeight() - descriptionHeight
                            + descriptionHeight/(3*numberOfRows/2+1)
                            * (1+3*(i/numberOfColumns))/2,
                            QString::number(i+1) + ": " + it->name.c_str());
        }
    }
}

Item const* BoxPlot::GetClickedItem(QMouseEvent *event) const
{
    if (items.size() == 1)
       return &(items[0]);

    int x = event->x();
    int numberOfEntries = items.size();
    int leftOffset = GetLeftBorderWidth();
    int clientWidth = GetCanvasWidth() - GetRightBorderWidth() - leftOffset;
    int boxWidth = 2 * clientWidth / (3 * (numberOfEntries + 1));

    int index = (x - leftOffset - boxWidth/2) / (clientWidth / (numberOfEntries + 1));
    int middle = leftOffset + clientWidth / (numberOfEntries + 1) * (index + 1);
    if(x >= middle - boxWidth/2 && x <= middle + boxWidth/2)
    {
        if(index >= 0 && index < numberOfEntries)
        {
            return &(items[index]);
        }
    }
    return 0;
}

void BoxPlot::ShowStatisticsTooltip(Item const *item)
{
    if (prec == 0) prec = new PrecisionWidget(0);
    if (statisticInfo != 0) {
       statisticInfo->close();
       delete statisticInfo; statisticInfo=0;
    }
    statisticInfo = new StatisticInfo(item->Print(prec), 0, true);
    statisticInfo->setWindowFlags(Qt::ToolTip); // create seperate window
    QPoint localMouse(firstPosition.first, firstPosition.second); //QCursor::pos());
    QPoint mousePos = this->mapToGlobal(localMouse);
    statisticInfo->move(mousePos);
    statisticInfo->show();
 }

void BoxPlot::ShowStatistics(Item const *item)
{
    if (prec == 0) prec = new PrecisionWidget(0);
    QDialog *dialog = new QDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle("Statistics info");
    StatisticInfo *statisticInfo = new StatisticInfo(item->Print(prec), dialog);

    QVBoxLayout * layout = new QVBoxLayout();
    layout->addWidget(statisticInfo);

    QWidget *buttons = new QWidget(this);
    QHBoxLayout *bLayout = new QHBoxLayout();
    buttons->setLayout(bLayout);

    bLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    QPushButton *clip = new QPushButton("To Clipboard", dialog);
    connect(clip, SIGNAL(pressed()), statisticInfo, SLOT(toClipboard()));
    bLayout->addWidget(clip);
    QPushButton *okButton = new QPushButton("Close", dialog);
    connect(okButton, SIGNAL(pressed()), dialog, SLOT(accept()));
    bLayout->addWidget(okButton);
    layout->addWidget(buttons);

    dialog->setLayout(layout);

    dialog->setModal(false);
    dialog->show();
    //dialog.exec();
}

void BoxPlot::DrawBoxPlots(QPainter &painter)
{
    int numberOfEntries = items.size();

    int leftOffset = GetLeftBorderWidth();
    int clientWidth = GetCanvasWidth() - GetRightBorderWidth() - leftOffset;
    int lowOffset = GetCanvasHeight() - GetLowerBorderHeight();
    int clientHeight = lowOffset - GetUpperBorderHeight();
    double maxValue = GetMaxValue();
    double minValue = GetMinValue();
    int boxWidth = 2 * clientWidth / (3 * (numberOfEntries + 1));

    HeightCalculator valueModifier(lowOffset, clientHeight, maxValue, minValue);
    HeightCalculator const &CalculateHeight = valueModifier;

    int i = 0;
    for(vector<Item>::const_iterator it = items.begin(); it != items.end(); ++it, ++i)
    {
        int currWidth = leftOffset + clientWidth / (numberOfEntries + 1) * (i + 1);

        //Draw the horizontal lines for minimum and maximum
        QPen oldPen = painter.pen();
        QPen thickPen;
        thickPen.setWidth(2);
        painter.setPen(thickPen);
        if(valueModifier.CalculatePosition(it->min) == HeightCalculator::Inside)
        {
            painter.drawLine(currWidth - boxWidth/4, CalculateHeight(it->min),
                             currWidth + boxWidth/4, CalculateHeight(it->min));
        }
        if(valueModifier.CalculatePosition(it->max) == HeightCalculator::Inside)
        {
            painter.drawLine(currWidth - boxWidth/4, CalculateHeight(it->max),
                             currWidth + boxWidth/4, CalculateHeight(it->max));
        }
        painter.setPen(oldPen);

        //if there are enough values to have Q75 and Q25
        if(it->count >= 5)
        {
            //Draw the connection lines between max/min and Q75/Q25
            painter.setPen(Qt::DashLine);
            if(valueModifier.CalculatePosition(it->max) != valueModifier.CalculatePosition(it->q75)
               || valueModifier.CalculatePosition(it->max) == HeightCalculator::Inside)
            {
                painter.drawLine(currWidth, CalculateHeight(it->max), currWidth,
                                 CalculateHeight(it->q75));
            }
            if(valueModifier.CalculatePosition(it->min) != valueModifier.CalculatePosition(it->q25)
               || valueModifier.CalculatePosition(it->min) == HeightCalculator::Inside)
            {
                painter.drawLine(currWidth, CalculateHeight(it->min), currWidth,
                                 CalculateHeight(it->q25));
            }
            painter.setPen(Qt::SolidLine);

            //Draw the box
            if(valueModifier.CalculatePosition(it->q75) != valueModifier.CalculatePosition(it->q25)
               || valueModifier.CalculatePosition(it->q75) == HeightCalculator::Inside)
            {
                QRect myBox(currWidth - boxWidth/2, CalculateHeight(it->q75),
                          boxWidth,
                          CalculateHeight(it->q25) - CalculateHeight(it->q75));
                painter.fillRect(myBox, QBrush(Qt::white));
                painter.drawRect(myBox);
            }
        }
        else
        {
            //Just draw a connection between min and max and no box
            painter.setPen(Qt::DashLine);
            if(valueModifier.CalculatePosition(it->max) != valueModifier.CalculatePosition(it->min)
              || valueModifier.CalculatePosition(it->max) == HeightCalculator::Inside)
            {
                painter.drawLine(currWidth, CalculateHeight(it->max), currWidth,
                                 CalculateHeight(it->min));
            }
            painter.setPen(Qt::SolidLine);
        }

        //Draw the median line
        if(valueModifier.CalculatePosition(it->median) == HeightCalculator::Inside)
        {
            thickPen.setWidth(3);
            painter.setPen(thickPen);
            painter.drawLine(currWidth - boxWidth/2, CalculateHeight(it->median),
                             currWidth + boxWidth/2, CalculateHeight(it->median));
            painter.setPen(oldPen);
        }

        //Draw the mean line
        if(valueModifier.CalculatePosition(it->mean) == HeightCalculator::Inside)
        {
            painter.setPen(Qt::DashLine);
            painter.drawLine(currWidth - boxWidth/2, CalculateHeight(it->mean),
                             currWidth + boxWidth/2, CalculateHeight(it->mean));
            painter.setPen(Qt::SolidLine);
        }
	// values are invalid. Paint error message
        if ( !it->valid_values )
        {
            QPen redPen;
            redPen.setColor( Qt::red );
            painter.setPen( redPen );

            QRect rect = painter.boundingRect( 0, 0, 0, 0, Qt::AlignLeft, "W" ); // get a hight of the line

            for ( int i = 0; i < it->data_status_description.size(); i++ )
            {
                painter.drawText(
                    GetLeftBorderWidth(), GetUpperBorderHeight() +  +( rect.height() + 3 ) * ( i + 1 ),
                    it->data_status_description.at( i )  );
            }
            painter.setPen( oldPen );
        }
    }
}

/*int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    BoxPlot theBoxPlot;
    theBoxPlot.Load("test.stat");

    theBoxPlot.show();

    return app.exec();
}*/

