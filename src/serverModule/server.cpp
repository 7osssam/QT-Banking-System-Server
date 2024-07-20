#include "server.h"

Server::Server(QObject* parent) : QTcpServer(parent), threadPool(new QThreadPool(this)), Mutex_(new QMutex)
{
	threadPool->setMaxThreadCount(10);
	qDebug() << "ThreadPool: " << threadPool->maxThreadCount();

	// Set object name for easy identification
	this->setObjectName("Server");
}

Server::~Server()
{
	qInfo() << this->objectName() << "Destructor";

	threadPool->clear();
	threadPool->deleteLater();
}

void Server::start(qint16 port)
{
	qDebug() << "Server::start: " << QThread::currentThread();
	if (!this->listen(QHostAddress::Any, port))
	{
		qCritical() << "Failed to start server";
		qDebug() << "Error: " << this->errorString();

		return;
	}
	else
	{
		qInfo() << "Server started on:" << port << " at " << QDate::currentDate().toString() << " "
				<< QTime::currentTime().toString();
	}
}

void Server::quit()
{
	this->close();
	qInfo() << "Server Stopped!";
}

void Server::incomingConnection(qintptr handle)
{
	qDebug() << "Incoming Connection: " << QThread::currentThread();

	ServerHandler* serverhandler = new ServerHandler(Mutex_, handle, this);
	serverhandler->setAutoDelete(true);

	threadPool->start(serverhandler);
}