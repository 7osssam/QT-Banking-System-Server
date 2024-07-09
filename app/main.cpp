#include <QCoreApplication>
#include <QDebug>
#include <QJsonObject>

#include "dbresult.h"

#include <QFile>
#include <QTextStream>

#include "server.h"
#include "db.h"

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

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

	// !
	QString error;
	DB::DatabaseManager::checkConnection(error);
	Server server;
	server.start(2222);

	return a.exec();
}
