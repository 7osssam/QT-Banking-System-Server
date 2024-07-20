// LoginRequest.h
#ifndef LOGINREQUEST_H
#define LOGINREQUEST_H

#include "Request.h"
#include "db.h"

/**
 * @brief The LoginRequest class handles user login requests.
 *
 * This class processes login requests, validates user credentials, and returns the login status.
 */
class LoginRequest : public Request
{
private:
    DB::DatabaseManager* dbManager = nullptr; ///< Pointer to the DatabaseManager instance.

public:
    /**
     * @brief Constructor for the LoginRequest class.
     *
     * Initializes the DatabaseManager instance for handling database operations.
     */
    LoginRequest() : dbManager(DB::DatabaseManager::createInstance())
    {
        // Log to database log table (if needed)
    }

    /**
     * @brief Executes the login request.
     *
     * This method processes the JSON request to validate user credentials.
     * It checks the database for the provided email and password, and returns the login status.
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
			if (!isDBConnectionValid(dbManager))
			{
				return CreateDBConnectionError(response, data);
			}

			DB::DbResult result = dbManager->select("*")->table("users")->where("email =", email)->exec();
			int			 user_id = result.first().value("id").toInt();
			QJsonObject	 obj = result.first();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "email not found");
			}

			result = dbManager->select("password")->table("users")->where("id =", user_id)->exec();
			if (result.first().value("password").toString() != password)
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
};

#endif // LOGINREQUEST_H
