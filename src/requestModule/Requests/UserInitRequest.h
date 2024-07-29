/**
 * @file UserInitRequest.h
 *
 * @brief This file contains the declaration of the UserInitRequest class, which handles user initialization requests for User Widget and Admin Widget.
 * @image html UserInitRequest.svg
 */

#ifndef USERINITREQUEST_H
#define USERINITREQUEST_H

#include "Request.h"
#include "db.h"

/**
 * @brief The UserInitRequest class handles user initialization requests for User Widget and Admin Widget
 *
 * This class processes requests to initialize user information based on their
 * credentials. It retrieves user details and account information if applicable,
 * and prepares a response with the user's role and associated data.
 */
class UserInitRequest : public Request
{
private:
    DB::DatabaseManager* dbManager = nullptr; ///< Pointer to the DatabaseManager instance.

public:
    /**
     * @brief Constructor for the UserInitRequest class.
     *
     * Initializes the DatabaseManager instance for handling database operations.
     */
    UserInitRequest() : dbManager(DB::DatabaseManager::createInstance())
    {
        // Log to database log table (if needed)
    }

    /**
     * @brief Executes the user initialization request.
     *
     * This method processes the JSON request to initialize user information by validating
     * the user's credentials and retrieving their details from the database. If the user
     * is a regular user, it also retrieves their account number and current balance.
     *
     * @param jsonObj The JSON object containing the request data.
     * @param m The mutex to lock during the execution.
     * @return A JSON object containing the response data.
     */
	QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) override
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
			if (!isDBConnectionValid(dbManager))
			{
				return CreateDBConnectionError(response, data);
			}

			// Validate user credentials
			DB::DbResult result = dbManager->select("*")->table("users")->where("email =", email)->exec();
			int			 user_id = result.first().value("id").toInt();
			QJsonObject	 userObj = result.first();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "email not found");
			}

			result = dbManager->select("password")->table("users")->where("id =", user_id)->exec();
			if (result.first().value("password").toString() != password)
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

				QJsonObject accountObj = accountResult.first();

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
};

#endif // USERINITREQUEST_H
