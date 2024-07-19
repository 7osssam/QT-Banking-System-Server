#ifndef MAKETRANSACTIONREQUEST_H
#define MAKETRANSACTIONREQUEST_H

#include "Request.h"
#include "db.h"

class MakeTransactionRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr;

public:
	MakeTransactionRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// log to database log table
	}

	QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) override
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
			if (!isDBConnectionValid(dbManager))
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
};

#endif // MAKETRANSACTIONREQUEST_H
