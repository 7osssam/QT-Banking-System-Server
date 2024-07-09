#ifndef SETUP_H
#define SETUP_H

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include <QByteArray>

#include <QMutex>
#include <gtest/gtest.h>

#include "RequestManager.h"
#include "db.h"
#include "dbresult.h"

class handleGetAccountNumberTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		// Initialize QCoreApplication with dummy arguments
		static int	 argc = 1;
		static char* argv[] = {(char*)"test"};
		app_ = new QCoreApplication(argc, argv);

		DB::DatabaseManager::setDbSetting("aws-0-us-east-1.pooler.supabase.com", 6543, "postgres.yflugnxlcmfwjczwzspw",
										  "dpd2q2*5BULBCz$J", "postgres");

		dbManager_ = DB::DatabaseManager::createInstance();

		// Initialize any necessary objects or setup before each test.
		mutex_ = new QMutex();
		handler_ = new RequestManager(mutex_);

		// Ensure dbManager is initialized and connected
		if (dbManager_ == nullptr)
		{
			qFatal("Failed to initialize database manager.");
			//qDebug() << "Failed to initialize or connect to database.";
		}

		// Set up the temporary PostgreSQL database
		setupDatabase();
	}

	void TearDown() override
	{
		// Clean up any objects created in SetUp.
		delete handler_;
		delete mutex_;

		// Clean up the database
		cleanupDatabase();
	}

	void CreateTables()
	{
		dbManager_->exec("CREATE TABLE IF NOT EXISTS users ("
						 "id SERIAL PRIMARY KEY,"
						 "first_name VARCHAR(50) NOT NULL,"
						 "last_name VARCHAR(50) NOT NULL,"
						 "email VARCHAR(255) UNIQUE NOT NULL,"
						 "password VARCHAR(255) NOT NULL,"
						 "role VARCHAR(50) CHECK (role IN ('admin', 'user')) NOT NULL,"
						 "created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
						 "updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
						 ")");

		// Create Accounts table
		dbManager_->exec("CREATE TABLE IF NOT EXISTS accounts ("
						 "account_number SERIAL PRIMARY KEY,"
						 "user_id INTEGER REFERENCES users(id) ON DELETE CASCADE,"
						 "balance DECIMAL(15, 2) NOT NULL,"
						 "created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
						 ")");

		// Create Transactions table
		dbManager_->exec("CREATE TABLE IF NOT EXISTS transactions ("
						 "id SERIAL PRIMARY KEY,"
						 "account_number INTEGER REFERENCES accounts(account_number) ON DELETE CASCADE,"
						 "from_account_number INTEGER REFERENCES accounts(account_number) ON DELETE CASCADE,"
						 "to_account_number INTEGER REFERENCES accounts(account_number) ON DELETE CASCADE,"
						 "amount DECIMAL(15, 2) NOT NULL,"
						 "created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
						 "updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
						 ")");

		// Create DatabaseLogs table
		dbManager_->exec("CREATE TABLE IF NOT EXISTS DatabaseLogs ("
						 "id SERIAL PRIMARY KEY,"
						 "user_id INTEGER REFERENCES users(id) ON DELETE CASCADE,"
						 "request VARCHAR(255) NOT NULL,"
						 "created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
						 "updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
						 ")");
	}

	void CreateTestUsers()
	{
		// Insert the test user 1
		bool success = dbManager_->insert("users", {{"email", "user1@example.com"},
													{"password", "pass1"},
													{"first_name", "User1"},
													{"last_name", "Test1"},
													{"role", "user"}});

		if (!success)
		{
			qFatal("Failed to insert test user into database.");
		}

		// Insert the test user 2 (with no acosiated bank account)
		success = dbManager_->insert("users", {{"email", "user2@example.com"},
											   {"password", "pass2"},
											   {"first_name", "User2"},
											   {"last_name", "Test2"},
											   {"role", "user"}});

		if (!success)
		{
			qFatal("Failed to insert test user into database.");
		}

		// Insert a test account for the user1 with account number 111
		dbManager_->insert("accounts", {{"user_id", 1}, {"balance", 1000.00}, {"account_number", 111}});
	}

	void setupDatabase()
	{
		QString dbError;

		if (!DB::DatabaseManager::checkConnection(dbError))
		{
			//qDebug() << "Failed to connect to db" << dbError;
			qFatal("Failed to connect to database.");
		}

		// Create the necessary tables
		CreateTables();

		// Insert the test users
		CreateTestUsers();
	}

	void cleanupDatabase()
	{
		// force drop all tables
		dbManager_->exec("DROP SCHEMA public CASCADE; CREATE SCHEMA public;");

		app_->processEvents(); // Process any pending events in the event loop

		app_->quit();		   // Quit the application event loop

		delete app_;		   // Clean up QCoreApplication instance
	}

	RequestManager*		 handler_;
	QMutex*				 mutex_;
	QCoreApplication*	 app_; // Add a QCoreApplication instance
	DB::DatabaseManager* dbManager_;
};

#endif // SETUP_H
