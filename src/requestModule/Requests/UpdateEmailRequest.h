#ifndef UPDATEEMAILREQUEST_H
#define UPDATEEMAILREQUEST_H

#include "Request.h"
#include "db.h"

/**
 * @brief The UpdateEmailRequest class handles email update requests for users.
 *
 * This class processes email update requests, validates user credentials, checks for existing emails,
 * and updates the email in the database.
 */
class UpdateEmailRequest : public Request
{
private:
	DB::DatabaseManager* dbManager = nullptr; ///< Pointer to the DatabaseManager instance.

public:
	/**
     * @brief Constructor for the UpdateEmailRequest class.
     *
     * Initializes the DatabaseManager instance for handling database operations.
     */
	UpdateEmailRequest() : dbManager(DB::DatabaseManager::createInstance())
	{
		// Log to database log table (if needed)
	}

	/**
     * @brief Executes the email update request.
     *
     * This method processes the JSON request to update the user's email.
     * It validates user credentials, checks if the new email is already in use, and updates the email in the database.
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
			if (!isDBConnectionValid(dbManager))
			{
				return CreateDBConnectionError(response, data);
			}

			// Validate user credentials
			DB::DbResult result = dbManager->select("*")->table("users")->where("email =", email)->exec();
			int			 user_id = result.first().value("id").toInt();

			if (result.isEmpty())
			{
				return CreateErrorResponse(response, data, "email not found");
			}

			// change the password of that specific user
			result = dbManager->select("password")->table("users")->where("id =", user_id)->exec();
			if (result.first().value("password").toString() != password)
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
};

#endif // UPDATEEMAILREQUEST_H
