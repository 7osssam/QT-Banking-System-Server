/**
 * @file db.h
 * @brief Database management classes for handling database connections and operations.
 * @details This file contains the declarations for the DatabaseConfig structure and DatabaseManager class, which are used for configuring database connections and executing various SQL operations.
 * @image html DatabaseManager_init.svg
 */

#ifndef DB_H
#define DB_H

#include "dbresult.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

namespace DB
{

/**
 * @struct DatabaseConfig
 * @brief Configuration settings for database connections.
 * @details This structure holds the necessary parameters for configuring a database connection, including host, port, username, password, and database name.
 */
struct DatabaseConfig
{
	/**
     * @brief Default constructor for DatabaseConfig.
     */
	DatabaseConfig()
	{
	}

	/**
     * @brief Parameterized constructor for DatabaseConfig.
     * @param host Database host.
     * @param port Database port.
     * @param username Database username.
     * @param password Database password.
     * @param dbname Database name.
     */
	DatabaseConfig(const QString& host, int port, const QString& username, const QString& password,
				   const QString& dbname) :
		port(port), host(host), username(username), password(password), dbName(dbname)
	{
	}

	/**
     * @brief Sets the database configuration parameters.
     * @param host Database host.
     * @param port Database port.
     * @param username Database username.
     * @param password Database password.
     * @param dbname Database name.
     */
	void set(const QString& host, int port, const QString& username, const QString& password, const QString& dbname)
	{
		this->port = port;
		this->host = host;
		this->username = username;
		this->password = password;
		this->dbName = dbname;
	}

	int		port;	  ///< Database port.
	QString host;	  ///< Database host.
	QString username; ///< Database username.
	QString password; ///< Database password.
	QString dbName;	  ///< Database name.
};

/**
 * @class DatabaseManager
 * @brief Manages database connections and SQL operations.
 * @details This class provides methods for managing database connections, executing SQL queries, and handling transactions.
 */
class DatabaseManager
{
private:
	/**
     * @brief Private constructor for singleton pattern.
     */
	DatabaseManager();

public:
	/**
     * @brief Destructor for DatabaseManager.
     */
	~DatabaseManager();

	/**
     * @brief Creates an instance of DatabaseManager.
     * @param checkDBName Flag to check database name.
     * @param newConnection Flag to create a new connection.
     * @param err Pointer to store error message.
     * @return Pointer to DatabaseManager instance.
     */
	static DatabaseManager* createInstance(bool checkDBName = false, bool newConnection = false,
										   QString* err = nullptr);

	/**
     * @brief Sets the database settings.
     * @param host Database host.
     * @param port Database port.
     * @param username Database username.
     * @param password Database password.
     * @param dbname Database name.
     * @return True if settings are set successfully, false otherwise.
     */
	static bool setDbSetting(const QString& host, int port, const QString& username, const QString& password,
							 const QString& dbname);

	/**
     * @brief Checks the database connection.
     * @param error Reference to store error message.
     * @return True if connection is successful, false otherwise.
     */
	static bool checkConnection(QString& error);

	/**
     * @brief Resets the query settings.
     * @return Pointer to the DatabaseManager instance.
     */
	DatabaseManager* reset();

	/**
     * @brief Selects columns for the query.
     * @param value Columns to select.
     * @return Pointer to the DatabaseManager instance.
     */
	DatabaseManager* select(const QString& value);

	/**
     * @brief Sets the table for the query.
     * @param value Table name.
     * @return Pointer to the DatabaseManager instance.
     */
	DatabaseManager* table(const QString& value);

	/**
     * @brief Adds a WHERE clause to the query.
     * @param value Condition for the WHERE clause.
     * @param val Value for the condition.
     * @return Pointer to the DatabaseManager instance.
     */
	DatabaseManager* where(const QString& value, const QVariant& val = QVariant());

	/**
     * @brief Adds a LIKE clause to the query.
     * @param value Column name.
     * @param val Value for the LIKE clause.
     * @return Pointer to the DatabaseManager instance.
     */
	DatabaseManager* like(const QString& value, const QString& val);

	/**
     * @brief Adds a LIKE clause with a postfix wildcard to the query.
     * @param value Column name.
     * @param val Value for the LIKE clause.
     * @return Pointer to the DatabaseManager instance.
     */
	DatabaseManager* likePost(const QString& value, const QString& val);

	/**
     * @brief Adds a native LIKE clause to the query.
     * @param value Column name.
     * @param val Value for the LIKE clause.
     * @return Pointer to the DatabaseManager instance.
     */
	DatabaseManager* likeNative(const QString& value, const QString& val);

	/**
     * @brief Adds an OR condition to the WHERE clause.
     * @param value Condition for the OR clause.
     * @return Pointer to the DatabaseManager instance.
     */
	DatabaseManager* whereOr(const QString& value);

	/**
     * @brief Adds a JOIN clause to the query.
     * @param value JOIN clause.
     * @return Pointer to the DatabaseManager instance.
     */
	DatabaseManager* join(const QString& value);

	/**
     * @brief Adds a LIMIT clause to the query.
     * @param limit Number of records to limit.
     * @return Pointer to the DatabaseManager instance.
     */
	DatabaseManager* limit(int limit);

	/**
     * @brief Adds an OFFSET clause to the query.
     * @param start Offset value.
     * @return Pointer to the DatabaseManager instance.
     */
	DatabaseManager* start(int start);

	/**
     * @brief Adds an ORDER BY clause to the query.
     * @param sort Sorting criteria.
     * @return Pointer to the DatabaseManager instance.
     */
	DatabaseManager* sort(const QString& sort);

	/**
     * @brief Adds a GROUP BY clause to the query.
     * @param group Grouping criteria.
     * @return Pointer to the DatabaseManager instance.
     */
	DatabaseManager* group(const QString& group);

	/**
     * @brief Gets the SQL SELECT query.
     * @param select Optional select clause.
     * @return SQL SELECT query as a string.
     */
	QString getSelectQuery(const QString& select = QString());

	/**
     * @brief Gets the last error occurred.
     * @return QSqlError object containing the last error.
     */
	QSqlError lastError();

	/**
     * @brief Gets the ID of the last inserted record.
     * @return ID of the last inserted record.
     */
	QVariant lastInsertedId();

	/**
     * @brief Gets the last executed query.
     * @return Last executed query as a string.
     */
	QString lastQuery();

	/**
     * @brief Executes the built query.
     * @return DbResult object containing the result of the query.
     */
	DbResult exec();

	/**
     * @brief Executes a SQL query.
     * @param sql SQL query to execute.
     * @return DbResult object containing the result of the query.
     */
	DbResult execResult(const QString& sql);

	/**
     * @brief Executes a SELECT query on a specified table.
     * @param tableName Name of the table.
     * @return DbResult object containing the result of the query.
     */
	DbResult get(const QString& tableName);

	/**
     * @brief Executes a SQL command.
     * @param sqlcommand SQL command to execute.
     * @return True if the command executed successfully, false otherwise.
     */
	bool exec(const QString& sqlcommand);

	/**
     * @brief Inserts data into a specified table.
     * @param table Name of the table.
     * @param data Data to insert.
     * @return True if the data was inserted successfully, false otherwise.
     */
	bool insert(const QString& table, const QVariantMap& data);

	/**
     * @brief Updates data in a specified table.
     * @param table Name of the table.
     * @param data Data to update.
     * @return True if the data was updated successfully, false otherwise.
     */
	bool update(const QString& table, const QVariantMap& data);

	/**
     * @brief Deletes data from a specified table.
     * @param table Name of the table.
     * @return True if the data was deleted successfully, false otherwise.
     */
	bool del(const QString& table);

	/**
     * @brief Gets the count of records matching the query criteria.
     * @return Count of records.
     */
	int count();

	/**
     * @brief Clones the current DatabaseManager instance.
     * @return Pointer to the cloned DatabaseManager instance.
     */
	DatabaseManager* clone();

	/**
     * @brief Begins a transaction.
     * @return True if the transaction started successfully, false otherwise.
     */
	bool beginTransaction();

	/**
     * @brief Commits a transaction.
     * @return True if the transaction was committed successfully, false otherwise.
     */
	bool commit();

	/**
     * @brief Rolls back a transaction.
     * @return True if the transaction was rolled back successfully, false otherwise.
     */
	bool rollback();

	/**
     * @brief Checks if transactions are supported.
     * @return True if transactions are supported, false otherwise.
     */
	inline bool isSupportTransaction()
	{
		return isSupportTransaction_;
	}

	/**
     * @brief Clears the SELECT clause.
     * @return Pointer to the DatabaseManager instance.
     */
	inline DatabaseManager* clearSelect()
	{
		Select_.clear();
		return this;
	}

	/**
     * @brief Truncates a specified table.
     * @param table Name of the table.
     * @return True if the table was truncated successfully, false otherwise.
     */
	bool truncateTable(const QString& table);

private:
	static QStringList		Created_;					   ///< List of created tables.
	static QStringList		Update_;					   ///< List of tables with update timestamps.
	static QStringList		SoftDelete_;				   ///< List of tables with soft delete support.
	bool					isSupportTransaction_ = false; ///< Flag indicating if transactions are supported.
	QString					Where_;						   ///< WHERE clause.
	QString					Select_;					   ///< SELECT clause.
	QString					Join_;						   ///< JOIN clause.
	QString					Table_;						   ///< Table name.
	QString					Limit_;						   ///< LIMIT clause.
	QString					Start_;						   ///< OFFSET clause.
	QString					Sort_;						   ///< ORDER BY clause.
	QString					Group_;						   ///< GROUP BY clause.
	QList<DatabaseManager*> Childs_;					   ///< List of child DatabaseManager instances.
	DatabaseManager*		Parent_;					   ///< Parent DatabaseManager instance.
	QString					LastQuery_;					   ///< Last executed query.
	QVariant				InsertedId_;				   ///< ID of the last inserted record.
	QSqlError				LastError_;					   ///< Last error occurred.

	/**
     * @brief Gets the database connection.
     * @return QSqlDatabase object representing the database connection.
     */
	static QSqlDatabase getDatabase();

	/**
     * @brief Initializes the database connection.
     * @param host Database host.
     * @param port Database port.
     * @param username Database username.
     * @param password Database password.
     * @param dbname Database name.
     * @param checkDBName Flag to check database name.
     * @param newConnection Flag to create a new connection.
     * @return True if initialization was successful, false otherwise.
     */
	bool init(const QString& host, int port, const QString& username, const QString& password, const QString& dbname,
			  bool checkDBName = false, bool newConnection = false);

	/**
     * @brief Processes the query after execution.
     * @param query Pointer to the QSqlQuery object.
     */
	void postQuery(QSqlQuery* query);

	/**
     * @brief Converts data to a string format.
     * @param map Data to convert.
     * @return Data as a string.
     */
	QString dataToString(const QVariantMap& map);
};

} // namespace DB

#endif // DB_H
