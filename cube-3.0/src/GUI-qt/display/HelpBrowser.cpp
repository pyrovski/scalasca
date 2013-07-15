/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <QVBoxLayout>

#include "HelpBrowser.h"
#include "HtmlWidget.h"


/*--- Constructors & destructor -------------------------------------------*/

/**
 * @brief Constructs a new help browser dialog
 *
 * This constructor creates a new help browser dialog with the given window
 * @p title and @p parent widget.
 *
 * @param  title   Window title
 * @param  parent  Parent widget
 */
HelpBrowser::HelpBrowser(const QString& title, QWidget* parent)
  : QDialog(parent)
{
  // Dialog defaults
  setWindowTitle(title);
  resize(600, 600);

  // Set up layout manager
  QVBoxLayout* layout = new QVBoxLayout();
  setLayout(layout);

  // Create HTML display
  m_html = new HtmlWidget(this);
  layout->addWidget(m_html);
}


/*--- Displaying a help text ----------------------------------------------*/

/**
 * @brief Displays the help text with the given URL
 *
 * This method displays the help text with the given @p url. If the base
 * URL of @p url is identical to the one of the current document, this
 * method will only scroll to the new anchor (if provided). Otherwise, the
 * new document will be loaded.
 *
 * @param  url  URL of the help text to be shown
 *
 * @return @b true if successful, @b false otherwise (e.g., in case of a
 *         download error).
 */
bool HelpBrowser::showUrl(const QUrl& url)
{
  bool success = true;

  // New base? ==> download data
  QUrl base(url);
  base.setFragment(QString());
  if (base != m_html->baseUrl())
    success = m_html->loadUrl(url);

  if (success) {
    QString anchor = url.fragment();
    if (!anchor.isEmpty())
      m_html->scrollToAnchor(anchor);
    setWindowTitle(m_html->documentTitle());
    show();
  }

  return success;
}
