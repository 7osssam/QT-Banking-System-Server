
#include "Setup.h"

using namespace testing;

TEST_F(handleUpdatePasswordTest, onSuccessfulUpdatePassword)
{
	QByteArray requestData = R"(
		{
			"Request": 13,
			"Data": {
				"email": "user1@example.com",
				"password": "pass1",
				"new_password": "NewPassword"
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 13,
			"Data": {
				"status": 1,
				"message": "Password updated successfully"
			}
		}
	)";

	// get user id of old email
	DB::DbResult result = dbManager_->select("id")->table("users")->where("password =", "pass1")->exec();

	EXPECT_THAT(result.size(), Eq(1));

	int userId_1 = result.first().value("id").toInt();

	QByteArray actualResponse = handler_->makeRequest(requestData);

	// Ensure to strip whitespace before comparing
	QJsonDocument expectedDoc = QJsonDocument::fromJson(expectedResponse.trimmed());
	QJsonDocument actualDoc = QJsonDocument::fromJson(actualResponse.trimmed());

	qDebug() << "Expected: " << expectedDoc;
	qDebug() << "Actual: " << actualDoc;

	EXPECT_EQ(expectedDoc, actualDoc);

	//
	result = dbManager_->select("id")->table("users")->where("password =", "NewPassword")->exec();
	EXPECT_THAT(result.size(), Eq(1));

	int userId_2 = result.first().value("id").toInt();

	EXPECT_EQ(userId_1, userId_2); // Ensure the user id remains the same
}

TEST_F(handleUpdatePasswordTest, onInvalidPassword)
{
	QByteArray requestData = R"(
		{
			"Request": 13,
			"Data": {
				"email": "user1@example.com",
				"password": "notuser1password",
				"new_password": "NewPassword"
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 13,
			"Data": {
				"status": 0,
				"message": "Invalid password"
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
