
#include "Setup.h"

using namespace testing;

TEST_F(handleCreateNewUserTest, onSuccess)
{
	QByteArray requestData = R"(
		{
			"Request": 8,
			"Data": {
				"email": "admin@example.com",
				"newUser": [
					{
						"first_name": "New",
						"last_name": "User",
						"email": "newUser@mail.com",
						"password": "newpass",
						"role": "user",
						"initial_balance": 500.0
					}
				]
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 8,
			"Data": {
				"status": 1,
				"message": "New user created successfully"
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

	// Check if the new user and account were recorded in the database
	DB::DbResult userResult = dbManager_->select("*")->table("users")->where("email =", "newUser@mail.com")->exec();

	ASSERT_FALSE(userResult.isEmpty());
	QJsonObject userObj = userResult.data(0); // Use the correct index

	EXPECT_THAT(userObj.value("first_name").toString().toStdString(), Eq("New"));
	EXPECT_THAT(userObj.value("last_name").toString().toStdString(), Eq("User"));
	EXPECT_THAT(userObj.value("email").toString().toStdString(), Eq("newUser@mail.com"));
	EXPECT_THAT(userObj.value("password").toString().toStdString(), Eq("newpass"));
	EXPECT_THAT(userObj.value("role").toString().toStdString(), Eq("user"));

	DB::DbResult accountResult =
		dbManager_->select("*")->table("accounts")->where("user_id =", userObj.value("id").toInt())->exec();

	ASSERT_FALSE(accountResult.isEmpty());
	QJsonObject accountObj = accountResult.data(0);
	EXPECT_THAT(accountObj.value("balance").toDouble(), DoubleEq(500.0));
}

TEST_F(handleCreateNewUserTest, onUserAlreadyExists)
{
	QByteArray requestData = R"(
		{
			"Request": 8,
			"Data": {
				"email": "admin@example.com",
				"newUser": [
					{
						"first_name": "New",
						"last_name": "User",
						"email": "user1@example.com",
						"password": "newpass",
						"role": "user",
						"initial_balance": 500.0
					}
				]
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 8,
			"Data": {
				"status": 0,
				"message": "User already exists"
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

TEST_F(handleCreateNewUserTest, onUserNotAdmin)
{
	QByteArray requestData = R"(
		{
			"Request": 8,
			"Data": {
				"email": "user1@example.com",
				"newUser": [
					{
						"first_name": "New",
						"last_name": "User",
						"email": "newUser@example.com",
						"password": "newpass",
						"role": "user",
						"initial_balance": 500.0
					}
				]
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 8,
			"Data": {
				"status": 0,
				"message": "Cannot create new user. User is not an admin"
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
