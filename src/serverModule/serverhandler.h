#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H

#include <QEventLoop>
#include <QObject>
#include <QDebug>
#include <QRunnable>
#include <QThreadPool>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QByteArray>
#include <QMutex>
#include "RequestManager.h"
#include <QTcpServer>

/**
 * @brief The ServerHandler class handles incoming client connections in a separate thread.
 *
 * This class is responsible for managing communication with a client, verifying data integrity using
 * cryptographic hashes, and processing requests through the RequestManager. It operates in a separate
 * thread to ensure that the server remains responsive.
 */
class ServerHandler : public QObject, public QRunnable
{
	Q_OBJECT
public:
	/**
     * @brief Constructs a ServerHandler object with the specified parameters.
     *
     * Initializes the ServerHandler with a mutex, socket handle, and optional parent QObject.
     *
     * @param m Pointer to a QMutex used for synchronization.
     * @param handle The socket descriptor for the incoming connection.
     * @param parent The parent QObject.
     */
	explicit ServerHandler(QMutex* m, qintptr handle = 0, QObject* parent = nullptr);

	/**
     * @brief Destructor for the ServerHandler class.
     *
     * Cleans up resources by deleting the client socket and RequestManager.
     */
	~ServerHandler();

	/**
     * @brief Slot to handle readyRead signal from QTcpSocket.
     *
     * Reads data from the client socket, verifies its integrity using a cryptographic hash, and
     * processes the request using RequestManager. Sends the response back to the client.
     */
	void readyRead();

	/**
     * @brief Implements QRunnable's run method to handle the client connection.
     *
     * Sets up the client socket, connects signals for reading data and disconnection, and starts
     * the event loop to process client requests.
     */
	void run();
signals:
	// No signals defined for this class

public slots:
	// No slots defined for this class

private:
	qintptr			handle_;		///< The socket descriptor for the incoming connection.
	QTcpSocket*		clientSocket_;	///< Pointer to the QTcpSocket used for client communication.
	QMutex*			Mutex_;			///< Pointer to the mutex used for synchronization.
	RequestManager* requestManager; ///< Pointer to the RequestManager for processing requests.
};

#endif								// SERVERHANDLER_H
