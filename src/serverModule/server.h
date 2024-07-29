/**
 * @file server.h
 *
 * @brief This file contains the declaration of the Server class, which extends QTcpServer to handle incoming network connections.
 * @image html ServerModule.svg
 */

#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QAbstractSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDate>
#include <QTime>
#include <QList>
#include <QThreadPool>
#include <QThread>
#include <QMutex>

#include "serverhandler.h"

/**
 * @brief The Server class extends QTcpServer to handle incoming network connections.
 *
 * This class sets up a TCP server, manages incoming connections using a thread pool, and handles
 * incoming requests with instances of the ServerHandler class. It supports multithreaded
 * processing and provides methods to start and stop the server.
 */
class Server : public QTcpServer
{
	Q_OBJECT
public:
	/**
     * @brief Constructs a Server object with an optional parent.
     *
     * Initializes the thread pool and mutex, and sets the maximum thread count for handling
     * incoming connections.
     *
     * @param parent The parent QObject.
	 * @image html Server_init.svg
     */
	explicit Server(QObject* parent = nullptr);

	/**
     * @brief Destructor for the Server class.
     *
     * Cleans up resources by clearing and deleting the thread pool.
     */
	~Server();

signals:
	// No signals defined for this class

public slots:
	/**
     * @brief Starts the server and listens for incoming connections on the specified port.
     *
     * @param port The port number to listen on (default is 2222).
	 * @image html Server_start_stop.svg
     */
	void start(qint16 port = 2222);

	/**
     * @brief Stops the server by closing the listening socket.
     */
	void quit();

protected:
	/**
     * @brief Handles incoming connections by creating and starting a ServerHandler instance.
     *
     * @param handle The socket descriptor for the incoming connection.
	 * @image html Server_incomingConnection.svg
     */
	void incomingConnection(qintptr handle) override;

private:
	QThreadPool* threadPool; ///< Pointer to the thread pool used for handling connections.
	QMutex*		 Mutex_;	 ///< Mutex for synchronizing access to shared resources.
};

#endif						 // SERVER_H
