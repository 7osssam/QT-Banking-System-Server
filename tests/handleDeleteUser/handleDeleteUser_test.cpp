
#include "Setup.h"

TEST_F(handleDeleteUserTest, onSuccess)
{
	QByteArray requestData = R"(
		{
			"Request": 9,
			"Data": {
				"email": "admin@example.com",
				"account_number": "111"
				}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 9,
			"Data": {
				"status": 1,
				"message": "User deleted successfully"
			}
		}
	)";

	QByteArray actualResponse = handler_->makeRequest(requestData);

	// Ensure to strip whitespace before comparing
	QJsonDocument expectedDoc = QJsonDocument::fromJson(expectedResponse.trimmed());
	QJsonDocument actualDoc = QJsonDocument::fromJson(actualResponse.trimmed());

	qDebug() << "Expected: " << expectedDoc;
	qDebug() << "Actual: " << actualDoc;

	EXPECT_EQ(expectedDoc, actualDoc);

	// Ensure the account is deleted from the database
	DB::DbResult result =
		dbManager_->select("account_number")->table("accounts")->where("account_number =", 111)->exec();
	EXPECT_EQ(result.size(), 0);

	// Ensure the user is deleted from the database
	result = dbManager_->select("*")->table("users")->where("email =", "user1@example.com")->exec();
	EXPECT_EQ(result.size(), 0);
}

TEST_F(handleDeleteUserTest, onInvalidAccountNumber)
{
	QByteArray requestData = R"(
		{
			"Request": 9,
			"Data": {
				"email": "admin@example.com",
				"account_number": "999"
				}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 9,
			"Data": {
				"status": 0,
				"message": "Account number does not exist"
			}
		}
	)";

	QByteArray actualResponse = handler_->makeRequest(requestData);

	// Ensure to strip whitespace before comparing
	QJsonDocument expectedDoc = QJsonDocument::fromJson(expectedResponse.trimmed());
	QJsonDocument actualDoc = QJsonDocument::fromJson(actualResponse.trimmed());

	qDebug() << "Expected: " << expectedDoc;
	qDebug() << "Actual: " << actualDoc;

	EXPECT_EQ(expectedDoc, actualDoc);
}

TEST_F(handleDeleteUserTest, onUserTryingToDeleteAnotherUser)
{
	QByteArray requestData = R"(
		{
			"Request": 9,
			"Data": {
				"email": "user1@example.com",
				"account_number": "222"
				}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 9,
			"Data": {
				"status": 0,
				"message": "Unauthorized, Cannot delete user."
			}
		}
	)";

	QByteArray actualResponse = handler_->makeRequest(requestData);

	// Ensure to strip whitespace before comparing
	QJsonDocument expectedDoc = QJsonDocument::fromJson(expectedResponse.trimmed());
	QJsonDocument actualDoc = QJsonDocument::fromJson(actualResponse.trimmed());

	qDebug() << "Expected: " << expectedDoc;
	qDebug() << "Actual: " << actualDoc;

	EXPECT_EQ(expectedDoc, actualDoc);
}