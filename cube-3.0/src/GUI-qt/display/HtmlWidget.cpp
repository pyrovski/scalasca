/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <iostream>
#include <QBuffer>

#include "HtmlWidget.h"
#include "DownloadThread.h"

using namespace std;


/*--- Function prototypes (helper functions) ------------------------------*/

namespace
{

bool download(const QUrl& url, QBuffer& buffer);

}   /* unnamed namespace */


/*--- Constructors & destructor -------------------------------------------*/

/**
 * @brief Constructs an empty instance
 *
 * Constructs a new, empty HtmlWidget instance width the given @p parent.
 *
 * @param  parent  Parent widget
 */
HtmlWidget::HtmlWidget(QWidget* parent)
  : QTextBrowser(parent)
{
}


/*--- Loading of contents -------------------------------------------------*/

/**
 * @brief Loads and displays the document with the given URL
 *
 * This method tries to load the document with the given @p url. On success,
 * it returns @b true and displays the contents of the document within the
 * widget. Otherwise, @p false is returned.
 *
 * @param  url  URL of the document to be displayed
 *
 * @return @p true if successful, @b false otherwise.
 */
bool HtmlWidget::loadUrl(const QUrl& url)
{
  // Clear the document & reset base URL
  clear();
  m_base.clear();

  // Sanity check
  if (!url.isValid())
    return false;

  // Try loading the document
  QBuffer buffer;
  if (!download(url, buffer))
    return false;

  // Update base URL
  m_base = url;
  m_base.setFragment(QString());

  // Display document
  setHtml(QString(buffer.data()));

  return true;
}


/**
 * @brief Loads referenced resources
 *
 * This method is called whenever an additional resource (e.g., an image or
 * an CSS style sheet) ir referenced from the document. It reimplements the
 * implementation provided by Qt's QTextBrowser class.
 *
 * @param  type  Type of the resource
 * @param  name  Resource URL
 *
 * @return Resource data as QByteArray (encapsulated in a QVariant) on
 *         success, an empty QVariant object otherwise.
 */
QVariant HtmlWidget::loadResource(int type, const QUrl& name)
{
  Q_UNUSED(type);

  // Sanity check
  if (!name.isValid()) {
    cerr << "Invalid URL: " << name.toString().toStdString() << endl;
    return QVariant();
  }

  // Determine absolute URL
  QUrl absoluteUrl;
  if (name.isRelative())
    absoluteUrl = m_base.resolved(name);
  else
    absoluteUrl = name;

  // Try downloading the resource data
  QBuffer buffer;
  if (!download(absoluteUrl, buffer)) {
    cerr << "Error loading resource: " << absoluteUrl.toString().toStdString()
         << endl;
    return QVariant();
  }

  // Return the data
  return QVariant(buffer.data());
}


/*--- Retrieving document information -------------------------------------*/

/**
 * @brief Returns the base URL of the current document
 *
 * Returns the base URL of the current document.
 *
 * @return Base URL
 */
QUrl HtmlWidget::baseUrl() const
{
  return m_base;
}


/*--- Helper functions ----------------------------------------------------*/

namespace
{

/**
 * @internal
 * @brief Download data from a given URL into a memory buffer
 *
 * This helper function downloads the data referenced by the given @p url
 * into the provided memory @p buffer.
 *
 * @param  url     URL of the data to be downloaded
 * @param  buffer  Memory buffer to store the data
 *
 * @return @p true if successful, @b false otherwise.
 */
bool download(const QUrl& url, QBuffer& buffer)
{
  // Start download thread
  DownloadThread thread(url, buffer);
  thread.start();
  thread.wait();

  return !thread.error();
}

}   /* unnamed namespace */
