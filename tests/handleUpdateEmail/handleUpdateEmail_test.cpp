
#include "Setup.h"

using namespace testing;

TEST_F(handleUpdateEmailTest, onSuccessfulUpdateEmail)
{
	QByteArray requestData = R"(
		{
			"Request": 12,
			"Data": {
				"email": "user1@example.com",
				"password": "pass1",
				"new_email": "New@mail.com"
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 12,
			"Data": {
				"status": 1,
				"message": "Email updated successfully"
			}
		}
	)";

	// get user id of old email
	DB::DbResult result = dbManager_->select("id")->table("users")->where("email =", "user1@example.com")->exec();

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
	result = dbManager_->select("id")->table("users")->where("email =", "New@mail.com")->exec();
	EXPECT_THAT(result.size(), Eq(1));

	int userId_2 = result.first().value("id").toInt();

	EXPECT_EQ(userId_1, userId_2); // Ensure the user id remains the same
}

TEST_F(handleUpdateEmailTest, onEmailAlreadyExists)
{
	QByteArray requestData = R"(
		{
			"Request": 12,
			"Data": {
				"email": "user1@example.com",
				"password": "pass1",
				"new_email": "user2@example.com"
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 12,
			"Data": {
				"status": 0,
				"message": "Email already exists"
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

TEST_F(handleUpdateEmailTest, onInvalidPassword)
{
	QByteArray requestData = R"(
		{
			"Request": 12,
			"Data": {
				"email": "user1@example.com",
				"password": "pass2",
				"new_email": "New@mail.com"
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 12,
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