/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <QBuffer>
#include <QFile>
#include <QHttp>
#include <QHttpResponseHeader>

#include "DownloadThread.h"


/*--- Constructors & destructor -------------------------------------------*/

/**
 * @brief Constructs a thread object for downloading data
 *
 * Constructs a new thread object for downloading the data referenced by
 * the given @p url into the given memory @þ buffer. To actually start the
 * downloading process, the thread has to be started using the run()
 * method.
 *
 * @param  url     URL of the data to be downloaded
 * @param  buffer  Memory buffer to store the data
 */
DownloadThread::DownloadThread(const QUrl& url, QBuffer& buffer)
  : m_url(url), m_buffer(&buffer), m_error(true)
{
}


/**
 * @brief Starts the download thread
 *
 * Calling this method actually starts the thread downloading the data
 * specified by the URL given to the constructor. The thread will
 * terminate once either the download process is finished or an error
 * occurred during the download.
 */
void DownloadThread::run()
{
  // Assume that there will be an error (simplifies returns)
  m_error = true;

  // Sanity check
  if (!m_url.isValid())
    return;

  // Load data depending on the specified protocol
  QString protocol = m_url.scheme();
  if ("http" == protocol)
  {
    QHttp http;

    // Catch various signals
    connect(&http, SIGNAL(done(bool)), this, SLOT(downloadFinished(bool)),
            Qt::DirectConnection);
    connect(&http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)),
            this, SLOT(checkResponse(const QHttpResponseHeader&)),
            Qt::DirectConnection);

    // Set up HTTP connection and start thread
    http.setHost(m_url.host(), m_url.port() != -1 ? m_url.port() : 80);
    http.get(m_url.path(), m_buffer);
    exec();

    // Disconnect signals
    disconnect(&http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)),
               this, SLOT(checkResponse(const QHttpResponseHeader&)));
    disconnect(&http, SIGNAL(done(bool)), this, SLOT(downloadFinished(bool)));
  }
  else if ("file" == protocol)
  {
    // Open file for reading
    QFile file(m_url.path());
    if (!file.open(QIODevice::ReadOnly))
      return;

    // Read data & reset error flag
    m_buffer->setData(file.readAll());
    m_error = false;
  }
}


/*--- Retrieving thread information ---------------------------------------*/

/**
 * @brief Returns the error flag
 *
 * Returns @b true if an error occurred during the download and @b false
 * otherwise.
 *
 * @return Error flag
 */
bool DownloadThread::error() const
{
  return m_error;
}


/*--- HTTP-related slots --------------------------------------------------*/

/**
 * @brief Called when a HTTP header of a server response is available
 *
 * This method is called whenever a HTTP header of a server response is
 * available. It basically checks whether the document is available or
 * some error (e.g., "Not found") occurred.
 *
 * @param  resp  HTTP response header
 */
void DownloadThread::checkResponse(const QHttpResponseHeader& resp)
{
  m_error = (resp.statusCode() != 200);
}


/**
 * @brief Called when a HTTP download is finished.
 *
 * This method is called whenever a HTTP download finishes, either due to
 * an error or after all data has been downloaded. The thread's error flag
 * will be set to the given flag.
 *
 * @param  error  Error flag
 */
void DownloadThread::downloadFinished(bool error)
{
  // Set error flag
  if (!m_error)
    m_error = error;

  // Signal thread termination
  emit quit();
}
