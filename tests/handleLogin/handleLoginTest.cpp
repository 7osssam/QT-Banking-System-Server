
#include "Setup.h"

TEST_F(handleLoginTest, onSuccess)
{
	QByteArray requestData = R"(
        {
            "Request": 1,
            "Data": {
                "email": "user1@example.com",
                "password": "pass1"
            }
        }
    )";

	QByteArray expectedResponse = R"(
        {
            "Response": 1,
            "Data": {
                "status": 1,
                "message": "Login successful",
                "first_name": "User1",
                "role": "user"
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

TEST_F(handleLoginTest, onEmailNotFound)
{
	QByteArray requestData = R"(
		{
			"Request": 1,
			"Data": {
				"email": "notuser@example.com",
				"password": "pass1"
				}
			}
		)";

	QByteArray expectedResponse = R"(
		{
			"Response": 1,
			"Data": {
				"status": 0,
				"message": "email not found"
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

TEST_F(handleLoginTest, onPasswordNotFound)
{
	QByteArray requestData = R"(
		{
			"Request": 1,
			"Data": {
				"email": "user1@example.com",
				"password": "Notpass"
				}
			}
		)";

	QByteArray expectedResponse = R"(
		{
			"Response": 1,
			"Data": {
				"status": 0,
				"message": "password not found"
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
