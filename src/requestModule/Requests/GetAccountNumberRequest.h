#ifndef GETACCOUNTNUMBERREQUEST_H
#define GETACCOUNTNUMBERREQUEST_H

#include "Request.h"
#include "db.h"

/**
 * @brief The GetAccountNumberRequest class handles the retrieval of account numbers.
 *
 * This class processes requests to retrieve the account number associated with a user's email.
 * It performs database operations to fetch the account number and returns the result.
 */
class GetAccountNumberRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr; ///< Pointer to the DatabaseManager instance.

public:
	/**
     * @brief Constructor for the GetAccountNumberRequest class.
     *
     * Initializes the DatabaseManager instance for handling database operations.
     */
	GetAccountNumberRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// Log to database log table (if needed)
	}

	/**
     * @brief Executes the request to get the account number.
     *
     * This method processes the JSON request to retrieve the account number associated with
     * a user's email. It validates the input data, checks the database connection, and fetches
     * the account number from the database.
     *
     * @param jsonObj The JSON object containing the request data.
     * @param m The mutex to lock during the execution.
     * @return A JSON object containing the response data.
     */
	QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) override
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
			if (!isDBConnectionValid(dbManager))
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

			QJsonObject obj = result.first();

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
};

#endif // GETACCOUNTNUMBERREQUEST_H
