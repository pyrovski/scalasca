/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _DOWNLOADTHREAD_H
#define _DOWNLOADTHREAD_H


#include <QThread>
#include <QUrl>


/*-------------------------------------------------------------------------*/
/**
 * @file  DownloadThread.h
 * @brief Declaration of the class DownloadThread
 *
 * This header file provides the declaration of the class DownloadThread.
 */
/*-------------------------------------------------------------------------*/


/*--- Forward declarations ------------------------------------------------*/

class QBuffer;
class QHttpResponseHeader;

/*-------------------------------------------------------------------------*/
/**
 * @class DownloadThread
 * @brief Provides a thread class capable of downloading data
 *
 * The class DownloadThread provides the capability of downloading data
 * via the HTTP and FILE protocols into a memory buffer in the background.
 */
/*-------------------------------------------------------------------------*/

class DownloadThread : public QThread
{
  Q_OBJECT

  public:
    /// @name Constructors &amp; destructor
    /// @{

    DownloadThread(const QUrl& url, QBuffer& buffer);

    /// @}
    /// @name Thread control
    /// @{

    void run();

    /// @}
    /// @name Retrieving thread information
    /// @{

    bool error() const;

    /// @}


  private slots:
    /// @name HTTP-related slots
    /// @{

    void checkResponse(const QHttpResponseHeader& resp);
    void downloadFinished(bool error);

    /// @}


  private:
    /// URL to download via this thread
    QUrl m_url;

    /// Memory buffer for storing the data
    QBuffer* m_buffer;

    /// Stores an error flag for the download
    bool m_error;
};


#endif
