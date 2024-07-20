/**
 * @file Request.h
 *
 * @brief This file contains the declaration of the Request class, which is an abstract base class for handling different types of requests.
 *
 */

#ifndef REQUEST_H
#define REQUEST_H

#include <QJsonObject>
#include <QMutex>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMutexLocker>
#include "db.h"

/**
 * @brief The Request class is an abstract base class for handling different types of requests.
 *
 * This class provides the common interface and utility functions for processing various requests.
 * It includes methods for checking database connectivity and creating error responses.
 */
class Request
{
protected:
	/**
     * @brief Checks if the database connection is valid.
     *
     * This method verifies that the database connection manager is not null and that the database
     * is properly connected. Logs error messages if the connection fails.
     *
     * @param dbManager A pointer to the DB::DatabaseManager instance used for checking the connection.
     * @return True if the database connection is valid, false otherwise.
     */
	bool isDBConnectionValid(DB::DatabaseManager* dbManager);

	/**
     * @brief Creates a JSON response indicating a database connection error.
     *
     * This method constructs a JSON response object with a status of false and a message indicating
     * an internal server error related to database connectivity.
     *
     * @param response A reference to the QJsonObject that will be populated with the error response.
     * @param dataObj A reference to the QJsonObject that contains additional data about the error.
     * @return The updated QJsonObject containing the error response.
     */
	QJsonObject CreateDBConnectionError(QJsonObject& response, QJsonObject& dataObj);

	/**
     * @brief Creates a generic error JSON response.
     *
     * This method constructs a JSON response object with a status of false and a custom error message.
     *
     * @param response A reference to the QJsonObject that will be populated with the error response.
     * @param dataObj A reference to the QJsonObject that contains additional data about the error.
     * @param message A QString containing the error message to include in the response.
     * @return The updated QJsonObject containing the error response.
     */
	QJsonObject CreateErrorResponse(QJsonObject& response, QJsonObject& dataObj, QString message);

public:
	/**
     * @brief Virtual destructor for the Request class.
     *
     * Ensures proper cleanup of derived classes.
     */
	virtual ~Request()
	{
	}

	/**
     * @brief Pure virtual method to execute a request.
     *
     * This method must be implemented by derived classes to handle specific types of requests.
     *
     * @param jsonObj A QJsonObject containing the request data.
     * @param m A QMutex reference for thread-safe operations.
     * @return A QJsonObject containing the response data.
     */
	virtual QJsonObject execute(const QJsonObject& jsonObj, QMutex& m) = 0;
};

#endif // REQUEST_H
