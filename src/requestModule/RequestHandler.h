// RequestHandler.h
#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "db.h"
#include "dbresult.h"
#include "RequestManager.h"
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include <QMutexLocker>

class RequestHandler
{
private:
	DB::DatabaseManager* dbManager = nullptr;

	bool isDBConnectionValid()
	{
		if (dbManager == nullptr)
		{
			qDebug() << "Failed to create instance";
			return false;
		}

		QString dbError;

		if (!DB::DatabaseManager::checkConnection(dbError))
		{
			qDebug() << "Failed to connect to db" << dbError;
			return false;
		}

		return true;
	}

	QJsonObject CreateDBConnectionError(QJsonObject& response, QJsonObject& dataObj)
	{
		dataObj.insert("status", int(false));
		dataObj.insert("message", "Internal server error");

		response.insert("Data", dataObj);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		qDebug().noquote() << "<-- GetDatabase::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

	QJsonObject CreateErrorResponse(QJsonObject& response, QJsonObject& dataObj, QString message)
	{
		dataObj.insert("status", int(false));
		dataObj.insert("message", message);

		response.insert("Data", dataObj);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		qDebug().noquote() << "<-- GetDatabase::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

public:
	RequestHandler() : dbManager(DB::DatabaseManager::createInstance())
	{
		// doe some random sql query to check if db is connected
	}

	~RequestHandler()
	{
	}

	QJsonObject handleLogin(const QJsonObject& jsonObj, QMutex& m)
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString email;
		QString password;

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 1);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				email = dataObj.value("email").toString();
			}

			if (dataObj.contains("password"))
			{
				password = dataObj.value("password").toString();
			}
		}
		else
		{
			qCritical() << "Data not found";
		}

		do
		{
			if (!isDBConnectionValid())
			{
				return CreateDBConnectionError(response, data);
			}

			DB::DbResult result = dbManager->select("*")->table("users")->where("email =", email)->exec();
			int			 user_id = result.data(0).value("id").toInt();
			QJsonObject	 obj = result.data(0);

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "email not found");
			}

			result = dbManager->select("password")->table("users")->where("id =", user_id)->exec();
			if (result.data(0).value("password").toString() != password)
			{
				return CreateErrorResponse(response, data, "Invalid password");
			}

			data.insert("status", int(true));
			data.insert("message", "Login successful");
			data.insert("first_name", obj.value("first_name").toString());
			data.insert("role", obj.value("role").toString());

			response.insert("Data", data);
		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- Login::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

	QJsonObject handleGetAccountNumber(const QJsonObject& jsonObj, QMutex& m)
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString email;

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 2);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				email = dataObj.value("email").toString();
			}
		}
		else
		{
			qCritical() << "Data not found";
		}

		do
		{
			if (!isDBConnectionValid())
			{
				return CreateDBConnectionError(response, data);
			}

			DB::DbResult result = dbManager->select("*")->table("users")->where("email =", email)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "email not found");
			}

			result = dbManager->select("A.account_number")
						 ->table("Users U")
						 ->join("JOIN Accounts A ON U.id = A.user_id")
						 ->where("U.email =", email)
						 ->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "No account found");
			}

			QJsonObject obj = result.data(0);

			data.insert("status", int(true));
			data.insert("message", "Account number found");
			data.insert("account_number", obj.value("account_number").toInt());

			response.insert("Data", data);
		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- GetAccountNumber::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

	QJsonObject handleGetBalance(const QJsonObject& jsonObj, QMutex& m)
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		int accountNumber;

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 3);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("account_number"))
			{
				accountNumber = dataObj.value("account_number").toInt();
			}
		}
		else
		{
			qCritical() << "Data not found";
		}

		do
		{
			if (!isDBConnectionValid())
			{
				return CreateDBConnectionError(response, data);
			}

			DB::DbResult result =
				dbManager->select("balance")->table("Accounts")->where("account_number =", accountNumber)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "Account not found");
			}

			QJsonObject obj = result.data(0);

			data.insert("status", int(true));
			data.insert("message", "Balance fetched successfully");
			data.insert("balance", obj.value("balance").toDouble());

			response.insert("Data", data);
		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- GetBalance::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

	QJsonObject handleGetTransactionsHistory(const QJsonObject& jsonObj, QMutex& m)
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QJsonObject response;
		QJsonObject data;
		QString		sender_email;
		QString		sender_role;

		response.insert("Response", 4);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				sender_email = dataObj.value("email").toString();
			}
		}
		else
		{
			qCritical() << "Data not found";
		}

		do
		{
			if (!isDBConnectionValid())
			{
				return CreateDBConnectionError(response, data);
			}

			// Check if the user is an admin
			DB::DbResult result = dbManager->select("role")->table("Users")->where("email =", sender_email)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "you are not registered user!");
			}

			QString role = result.data(0).value("role").toString();

			if (role == "user")
			{
				// Get the account number for the user
				result = dbManager->select("A.account_number")
							 ->table("Users U")
							 ->join("JOIN Accounts A ON U.id = A.user_id")
							 ->where("U.email =", sender_email)
							 ->exec();

				if (result.isEmpty())
				{
					return CreateErrorResponse(response, data, "No account found");
				}

				int accountNumber = result.data(0).value("account_number").toInt();

				result = dbManager->select("*")
							 ->table("Transactions")
							 ->where("from_account_number =", accountNumber)
							 ->whereOr("to_account_number =" + QString::number(accountNumber))
							 ->exec();

				if (result.isEmpty())
				{
					return CreateErrorResponse(response, data, "No transactions found");
				}

				QJsonArray transactionList;
				for (int i = 0; i < result.size(); ++i)
				{
					QJsonObject transactionObj;
					QJsonObject obj = result.data(i);

					transactionObj.insert("from_account_number", obj.value("from_account_number").toInt());
					transactionObj.insert("to_account_number", obj.value("to_account_number").toInt());
					transactionObj.insert("amount", obj.value("amount").toDouble());
					transactionObj.insert("created_at", obj.value("created_at").toString());

					transactionList.append(transactionObj);
				}

				data.insert("status", int(true));
				data.insert("message",
							"Transaction history retrieved for account number " + QString::number(accountNumber));
				data.insert("List", transactionList);

				response.insert("Data", data);
			}
			else if (role == "admin")
			{
				result = dbManager->select("*")->table("Transactions")->exec();

				if (result.isEmpty())
				{
					return CreateErrorResponse(response, data, "No transactions found");
				}

				QJsonArray transactionList;
				for (int i = 0; i < result.size(); ++i)
				{
					QJsonObject transactionObj;
					QJsonObject obj = result.data(i);

					transactionObj.insert("from_account_number", obj.value("from_account_number").toInt());
					transactionObj.insert("to_account_number", obj.value("to_account_number").toInt());
					transactionObj.insert("amount", obj.value("amount").toDouble());
					transactionObj.insert("created_at", obj.value("created_at").toString());

					transactionList.append(transactionObj);
				}

				data.insert("status", int(true));
				data.insert("message", "Transaction history retrieved for all users");
				data.insert("List", transactionList);

				response.insert("Data", data);
			}

		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- GetTransactionsHistory::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

	QJsonObject handleMakeTransaction(const QJsonObject& jsonObj, QMutex& m)
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		int		from_account_number{};
		int		to_account_number{};
		QString to_email{};
		double	amount{};

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 5);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("from_account_number"))
			{
				from_account_number = dataObj.value("from_account_number").toInt();
			}
			if (dataObj.contains("to_account_number"))
			{
				to_account_number = dataObj.value("to_account_number").toInt();
			}
			if (dataObj.contains("transaction_amount"))
			{
				amount = dataObj.value("transaction_amount").toDouble();
			}
			if (dataObj.contains("to_email"))
			{
				to_email = dataObj.value("to_email").toString();
			}
		}
		else
		{
			qCritical() << "Data not found";
		}

		do
		{
			if (!isDBConnectionValid())
			{
				return CreateDBConnectionError(response, data);
			}

			if (!to_email.isEmpty())
			{
				// check if email is valid and get the account number associated with it
				DB::DbResult result = dbManager->select("account_number")
										  ->table("Users U")
										  ->join("JOIN Accounts A ON U.id = A.user_id")
										  ->where("U.email =", to_email)
										  ->exec();

				if (result.isEmpty())
				{
					return CreateErrorResponse(response, data, "This email is not associated with any account");
				}

				to_account_number = result.data(0).value("account_number").toInt();
			}

			DB::DbResult fromAccountResult =
				dbManager->select("balance")->table("Accounts")->where("account_number =", from_account_number)->exec();

			if (fromAccountResult.isEmpty())
			{
				return CreateErrorResponse(response, data, "You don't have an account");
			}

			DB::DbResult toAccountResult =
				dbManager->select("balance")->table("Accounts")->where("account_number =", to_account_number)->exec();

			if (toAccountResult.isEmpty())
			{
				return CreateErrorResponse(response, data, "Invalid to account number");
			}

			double fromAccountBalance = fromAccountResult.data(0).value("balance").toDouble();

			if (fromAccountBalance < amount)
			{
				return CreateErrorResponse(response, data, "Insufficient balance");
			}

			double toAccountBalance = toAccountResult.data(0).value("balance").toDouble();

			// Update balances
			fromAccountBalance -= amount;
			toAccountBalance += amount;
			qDebug() << "fromAccountBalance: " << fromAccountBalance;
			qDebug() << "toAccountBalance: " << toAccountBalance;
			qDebug() << "amount: " << amount;

			// log transaction and update account balances
			bool success = dbManager->insert("Transactions", {{"account_number", from_account_number},
															  {"from_account_number", from_account_number},
															  {"to_account_number", to_account_number},
															  {"amount", amount}});

			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to log transaction");
			}

			// Update the account balances (from)
			dbManager->where("account_number = ", from_account_number)
				->update("Accounts", {{"balance", fromAccountBalance}});

			// Update the account balances (to)
			dbManager->where("account_number = ", to_account_number)
				->update("Accounts", {{"balance", toAccountBalance}});

			data.insert("status", int(true));
			data.insert("message", "Transaction successful");

			response.insert("Data", data);
		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- MakeTransaction::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

	QJsonObject handleGetDatabase(const QJsonObject& jsonObj, QMutex& m)
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QJsonObject response;
		QJsonObject data;

		QString admin_email;

		response.insert("Response", 7);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				admin_email = dataObj.value("email").toString();
			}
		}
		else
		{
			qCritical() << "Data not found";
		}

		do
		{
			if (!isDBConnectionValid())
			{
				return CreateDBConnectionError(response, data);
			}

			// Check if the user is an admin
			DB::DbResult result = dbManager->select("role")->table("Users")->where("email =", admin_email)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "you are not registered user!");
			}

			QJsonObject obj = result.data(0);

			if (obj.value("role").toString() != "admin")
			{
				return CreateErrorResponse(response, data, "Unauthorized, Cannot get database. User is not an admin");
			}

			// Get all users from the database
			result = dbManager->select("*")->table("Users")->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "No data found");
			}

			QJsonArray userList;

			for (int i = 0; i < result.size(); ++i)
			{
				QJsonObject userObj;
				QJsonObject obj = result.data(i);

				userObj.insert("account_number", QJsonValue::Null);
				userObj.insert("first_name", obj.value("first_name").toString());
				userObj.insert("last_name", obj.value("last_name").toString());
				userObj.insert("email", obj.value("email").toString());
				userObj.insert("role", obj.value("role").toString());
				userObj.insert("balance", QJsonValue::Null);

				userList.append(userObj);
			}

			// Get all accounts from the database that are associated with the users
			result = dbManager->select("A.account_number, A.balance, U.email")
						 ->table("Accounts A")
						 ->join("JOIN Users U ON A.user_id = U.id")
						 ->exec();

			if (!result.isEmpty())
			{
				for (int i = 0; i < result.size(); ++i)
				{
					QJsonObject obj = result.data(i);

					for (int j = 0; j < userList.size(); ++j)
					{
						QJsonObject userObj = userList[j].toObject();
						if (userObj.value("email").toString() == obj.value("email").toString())
						{
							userObj.insert("account_number", obj.value("account_number").toInt());
							userObj.insert("balance", obj.value("balance").toDouble());
							userList[j] = userObj;
							break;
						}
					}
				}
			}

			data.insert("status", int(true));
			data.insert("message", "Database fetched successfully");
			data.insert("users", userList);

			response.insert("Data", data);
		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- GetDatabase::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

	QJsonObject handleCreateNewUser(const QJsonObject& jsonObj, QMutex& m)
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString email{};
		QString password{};
		QString first_name{};
		QString last_name{};
		QString new_email{};
		QString new_password{};
		QString role{};
		double	initial_balance{};

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 8);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				email = dataObj.value("email").toString();
			}

			if (dataObj.contains("newUser"))
			{
				QJsonArray newUserArray = dataObj["newUser"].toArray();

				for (const auto& newUser: newUserArray)
				{
					QJsonObject newUserObj = newUser.toObject();

					if (newUserObj.contains("first_name"))
					{
						first_name = newUserObj.value("first_name").toString();
					}
					if (newUserObj.contains("last_name"))
					{
						last_name = newUserObj.value("last_name").toString();
					}
					if (newUserObj.contains("email"))
					{
						new_email = newUserObj.value("email").toString();
					}
					if (newUserObj.contains("password"))
					{
						new_password = newUserObj.value("password").toString();
					}
					if (newUserObj.contains("role"))
					{
						role = newUserObj.value("role").toString();
					}
					if (newUserObj.contains("initial_balance"))
					{
						initial_balance = newUserObj.value("initial_balance").toDouble();
					}
				}
			}
		}
		else
		{
			qCritical() << "Data not found";
		}

		do
		{
			if (!isDBConnectionValid())
			{
				return CreateDBConnectionError(response, data);
			}

			// Check if the user is an admin
			DB::DbResult result = dbManager->select("role")->table("users")->where("email =", email)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "you are not registered user!");
			}

			QJsonObject obj = result.data(0);

			if (obj.value("role").toString() != "admin")
			{
				return CreateErrorResponse(response, data,
										   "Unauthorized, Cannot create new user. User is not an admin");
			}

			if (new_email.isEmpty() || new_password.isEmpty() || first_name.isEmpty() || last_name.isEmpty() ||
				role.isEmpty())
			{
				return CreateErrorResponse(response, data, "Missing required fields");
			}

			// invalid role
			if (role != "admin" && role != "user")
			{
				return CreateErrorResponse(response, data, "Invalid role");
			}

			// admin can't have account and initial balance
			if (role == "admin" && initial_balance != 0)
			{
				return CreateErrorResponse(response, data, "Admin can't have account and initial balance");
			}

			// Check if there is a user with the same email
			result = dbManager->select("*")->table("users")->where("email =", new_email)->exec();

			if (!result.isEmpty())
			{
				return CreateErrorResponse(response, data, "User already exists");
			}

			// Create the new user
			bool success = dbManager->insert("users", {{"email", new_email},
													   {"password", new_password},
													   {"first_name", first_name},
													   {"last_name", last_name},
													   {"role", role}});
			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to create new user");
			}

			if (role == "admin" && initial_balance == 0)
			{
				qDebug() << "New admin created successfully";

				data.insert("status", int(true));
				data.insert("message", "New admin created successfully");

				response.insert("Data", data);

				break;
			}

			// Get the user id from the last insert operation
			result = dbManager->select("id")->table("users")->where("email =", new_email)->exec();
			QVariant lastId = result.data(0).value("id");
			//! or using QVariant lastId = dbManager->lastInsertedId();

			// Create the account for the new user and randomly generate an account number
			int account_number = rand() % 1000000 + 100000;

			success = dbManager->insert(
				"accounts",
				{{"account_number", account_number}, {"user_id", lastId.toInt()}, {"balance", initial_balance}});

			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to create account for the new user");
			}

			data.insert("status", int(true));
			data.insert("message", "New user created successfully");

			response.insert("Data", data);

		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- CreateNewUser::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

	QJsonObject handleDeleteUser(const QJsonObject& jsonObj, QMutex& m)
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString admin_email;
		QString account_number;

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 9);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				admin_email = dataObj.value("email").toString();
			}
			if (dataObj.contains("account_number"))
			{
				account_number = dataObj.value("account_number").toString();
			}
		}
		else
		{
			qCritical() << "Data not found";
		}

		do
		{
			if (!isDBConnectionValid())
			{
				return CreateDBConnectionError(response, data);
			}

			// Check if the user is an admin
			DB::DbResult result = dbManager->select("*")->table("users")->where("email =", admin_email)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "you are not registered user!");
			}

			QJsonObject obj = result.data(0);

			if (obj.value("role").toString() != "admin")
			{
				return CreateErrorResponse(response, data, "Unauthorized, Cannot delete user.");
			}

			// Check if the account number is valid
			result = dbManager->select("*")->table("accounts")->where("account_number =", account_number)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "Account number does not exist");
			}

			// Get the user id from the account number
			int user_id = result.data(0).value("user_id").toInt();

			bool success = dbManager->where("account_number = ", account_number)->del("accounts");

			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to delete account");
			}

			success = dbManager->where("id = ", user_id)->del("users");

			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to delete user");
			}

			data.insert("status", int(true));
			data.insert("message", "User deleted successfully");

			response.insert("Data", data);

		} while (false);

		// Send response
		qDebug().noquote() << "<-- DeleteUser::Response :\n" << QJsonDocument(response).toJson(QJsonDocument::Indented);

		return response;
	}

	QJsonObject handleUpdateUser(const QJsonObject& jsonObj, QMutex& m)
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString email;
		int		account_number;
		QString new_first_name;
		QString new_last_name;
		QString new_email;
		QString new_role;

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 10);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				email = dataObj.value("email").toString();
			}
			if (dataObj.contains("account_number"))
			{
				account_number = dataObj.value("account_number").toInt();
			}
			if (dataObj.contains("newData"))
			{
				QJsonObject newDataObj = dataObj["newData"].toObject();

				if (newDataObj.contains("first_name"))
				{
					new_first_name = newDataObj.value("first_name").toString();
				}
				if (newDataObj.contains("last_name"))
				{
					new_last_name = newDataObj.value("last_name").toString();
				}
				if (newDataObj.contains("email"))
				{
					new_email = newDataObj.value("email").toString();
				}
				if (newDataObj.contains("role"))
				{
					new_role = newDataObj.value("role").toString();
				}
			}
		}
		else
		{
			qCritical() << "Data not found";
		}

		do
		{
			if (!isDBConnectionValid())
			{
				return CreateDBConnectionError(response, data);
			}

			// Check if the user is an admin
			DB::DbResult result = dbManager->select("role")->table("users")->where("email =", email)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "you are not registered user!");
			}

			QJsonObject obj = result.data(0);

			if (obj.value("role").toString() != "admin")
			{
				return CreateErrorResponse(response, data, "Unauthorized, Cannot update user");
			}

			// Check if the account number is valid
			result = dbManager->select("*")->table("accounts")->where("account_number =", account_number)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "Account number does not exist");
			}

			// Get the user id from the account number
			int user_id = result.data(0).value("user_id").toInt();

			// check that new email is not associated with another account or user unless it is the current user
			result = dbManager->select("*")->table("users")->where("email =", new_email)->exec();

			int new_user_id = result.data(0).value("id").toInt();

			qDebug() << "user_id: " << user_id;
			qDebug() << "new_user_id: " << new_user_id;

			if (!result.isEmpty() && result.data(0).value("id").toInt() != user_id)
			{
				return CreateErrorResponse(response, data, "Email is associated with another account");
			}

			qDebug() << "Debugging update user";
			qDebug() << "user_id: " << user_id;
			qDebug() << "new_email: " << new_email;
			qDebug() << "new_role: " << new_role;
			qDebug() << "new_first_name: " << new_first_name;
			qDebug() << "new_last_name: " << new_last_name;

			// Update the user details
			bool success = dbManager->where("id = ", user_id)
							   ->update("users", {{"first_name", new_first_name},
												  {"last_name", new_last_name},
												  {"email", new_email},
												  {"role", new_role}});

			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to update user");
			}

			data.insert("status", int(true));
			data.insert("message", "User updated successfully");

			response.insert("Data", data);

		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- UpdateUser::Response :\n" << QJsonDocument(response).toJson(QJsonDocument::Indented);

		return response;
	}

	QJsonObject handleUserInit(const QJsonObject& jsonObj, QMutex& m)
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString email;
		QString password;

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 11);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				email = dataObj.value("email").toString();
			}

			if (dataObj.contains("password"))
			{
				password = dataObj.value("password").toString();
			}
		}
		else
		{
			qCritical() << "Data not found";
		}

		do
		{
			if (!isDBConnectionValid())
			{
				return CreateDBConnectionError(response, data);
			}

			// Validate user credentials
			DB::DbResult result = dbManager->select("*")->table("users")->where("email =", email)->exec();
			int			 user_id = result.data(0).value("id").toInt();
			QJsonObject	 userObj = result.data(0);

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "email not found");
			}

			result = dbManager->select("password")->table("users")->where("id =", user_id)->exec();
			if (result.data(0).value("password").toString() != password)
			{
				return CreateErrorResponse(response, data, "Invalid password");
			}

			QString role = userObj.value("role").toString();

			data.insert("status", int(true));
			data.insert("first_name", userObj.value("first_name").toString());
			data.insert("role", role);
			data.insert("email", email);

			if (role == "user")
			{
				// Retrieve account number and current balance for the user
				DB::DbResult accountResult = dbManager->select("account_number, balance")
												 ->table("accounts")
												 ->where("user_id =", userObj.value("id").toInt())
												 ->exec();

				QJsonObject accountObj = accountResult.data(0);

				int	   accountNumber = accountObj.value("account_number").toInt();
				double currentBalance = accountObj.value("balance").toDouble();

				data.insert("account_number", accountNumber);
				data.insert("current_balance", currentBalance);
			}

			response.insert("Data", data);
		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- InitRequest::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

	QJsonObject handleUpdateEmail(const QJsonObject& jsonObj, QMutex& m)
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString email;
		QString password;
		QString new_email;

		QJsonObject response;

		QJsonObject data;

		response.insert("Response", 12);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				email = dataObj.value("email").toString();
			}

			if (dataObj.contains("password"))
			{
				password = dataObj.value("password").toString();
			}

			if (dataObj.contains("new_email"))
			{
				new_email = dataObj.value("new_email").toString();
			}
		}
		else
		{
			qCritical() << "Data not found";
		}

		do
		{
			if (!isDBConnectionValid())
			{
				return CreateDBConnectionError(response, data);
			}

			// Validate user credentials
			DB::DbResult result = dbManager->select("*")->table("users")->where("email =", email)->exec();
			int			 user_id = result.data(0).value("id").toInt();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "email not found");
			}

			// change the password of that specific user
			result = dbManager->select("password")->table("users")->where("id =", user_id)->exec();
			if (result.data(0).value("password").toString() != password)
			{
				return CreateErrorResponse(response, data, "Invalid password");
			}

			// Check if the new email is already associated with another account
			result = dbManager->select("*")->table("users")->where("email =", new_email)->exec();

			if (!result.isEmpty())
			{
				return CreateErrorResponse(response, data, "Email already exists");
			}

			// Update the email
			bool success = dbManager->where("email = ", email)->update("users", {{"email", new_email}});

			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to update email");
			}

			data.insert("status", int(true));
			data.insert("message", "Email updated successfully");

			response.insert("Data", data);

		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- UpdateEmail::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

	QJsonObject handleUpdatePassword(const QJsonObject& jsonObj, QMutex& m)
	{
		QMutexLocker locker(&m); // Lock the mutex for the duration of this function

		QString email;
		QString password;
		QString new_password;

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 13);

		// Extract the data array
		if (jsonObj.contains("Data"))
		{
			QJsonObject dataObj = jsonObj["Data"].toObject();

			if (dataObj.contains("email"))
			{
				email = dataObj.value("email").toString();
			}

			if (dataObj.contains("password"))
			{
				password = dataObj.value("password").toString();
			}

			if (dataObj.contains("new_password"))
			{
				new_password = dataObj.value("new_password").toString();
			}
		}
		else
		{
			qCritical() << "Data not found";
		}

		do
		{
			if (!isDBConnectionValid())
			{
				return CreateDBConnectionError(response, data);
			}

			// Validate user credentials
			DB::DbResult result = dbManager->select("*")->table("users")->where("email =", email)->exec();
			int			 user_id = result.data(0).value("id").toInt();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "email not found");
			}

			result = dbManager->select("password")->table("users")->where("id =", user_id)->exec();
			if (result.data(0).value("password").toString() != password)
			{
				return CreateErrorResponse(response, data, "Invalid password");
			}

			// Update the password
			bool success = dbManager->where("email = ", email)->update("users", {{"password", new_password}});

			if (!success)
			{
				return CreateErrorResponse(response, data, "Failed to update password");
			}

			data.insert("status", int(true));
			data.insert("message", "Password updated successfully");

			response.insert("Data", data);

		} while (false);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- UpdatePassword::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}

	QJsonObject handleJsonParseError(const QJsonParseError& parseError)
	{
		QJsonObject response;
		QJsonObject data;

		response.insert("Response", -1);

		data.insert("status", int(false));
		data.insert("message", "JSON Parse Error: " + parseError.errorString());

		response.insert("Data", data);

		// Convert response to JSON
		QJsonDocument responseDoc(response);
		QByteArray	  responseData = responseDoc.toJson();

		// Send response
		qDebug().noquote() << "<-- JSON Parse Error::Response :\n" << responseDoc.toJson(QJsonDocument::Indented);

		return response;
	}
};

#endif // REQUESTHANDLER_H
