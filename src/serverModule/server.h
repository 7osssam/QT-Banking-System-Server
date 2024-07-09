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

#include "client.h"

class Server : public QTcpServer
{
	Q_OBJECT
public:
	explicit Server(QObject* parent = nullptr) :
		QTcpServer(parent), threadPool(new QThreadPool(this)), Mutex_(new QMutex)
	{
		threadPool->setMaxThreadCount(10);
		qDebug() << "ThreadPool: " << threadPool->maxThreadCount();

		// set object name
		this->setObjectName("Server");
	}

	~Server()
	{
		qInfo() << this->objectName() << "Destructor";

		threadPool->clear();
		threadPool->deleteLater();
	}

signals:

public slots:
	void start(qint16 port = 9876)
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
	void quit()
	{
		this->close();
		qInfo() << "Server Stopped!";
	}

protected:
	void incomingConnection(qintptr handle) override
	{
		qDebug() << "Incoming Connection: " << QThread::currentThread();

		Client* client = new Client(Mutex_, handle, this);
		client->setAutoDelete(true);

		threadPool->start(client);
	}

private:
	QThreadPool* threadPool;
	QMutex*		 Mutex_;
};

#endif // SERVER_H
