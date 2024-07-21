#include <QCoreApplication>
#include <QDebug>
#include <QJsonObject>

#include "server.h"
#include "db.h"
#include "logger.h"

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	qInstallMessageHandler(Logger::myMessageOutput);

	Logger::instance()->startLogging();

	DB::DatabaseManager::setDbSetting("aws-0-us-east-1.pooler.supabase.com", 6543, "postgres.ajfrcwmgtitxbvcnhzwn",
									  "z7HQs^*#HnmqQ3m2", "postgres");

	DB::DatabaseManager* db = DB::DatabaseManager::createInstance();

	if (db == nullptr)
	{
		qCritical() << "Failed to create database instance";
	}
	else
	{
		qInfo() << "Database instance created";
	}

	QString error;
	DB::DatabaseManager::checkConnection(error);

	if (!error.isEmpty())
	{
		qCritical() << error;
	}
	else
	{
		qInfo() << "Database connection established";
	}

	Server server;
	server.start(2222);

	return a.exec();
}
