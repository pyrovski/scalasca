/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _HELPBROWSER_H
#define _HELPBROWSER_H


#include <QDialog>


/*-------------------------------------------------------------------------*/
/**
 * @file  HelpBrowser.h
 * @brief Declaration of the class HelpBrowser
 *
 * This header file provides the declaration of the class HelpBrowser.
 */
/*-------------------------------------------------------------------------*/


/*--- Forward declarations ------------------------------------------------*/

class HtmlWidget;
class QUrl;


/*-------------------------------------------------------------------------*/
/**
 * @class HelpBrowser
 * @brief Provides a simple dialog window for displaying HTML help texts
 *
 * This class provides a simple non-modal dialog window for displaying
 * HTML help texts. The displayed text (including referenced image data)
 * will be cached until either a new document is loaded, in which case its
 * data will be cached, or the instance is destroyed (closing the dialog
 * will only hide it).
 */
/*-------------------------------------------------------------------------*/

class HelpBrowser : public QDialog
{
  Q_OBJECT

  public:
    /// @name Constructors &amp; destructor
    /// @{

    HelpBrowser(const QString& title, QWidget* parent=0);

    /// @}


  public slots:
    /// @name Displaying a help text
    /// @{

    bool showUrl(const QUrl& url);

    /// @}


  private:
    /// Browser widget used to display the help text
    HtmlWidget* m_html;
};


#endif
