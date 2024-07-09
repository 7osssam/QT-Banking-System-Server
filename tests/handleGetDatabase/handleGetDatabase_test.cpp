#include "Setup.h"

using namespace testing;

TEST_F(handleGetDatabaseTest, onSuccess)
{
    QByteArray requestData = R"(
        {
            "Request": 7,
            "Data": {
                "email": "admin@example.com"
            }
        }
    )";

	QByteArray expectedResponse = R"(
        {
            "Response": 7,
            "Data": {
                "status": 1,
                "message": "Database fetched successfully",
                "users": [
                    {
                        "account_number": null,
                        "first_name": "Admin",
                        "last_name": "Test",
                        "email": "admin@example.com",
                        "role": "admin",
                        "balance": null
                    },
                    {
                        "account_number": 111,
                        "first_name": "User1",
                        "last_name": "Test1",
                        "email": "user1@example.com",
                        "role": "user",
                        "balance": 1000.00
                    },
                    {
                        "account_number": 2222,
                        "first_name": "User2",
                        "last_name": "Test2",
                        "email": "user2@example.com",
                        "role": "user",
                        "balance": 150.00
                    }
					
                ]
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

TEST_F(handleGetDatabaseTest, onUserNotAdmin)
{
    QByteArray requestData = R"(
        {
            "Request": 7,
            "Data": {
                "email": "user1@example.com"
            }
        }
    )";

    QByteArray expectedResponse = R"(
        {
            "Response": 7,
            "Data": {
                "status": 0,
                "message": "Cannot get database. User is not an admin"
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

TEST_F(handleGetDatabaseTest, onEmailNotFound)
{
    QByteArray requestData = R"(
        {
            "Request": 7,
            "Data": {
                "email": "nonexistent@example.com"
            }
        }
    )";

    QByteArray expectedResponse = R"(
        {
            "Response": 7,
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