#ifndef CLIENT_H
#define CLIENT_H

#include <QEventLoop>

#include <QObject>
#include <QDebug>
#include <QRunnable>
#include <QThreadPool>
#include <QThread>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

#include <QMutex>

#include "db.h"
#include "dbresult.h"
#include "RequestManager.h"

class Client : public QObject, public QRunnable
{
	Q_OBJECT
public:
	explicit Client(QMutex* m, qintptr handle = 0, QObject* parent = nullptr) :
		QObject(parent), handle_(handle), clientSocket_(nullptr), Mutex_(m)
	{
		qDebug() << "Client on thread: " << QThread::currentThread() << " created";

		requestManager = new RequestManager(Mutex_);
	}
	~Client()
	{
		qDebug() << "Client on thread: " << QThread::currentThread() << " destroyed";
		if (clientSocket_ != nullptr)
		{
			//clientSocket_->close();
			clientSocket_->deleteLater();
		}

		delete requestManager;
	}

	void readyRead()
	{
		QByteArray data = clientSocket_->readAll();

		// syncronus call that reaches the DataBase
		QByteArray l_response = requestManager->makeRequest(data);

		clientSocket_->write(l_response);
		clientSocket_->waitForBytesWritten();
	}

	void run() override
	{
		clientSocket_ = new QTcpSocket(); //!

		QEventLoop loop;

		if (!clientSocket_->setSocketDescriptor(handle_))
		{
			qCritical() << "Failed to set socket descriptor: " << clientSocket_->errorString();
			delete clientSocket_;
			return;
		}

		qInfo() << "Serving client: " << handle_ << " on:  " << QThread::currentThread();

		connect(clientSocket_, &QTcpSocket::disconnected, &loop, &QEventLoop::quit, Qt::DirectConnection);

		connect(clientSocket_, &QTcpSocket::readyRead, this, &Client::readyRead, Qt::DirectConnection);

		loop.exec();
	}

signals:

public slots:

private:
	qintptr			handle_;
	QTcpSocket*		clientSocket_;
	QMutex*			Mutex_;
	RequestManager* requestManager;
};

#endif // CLIENT_H
