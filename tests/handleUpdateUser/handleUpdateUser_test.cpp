
#include "Setup.h"

using namespace testing;

TEST_F(handleUpdateUserTest, onSuccessfulUpdate)
{
	QByteArray requestData = R"(
		{
			"Request": 10,
			"Data": {
				"email": "admin@example.com",
				"account_number": 111,
				"newData":{
						"first_name": "NewFirstName",
						"last_name": "NawLastName",
						"email": "newUser@mail.com",
						"role": "user"
					}
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 10,
			"Data": {
				"status": 1,
				"message": "User updated successfully"
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

	// Check if the user is updated in the database
	DB::DbResult userResult = dbManager_->select("*")->table("users")->where("email =", "newUser@mail.com")->exec();

	ASSERT_FALSE(userResult.isEmpty());

	QJsonObject userObj = userResult.data(0); // Use the correct index

	EXPECT_THAT(userObj.value("first_name").toString().toStdString(), Eq("NewFirstName"));
	EXPECT_THAT(userObj.value("last_name").toString().toStdString(), Eq("NawLastName"));
	EXPECT_THAT(userObj.value("email").toString().toStdString(), Eq("newUser@mail.com"));
	EXPECT_THAT(userObj.value("role").toString().toStdString(), Eq("user"));

	// Check if the old data is removed from the database
	DB::DbResult oldUserResult = dbManager_->select("*")->table("users")->where("email =", "user1@example.com")->exec();

	EXPECT_TRUE(oldUserResult.isEmpty());
}

TEST_F(handleUpdateUserTest, onUnauthorizedUsertryToUpdate)
{
	QByteArray requestData = R"(
		{
			"Request": 10,
			"Data": {
				"email": "user1@example.com",
				"account_number": 222,
				"newData":{
						"first_name": "NewFirstName",
						"last_name": "NawLastName",
						"email": "newUser@mail.com",
						"role": "user"
					}
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 10,
			"Data": {
				"status": 0,
				"message": "Unauthorized, Cannot update user"
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

TEST_F(handleUpdateUserTest, onAccountNotFound)
{
	QByteArray requestData = R"(
		{
			"Request": 10,
			"Data": {
				"email": "admin@example.com",
				"account_number": 333,
				"newData":{
						"first_name": "NewFirstName",
						"last_name": "NawLastName",
						"email": "newUser@mail.com",
						"role": "user"
					}
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 10,
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

TEST_F(handleUpdateUserTest, onEmailAlreadyExistsDifferentAccount)
{
	QByteArray requestData = R"(
		{
			"Request": 10,
			"Data": {
				"email": "admin@example.com",
				"account_number": 222,
				"newData":{
						"first_name": "NewFirstName",
						"last_name": "NawLastName",
						"email": "user1@example.com",
						"role": "user"
					}
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 10,
			"Data": {
				"status": 0,
				"message": "Email is associated with another account"
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

TEST_F(handleUpdateUserTest, onEmailAlreadyExistsSameAccount)
{
	QByteArray requestData = R"(
		{
			"Request": 10,
			"Data": {
				"email": "admin@example.com",
				"account_number": 111,
				"newData":{
						"first_name": "NewFirstName",
						"last_name": "NawLastName",
						"email": "user1@example.com",
						"role": "user"
					}
			}
		}
	)";

	QByteArray expectedResponse = R"(
		{
			"Response": 10,
			"Data": {
				"status": 1,
				"message": "User updated successfully"
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
