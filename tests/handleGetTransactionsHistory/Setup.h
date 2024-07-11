// setup.h
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

class handleGetTransactionsHistoryTest : public ::testing::Test
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

		dbManager_->exec("CREATE TABLE IF NOT EXISTS Accounts ("
						 "account_number BIGINT PRIMARY KEY,"
						 "user_id INTEGER REFERENCES users(id) ON DELETE CASCADE,"
						 "balance DECIMAL(15, 2) NOT NULL,"
						 "created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
						 ");");

		dbManager_->exec("CREATE TABLE IF NOT EXISTS Transactions ("
						 "id SERIAL PRIMARY KEY,"
						 "account_number BIGINT REFERENCES Accounts(account_number) ON DELETE CASCADE,"
						 "from_account_number BIGINT REFERENCES Accounts(account_number) ON DELETE CASCADE,"
						 "to_account_number BIGINT REFERENCES Accounts(account_number) ON DELETE CASCADE,"
						 "amount DECIMAL(15, 2) NOT NULL,"
						 "created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
						 "updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
						 ");");
	}

	void CreateTestUsersAndAccounts()
	{
		// Insert the test user 1 with account number 111
		dbManager_->insert("users", {{"email", "user1@example.com"},
									 {"password", "pass1"},
									 {"first_name", "User1"},
									 {"last_name", "Test1"},
									 {"role", "user"}});
		// Insert the test user 2 with account number 222
		dbManager_->insert("users", {{"email", "user2@example.com"},
									 {"password", "pass2"},
									 {"first_name", "User2"},
									 {"last_name", "Test2"},
									 {"role", "user"}});

		// Insert the test user 3 to test retrieving only history of the requested account
		dbManager_->insert("users", {{"email", "user3@example.com"},
									 {"password", "pass3"},
									 {"first_name", "User3"},
									 {"last_name", "Test3"},
									 {"role", "user"}});

		// Insert the test user 4 with account and no transactions
		dbManager_->insert("users", {{"email", "user4@example.com"},
									 {"password", "pass4"},
									 {"first_name", "User4"},
									 {"last_name", "Test4"},
									 {"role", "user"}});

		// Insert the test user 5  with no account
		dbManager_->insert("users", {{"email", "user5@example.com"},
									 {"password", "pass5"},
									 {"first_name", "User5"},
									 {"last_name", "Test5"},
									 {"role", "user"}});

		// Insert the test admin user
		dbManager_->insert("users", {{"email", "admin@mail.com"},
									 {"password", "admin"},
									 {"first_name", "Admin"},
									 {"last_name", "Test"},
									 {"role", "admin"}});

		// Insert the test accounts with specific account numbers
		dbManager_->insert("Accounts", {{"account_number", 111}, {"user_id", 1}, {"balance", 1000.00}});

		// Insert the test accounts with specific account numbers
		dbManager_->insert("Accounts", {{"account_number", 222}, {"user_id", 2}, {"balance", 1000.00}});

		// Insert the test accounts with specific account numbers
		dbManager_->insert("Accounts", {{"account_number", 333}, {"user_id", 3}, {"balance", 5000.00}});

		// Insert the test accounts with specific account numbers
		dbManager_->insert("Accounts", {{"account_number", 444}, {"user_id", 4}, {"balance", 50.00}});

		dbManager_->insert("Transactions", {{"account_number", 111},
											{"from_account_number", 111},
											{"to_account_number", 222},
											{"amount", 200.00},
											{"created_at", "2021-01-01"}});

		dbManager_->insert("Transactions", {{"account_number", 222},
											{"from_account_number", 222},
											{"to_account_number", 111},
											{"amount", 500.50},
											{"created_at", "2021-01-03"}});

		dbManager_->insert("Transactions", {{"account_number", 222},
											{"from_account_number", 222},
											{"to_account_number", 333},
											{"amount", 700},
											{"created_at", "2021-01-04"}});
	}

	void setupDatabase()
	{
		QString dbError;

		if (!DB::DatabaseManager::checkConnection(dbError))
		{
			qFatal("Failed to connect to database.");
		}

		// Create the necessary tables
		CreateTables();

		// Insert the test users and accounts
		CreateTestUsersAndAccounts();
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

#endif						   // SETUP_H
