/**
 * @file GetBalanceRequest.h
 *
 * @brief This file contains the declaration of the GetBalanceRequest class, which handles requests to retrieve account balances.
 * @image html GetAccountNumberRequest.svg
 */

#ifndef GETBALANCEREQUEST_H
#define GETBALANCEREQUEST_H

#include "Request.h"
#include "db.h"

/**
 * @brief The GetBalanceRequest class handles the retrieval of account balances.
 *
 * This class processes requests to retrieve the balance associated with an account number.
 * It performs database operations to fetch the balance and returns the result.
 */
class GetBalanceRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr; ///< Pointer to the DatabaseManager instance.

public:
	/**
     * @brief Constructor for the GetBalanceRequest class.
     *
     * Initializes the DatabaseManager instance for handling database operations.
     */
	GetBalanceRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// Log to database log table (if needed)
	}

	/**
     * @brief Executes the request to get the account balance.
     *
     * This method processes the JSON request to retrieve the balance associated with
     * an account number. It validates the input data, checks the database connection,
     * and fetches the balance from the database.
     *
     * @param jsonObj The JSON object containing the request data.
     * @param m The mutex to lock during the execution.
     * @return A JSON object containing the response data.
     */
	QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) override
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
			if (!isDBConnectionValid(dbManager))
			{
				return CreateDBConnectionError(response, data);
			}

			DB::DbResult result =
				dbManager->select("balance")->table("Accounts")->where("account_number =", accountNumber)->exec();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "Account not found");
			}

			QJsonObject obj = result.first();

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
};

#endif // GETBALANCEREQUEST_H
