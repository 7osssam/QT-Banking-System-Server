// RequestHandler.h
#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "db.h"
#include "dbresult.h"
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include <QMutexLocker>

// Access the data array
//QJsonArray dataArray = jsonObj["Data"].toArray();

//for (const auto& item: dataArray)
//{
//	qDebug() << "item: " << item;
//	QJsonObject obj = item.toObject();
//	qDebug() << "obj: " << obj;
//	if (obj.contains("email"))
//	{
//		email = obj.value("email").toString();
//	}
//	if (obj.contains("password"))
//	{
//		password = obj.value("password").toString();
//	}
//}

class RequestHandler
{
private:
	DB::DatabaseManager* dbManager;

	bool checkDBConnection(QJsonObject response, QJsonObject dataObj)
	{
		if (dbManager == nullptr)
		{
			qDebug() << "Failed to create instance";

			dataObj.insert("status", int(false));
			dataObj.insert("message", "Internal server error");

			response.insert("Data", dataObj);

			return false;
		}

		QString dbError;

		if (!DB::DatabaseManager::checkConnection(dbError))
		{
			qDebug() << "Failed to connect to db" << dbError;

			dataObj.insert("status", int(false));
			dataObj.insert("message", "Internal server error");

			response.insert("Data", dataObj);

			return false;
		}

		return true;
	}

public:
	RequestHandler(/* args */)
	{
		dbManager = DB::DatabaseManager::createInstance();
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
			if (!checkDBConnection(response, data))
			{
				break;
			}

			DB::DbResult result = dbManager->select("*")->table("users")->where("email =", email)->exec();

			if (result.isEmpty())
			{
				qDebug() << "email not found";

				data.insert("status", int(false));
				data.insert("message", "email not found");

				response.insert("Data", data);

				break;
			}

			result = dbManager->select("*")->table("users")->where("password =", password)->exec();

			if (result.isEmpty())
			{
				qDebug() << "password not found";

				data.insert("status", int(false));
				data.insert("message", "password not found");

				response.insert("Data", data);

				break;
			}

			QJsonObject obj = result.data(0);

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
			if (!checkDBConnection(response, data))
			{
				break;
			}

			DB::DbResult result = dbManager->select("*")->table("users")->where("email =", email)->exec();

			if (result.isEmpty())
			{
				qDebug() << "email not found";

				data.insert("status", int(false));
				data.insert("message", "email not found");

				response.insert("Data", data);

				break;
			}

			result = dbManager->select("A.account_number")
						 ->table("Users U")
						 ->join("JOIN Accounts A ON U.id = A.user_id")
						 ->where("U.email =", email)
						 ->exec();

			if (result.isEmpty())
			{
				qDebug() << "No account found";

				data.insert("status", int(false));
				data.insert("message", "No account found");

				response.insert("Data", data);

				break;
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

	//!TODO
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
			if (!checkDBConnection(response, data))
			{
				break;
			}

			DB::DbResult result =
				dbManager->select("balance")->table("Accounts")->where("account_number =", accountNumber)->exec();

			if (result.isEmpty())
			{
				qDebug() << "Account not found";

				data.insert("status", int(false));
				data.insert("message", "Account not found");

				response.insert("Data", data);

				break;
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

		int accountNumber;

		QJsonObject response;
		QJsonObject data;

		response.insert("Response", 4);

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
			if (!checkDBConnection(response, data))
			{
				break;
			}

			DB::DbResult result =
				dbManager->select("*")->table("Transactions")->where("account_number =", accountNumber)->exec();

			if (result.isEmpty())
			{
				qDebug() << "No transactions found";

				data.insert("status", int(false));
				data.insert("message", "No transactions found");

				response.insert("Data", data);

				break;
			}

			QJsonArray transactionList;
			for (int i = 0; i < result.size(); ++i)
			{
				QJsonObject transactionObj;
				QJsonObject obj = result.data(i);

				transactionObj.insert("to_account_number", obj.value("to_account_number").toInt());
				transactionObj.insert("transaction_amount", obj.value("amount").toDouble());
				transactionObj.insert("created_at", obj.value("created_at").toString());

				if (accountNumber == obj.value("from_account_number").toInt())
				{
					transactionObj.insert("transaction_type", "-"); // Withdrawal
				}
				else
				{
					transactionObj.insert("transaction_type", "+"); // Deposit
				}

				transactionList.append(transactionObj);
			}

			data.insert("status", int(true));
			data.insert("message", "Transaction history retrieved");
			data.insert("account_number", accountNumber);
			data.insert("List", transactionList);

			response.insert("Data", data);
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

		int	   from_account_number{};
		int	   to_account_number{};
		double amount{};

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
		}
		else
		{
			qCritical() << "Data not found";
		}

		do
		{
			if (!checkDBConnection(response, data))
			{
				break;
			}

			DB::DbResult fromAccountResult =
				dbManager->select("balance")->table("Accounts")->where("account_number =", from_account_number)->exec();

			if (fromAccountResult.isEmpty())
			{
				qDebug() << "From account not found";

				data.insert("status", int(false));
				data.insert("message", "Invalid from account number");

				response.insert("Data", data);

				break;
			}

			DB::DbResult toAccountResult =
				dbManager->select("balance")->table("Accounts")->where("account_number =", to_account_number)->exec();

			if (toAccountResult.isEmpty())
			{
				qDebug() << "To account not found";

				data.insert("status", int(false));
				data.insert("message", "Invalid to account number");

				response.insert("Data", data);

				break;
			}

			double fromAccountBalance = fromAccountResult.data(0).value("balance").toDouble();

			if (fromAccountBalance < amount)
			{
				qDebug() << "Insufficient balance";

				data.insert("status", int(false));
				data.insert("message", "Insufficient balance");

				response.insert("Data", data);

				break;
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
				qDebug() << "Failed to log transaction";

				data.insert("status", int(false));
				data.insert("message", "Failed to log transaction");

				response.insert("Data", data);

				break;
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
			if (!checkDBConnection(response, data))
			{
				break;
			}

			// Check if the user is an admin
			DB::DbResult result = dbManager->select("role")->table("Users")->where("email =", admin_email)->exec();

			if (result.isEmpty())
			{
				qDebug() << "email not found";

				data.insert("status", int(false));
				data.insert("message", "email not found");

				response.insert("Data", data);

				break;
			}

			QJsonObject obj = result.data(0);

			if (obj.value("role").toString() != "admin")
			{
				qDebug() << "Cannot get database. User is not an admin";

				data.insert("status", int(false));
				data.insert("message", "Cannot get database. User is not an admin");

				response.insert("Data", data);

				break;
			}

			// Get all users from the database
			result = dbManager->select("*")->table("Users")->exec();

			if (result.isEmpty())
			{
				qDebug() << "No data found";

				data.insert("status", int(false));
				data.insert("message", "No data found");

				response.insert("Data", data);

				break;
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
			if (!checkDBConnection(response, data))
			{
				break;
			}

			// Check if the user is an admin
			DB::DbResult result = dbManager->select("role")->table("users")->where("email =", email)->exec();

			if (result.isEmpty())
			{
				qDebug() << "email not found";

				data.insert("status", int(false));
				data.insert("message", "email not found");

				response.insert("Data", data);

				break;
			}

			QJsonObject obj = result.data(0);

			if (obj.value("role").toString() != "admin")
			{
				qDebug() << "Cannot create new user. User is not an admin";

				data.insert("status", int(false));
				data.insert("message", "Cannot create new user. User is not an admin");

				response.insert("Data", data);

				break;
			}

			if (new_email.isEmpty() || new_password.isEmpty() || first_name.isEmpty() || last_name.isEmpty() ||
				role.isEmpty())
			{
				qDebug() << "Missing required fields";

				data.insert("status", int(false));
				data.insert("message", "Missing required fields");

				response.insert("Data", data);

				break;
			}

			// invalid role
			if (role != "admin" && role != "user")
			{
				qDebug() << "Invalid role";

				data.insert("status", int(false));
				data.insert("message", "Invalid role");

				response.insert("Data", data);

				break;
			}

			// admin can't have account and initial balance
			if (role == "admin" && initial_balance != 0)
			{
				qDebug() << "Admin can't have account and initial balance";

				data.insert("status", int(false));
				data.insert("message", "Admin can't have account and initial balance");

				response.insert("Data", data);

				break;
			}

			// Check if there is a user with the same email
			result = dbManager->select("*")->table("users")->where("email =", new_email)->exec();

			if (!result.isEmpty())
			{
				qDebug() << "User already exists";

				data.insert("status", int(false));
				data.insert("message", "User already exists");

				response.insert("Data", data);

				break;
			}

			// Create the new user
			bool success = dbManager->insert("users", {{"email", new_email},
													   {"password", new_password},
													   {"first_name", first_name},
													   {"last_name", last_name},
													   {"role", role}});
			if (!success)
			{
				qDebug() << "Failed to create new user";

				data.insert("status", int(false));
				data.insert("message", "Failed to create new user");

				response.insert("Data", data);

				break;
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
			QVariant lastId = dbManager->lastInsertedId();

			// Create the account for the new user and randomly generate an account number
			int account_number = rand() % 1000000 + 100000;

			success = dbManager->insert(
				"accounts",
				{{"account_number", account_number}, {"user_id", lastId.toInt()}, {"balance", initial_balance}});

			if (!success)
			{
				qDebug() << "Failed to create account for the new user";

				data.insert("status", int(false));
				data.insert("message", "Failed to create account for the new user");

				response.insert("Data", data);

				break;
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
