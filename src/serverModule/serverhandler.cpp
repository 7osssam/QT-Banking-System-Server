#include "serverhandler.h"

ServerHandler::ServerHandler(QMutex* m, qintptr handle, QObject* parent) :
	QObject(parent), handle_(handle), clientSocket_(nullptr), Mutex_(m)
{
	qDebug() << "ServerHandler on thread: " << QThread::currentThread() << " created";

	requestManager = new RequestManager(Mutex_);
}

ServerHandler::~ServerHandler()
{
	qDebug() << "ServerHandler on thread: " << QThread::currentThread() << " destroyed";
	if (clientSocket_ != nullptr)
	{
		clientSocket_->deleteLater();
	}

	delete requestManager;
}

void ServerHandler::readyRead()
{
	QByteArray receivedData = clientSocket_->readAll();

	// Assuming the hash is appended at the end of the data
	const int  hashSize = QCryptographicHash::hash(QByteArray(), QCryptographicHash::Sha256).size();
	QByteArray requestData = receivedData.left(receivedData.size() - hashSize);
	QByteArray receivedHash = receivedData.right(hashSize);

	// Recompute hash of the request data
	QCryptographicHash hash(QCryptographicHash::Sha256);
	hash.addData(requestData);
	QByteArray computedHash = hash.result();

	// Verify the hash
	if (receivedHash != computedHash)
	{
		qCritical() << "Hash verification failed!";
		// Handle the failure (e.g., reject the request, log the error, etc.)

		return;
	}
	else
	{
		qDebug() << "Hash verification succeeded!";
	}

	// Proceed with handling the request
	QByteArray response = requestManager->makeRequest(requestData);

	clientSocket_->write(response);
	clientSocket_->waitForBytesWritten();
}

void ServerHandler::run()
{
	clientSocket_ = new QTcpSocket();

	QEventLoop loop;

	if (!clientSocket_->setSocketDescriptor(handle_))
	{
		qCritical() << "Failed to set socket descriptor: " << clientSocket_->errorString();
		delete clientSocket_;
		return;
	}

	qInfo() << "Serving serverhandler: " << handle_ << " on:  " << QThread::currentThread();

	connect(clientSocket_, &QTcpSocket::disconnected, &loop, &QEventLoop::quit, Qt::DirectConnection);
	connect(clientSocket_, &QTcpSocket::readyRead, this, &ServerHandler::readyRead, Qt::DirectConnection);

	loop.exec();
}