#ifndef GETTRANSACTIONSHISTORYREQUEST_H
#define GETTRANSACTIONSHISTORYREQUEST_H

#include "Request.h"
#include "db.h"

/**
 * @brief The GetTransactionsHistoryRequest class handles the retrieval of transaction history.
 *
 * This class processes requests to retrieve transaction history for a user or an admin.
 * It performs database operations and returns the transaction details.
 */
class GetTransactionsHistoryRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr; ///< Pointer to the DatabaseManager instance.

public:
	/**
     * @brief Constructor for the GetTransactionsHistoryRequest class.
     *
     * Initializes the DatabaseManager instance for handling database operations.
     */
	GetTransactionsHistoryRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// Log to database log table (if needed)
	}

	/**
     * @brief Executes the request to get the transaction history.
     *
     * This method processes the JSON request to retrieve transaction history for a user or admin.
     * It validates the input data, checks the database connection, and fetches the data from the database.
     *
     * @param jsonObj The JSON object containing the request data.
     * @param m The mutex to lock during the execution.
     * @return A JSON object containing the response data.
     */
	QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) override
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
			if (!isDBConnectionValid(dbManager))
			{
				return CreateDBConnectionError(response, data);
			}

			// Check if the user is an admin
			DB::DbResult result = dbManager->select("role")->table("Users")->where("email =", sender_email)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "you are not registered user!");
			}

			QString role = result.first().value("role").toString();

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

				int accountNumber = result.first().value("account_number").toInt();

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
};

#endif // GETTRANSACTIONSHISTORYREQUEST_H
