/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _HTMLWIDGET_H
#define _HTMLWIDGET_H


#include <QTextBrowser>


/*-------------------------------------------------------------------------*/
/**
 * @file  HtmlWidget.h
 * @brief Declaration of the class HtmlWidget
 *
 * This header file provides the declaration of the class HtmlWidget.
 */
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/**
 * @class HtmlWidget
 * @brief Provides a simple HTML-browser widget
 *
 * This class provides a simple HTML-browser widget based on Qt's
 * QTextBrowser class. Its main extension over QTextBrowser is the
 * ability to load HTML pages as well as referenced resources (i.e.,
 * images in particular) directly from a web server or a file,
 * respectively.
 *
 * This widget is not intended to be used as a full-featured web
 * browser, but more as an easy way to display online help messages
 * in HTML format. When writing the documents to be displayed, keep
 * in mind that this widget only supports a limited subset of HTML
 * (see Qt's reference documentation for details).
 */
/*-------------------------------------------------------------------------*/

class HtmlWidget : public QTextBrowser
{
  Q_OBJECT

  public:
    /// @name Constructors &amp; destructor
    /// @{

    HtmlWidget(QWidget* parent=0);

    /// @}
    /// @name Loading of contents
    /// @{

    virtual bool     loadUrl(const QUrl& url);
    virtual QVariant loadResource(int type, const QUrl& name);

    /// @}
    /// @name Retrieving document information
    /// @{

    QUrl baseUrl() const;

    /// @}


  private:
    /// Base URL of the current document, used to resolve relative URLs
    QUrl m_base;
};


#endif   /* !_HTMLWIDGET_H */
