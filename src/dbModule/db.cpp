

#include "db.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QStringBuilder>
#include <QThreadStorage>
#include <QUuid>

using namespace DB;

static DatabaseConfig dbConfig;
const QString		  dbDriver = "QPSQL";

QStringList DatabaseManager::Created_ = QStringList();
QStringList DatabaseManager::Update_ = QStringList();
QStringList DatabaseManager::SoftDelete_ = QStringList();

static QThreadStorage<QSqlDatabase> mDatabasePool;

DatabaseManager::DatabaseManager() : Parent_(NULL)
{
}

DatabaseManager::~DatabaseManager()
{
	if (Parent_ == NULL)
	{
		for (int i = 0; i < Childs_.size(); i++)
			delete Childs_[i];
	}
}

DatabaseManager* DatabaseManager::createInstance(bool checkDBName, bool newConnection, QString* err)
{
	// Check if dbConfig has been set
	if (dbConfig.host.isEmpty() || dbConfig.username.isEmpty() || dbConfig.dbName.isEmpty())
	{
		qCritical() << "Database configuration not set. Call setDbSetting first.";
		if (err != nullptr)
			*err = "Database configuration not set. Call setDbSetting first.";
		return nullptr;
	}

	DatabaseManager* db = new DatabaseManager();
	if (db->init(dbConfig.host, dbConfig.port, dbConfig.username, dbConfig.password, dbConfig.dbName, checkDBName,
				 newConnection))
	{
		return db;
	}

	qCritical() << "Error openning database connection :" << db->lastError().text();
	if (err != nullptr)
		*err = db->lastError().text();
	delete db;
	return nullptr;
}

bool DatabaseManager::setDbSetting(const QString& host, int port, const QString& username, const QString& password,
								   const QString& dbname)
{
	dbConfig.set(host, port, username, password, dbname);

	qDebug() << "DB Setting : ";
	qDebug() << " Host : " << dbConfig.host;
	qDebug() << " Port : " << dbConfig.port;
	qDebug() << " Username : " << dbConfig.username;
	qDebug() << " Password : " << dbConfig.password;
	qDebug() << " DB Name : " << dbConfig.dbName;
	return true;
	/*auto db = createInstance();
    bool ret = db != nullptr;
    if(db) delete db;
    return ret;*/
}

bool DatabaseManager::checkConnection(QString& error)
{
	QString err;
	auto	db = createInstance(true, false, &err);
	if (db == nullptr)
	{
		error = err;
		return false;
	}
	delete db;
	return true;
}

DatabaseManager* DatabaseManager::reset()
{
	Where_ = QString();
	Select_ = QString();
	Join_ = QString();
	Table_ = QString();
	Limit_ = QString();
	Start_ = QString();
	Sort_ = QString();
	Group_ = QString();
	return this;
}

DatabaseManager* DatabaseManager::select(const QString& value)
{
	if (!Select_.isEmpty())
		Select_.append(QStringLiteral(","));
	Select_.append(value);
	return this;
}

DatabaseManager* DatabaseManager::table(const QString& value)
{
	Table_ = value;
	return this;
}

DatabaseManager* DatabaseManager::where(const QString& value, const QVariant& val)
{
	if (!Where_.isEmpty())
		Where_.append(QStringLiteral(" AND "));
	if (!val.isValid())
		Where_.append(value);
	else
	{
		Where_.append(value);
		if (val.typeId() == QVariant::String)
			Where_.append(QStringLiteral("'") + val.toString() + QStringLiteral("'"));
		else
			Where_.append(QString::number(val.toInt()));
	}
	return this;
}

DatabaseManager* DatabaseManager::like(const QString& value, const QString& val)
{
	if (!Where_.isEmpty())
		Where_.append(QStringLiteral(" AND "));
	Where_.append(value);
	if (val.startsWith(QStringLiteral("%")) || val.endsWith(QStringLiteral("%")))
	{
		Where_.append(QStringLiteral(" LIKE '"));
		Where_.append(val);
		Where_.append(QStringLiteral("'"));
	}
	else
	{
		Where_.append(QStringLiteral(" LIKE '%"));
		Where_.append(val);
		Where_.append(QStringLiteral("%'"));
	}
	return this;
}

DatabaseManager* DatabaseManager::likePost(const QString& value, const QString& val)
{
	if (!Where_.isEmpty())
		Where_.append(QStringLiteral(" AND "));
	Where_.append(value);
	Where_.append(QStringLiteral(" LIKE '"));
	Where_.append(val);
	Where_.append(QStringLiteral("%'"));
	return this;
}

DatabaseManager* DatabaseManager::likeNative(const QString& value, const QString& val)
{
	if (!Where_.isEmpty())
		Where_.append(QStringLiteral(" AND "));
	Where_.append(value);
	Where_.append(QStringLiteral(" LIKE '"));
	Where_.append(val);
	Where_.append(QStringLiteral("'"));
	return this;
}

DatabaseManager* DatabaseManager::whereOr(const QString& value)
{
	if (!Where_.isEmpty())
		Where_.append(QStringLiteral(" OR "));
	Where_.append(value);
	return this;
}

DatabaseManager* DatabaseManager::join(const QString& value)
{
	if (!Join_.isEmpty())
		Join_.append(QStringLiteral(" "));
	Join_.append(value);
	return this;
}

DatabaseManager* DatabaseManager::limit(int limit)
{
	Limit_ = QString::number(limit);
	return this;
}

DatabaseManager* DatabaseManager::start(int start)
{
	Start_ = QString::number(start);
	return this;
}

DatabaseManager* DatabaseManager::sort(const QString& sort)
{
	if (!Sort_.isEmpty())
		Sort_.append(QStringLiteral(", "));
	Sort_.append(sort);
	return this;
}

DatabaseManager* DatabaseManager::group(const QString& group)
{
	Group_ = group;
	return this;
}

QString DatabaseManager::getSelectQuery(const QString& select)
{
	QString sql = QString(QStringLiteral("SELECT "));
	/*if(SoftDelete_.contains(Table_))
        where(QStringLiteral("deleted = 0"));*/
	if (Select_.isEmpty())
		Select_.append(QStringLiteral("*"));
	if (!select.isEmpty())
		sql.append(select);
	else
		sql.append(Select_);
	sql.append(QStringLiteral(" FROM ")).append(Table_).append(QStringLiteral(" "));
	if (!Join_.isEmpty())
		sql.append(Join_);
	if (!Where_.isEmpty())
		sql.append(QStringLiteral(" WHERE ")).append(Where_);
	if (!Group_.isEmpty())
		sql.append(QStringLiteral(" GROUP BY ")).append(Group_);
	if (!Sort_.isEmpty())
		sql.append(QStringLiteral(" ORDER BY ")).append(Sort_);
	if (!Limit_.isEmpty())
		sql.append(QStringLiteral(" LIMIT ")).append(Limit_);
	if (!Start_.isEmpty())
		sql.append(QStringLiteral(" OFFSET ")).append(Start_);

	qDebug() << sql;
	return sql;
}

QSqlError DatabaseManager::lastError()
{
	return LastError_;
}

QVariant DatabaseManager::lastInsertedId()
{
	return InsertedId_;
}

QString DatabaseManager::lastQuery()
{
	return LastQuery_;
}

DbResult DatabaseManager::exec()
{
	QList<QVariant> result;
	QSqlQuery		query(getDatabase());
	if (query.exec(getSelectQuery()))
	{
		QSqlRecord record = query.record();
		while (query.next())
		{
			QVariantMap map;
			for (int i = 0; i < record.count(); i++)
				map.insert(record.fieldName(i), query.value(i));
			result.append(map);
		}
	}
	reset();
	postQuery(&query);
	return DbResult(result);
}

DbResult DatabaseManager::execResult(const QString& sql)
{
	QList<QVariant> result;
	QSqlQuery		query(getDatabase());
	if (query.exec(sql))
	{
		QSqlRecord record = query.record();
		while (query.next())
		{
			QVariantMap map;
			for (int i = 0; i < record.count(); i++)
				map.insert(record.fieldName(i), query.value(i));
			result.append(map);
		}
	}
	reset();
	postQuery(&query);
	return DbResult(result);
}

DbResult DatabaseManager::get(const QString& tableName)
{
	return table(tableName)->exec();
}

bool DatabaseManager::exec(const QString& sqlcommand)
{
	QSqlQuery query(getDatabase());
	bool	  ok = query.exec(sqlcommand);
	reset();
	postQuery(&query);

	qDebug() << sqlcommand;
	if (!ok)
	{
		qCritical() << lastQuery();
		qCritical() << LastError_.text();
	}
	return ok;
}

bool DatabaseManager::insert(const QString& table, const QVariantMap& data)
{
	QSqlQuery query(getDatabase());
	QString	  sql = QString(QStringLiteral("INSERT INTO "));
	QString	  values = QString(QStringLiteral("VALUES ("));
	sql.append(table).append(QStringLiteral(" ("));
	const QList<QString>& keys = data.keys();
	for (int i = 0; i < keys.size(); i++)
	{
		sql.append(keys.at(i));
		values.append(QStringLiteral("?"));
		if (i != keys.size() - 1)
		{
			sql.append(QStringLiteral(","));
			values.append(QStringLiteral(","));
		}
	}
	sql.append(QStringLiteral(") ")).append(values).append(QStringLiteral(")"));
	query.prepare(sql);
	for (int i = 0; i < keys.size(); i++)
		query.bindValue(i, data[keys.at(i)]);
	reset();
	bool res = query.exec();

	qDebug() << query.lastQuery() << dataToString(data);
	postQuery(&query);
	if (!res)
	{
		qCritical() << query.lastQuery() << dataToString(data);
		qCritical() << LastError_.text();
	}
	return res;
}

bool DatabaseManager::update(const QString& table, const QVariantMap& data)
{
	QSqlQuery query(getDatabase());
	QString	  sql = QString(QStringLiteral("UPDATE "));
	sql.append(table).append(QStringLiteral(" SET "));
	if (Update_.contains(table))
	{
		sql.append(QStringLiteral("updated_at = NOW() "));
	}
	const QList<QString>& keys = data.keys();
	for (int i = 0; i < keys.size(); i++)
	{
		sql.append(keys.at(i));
		if (data.value(keys.at(i)).typeId() != QVariant::Invalid)
		{
			sql.append(QStringLiteral(" = ?"));
		}
		if (i != keys.size() - 1)
			sql.append(QStringLiteral(","));
	}
	sql.append(QStringLiteral(" WHERE ")).append(Where_);
	query.prepare(sql);
	for (int i = 0; i < keys.size(); i++)
		query.bindValue(i, data[keys.at(i)]);
	reset();
	bool res = query.exec();

	qDebug() << query.lastQuery() << dataToString(data);
	postQuery(&query);
	if (!res)
	{
		qCritical() << query.lastQuery() << dataToString(data);
		qCritical() << LastError_.text();
	}
	return res;
}

bool DatabaseManager::del(const QString& table)
{
	if (SoftDelete_.contains(table))
	{
		QVariantMap map;
		map.insert(QStringLiteral("deleted"), 1);
		return update(table, map);
	}
	else
	{
		QSqlQuery query(getDatabase());
		QString	  sql = QString(QStringLiteral("DELETE FROM "));
		sql.append(table).append(QStringLiteral(" WHERE ")).append(Where_);
		reset();
		bool res = query.exec(sql);

		qDebug() << query.lastQuery();
		postQuery(&query);
		if (!res)
		{
			qCritical() << query.lastQuery();
			qCritical() << LastError_.text();
		}
		return res;
	}
}

int DatabaseManager::count()
{
	QSqlQuery		 query(getDatabase());
	DatabaseManager* db = clone();
	QString			 sql = db->getSelectQuery(QStringLiteral("count(*)"));
	query.exec(sql);
	postQuery(&query);
	if (query.next())
	{
		return query.value(0).toInt();
	}
	return 0;
}

DatabaseManager* DatabaseManager::clone()
{
	DatabaseManager* db = new DatabaseManager();
	*db = *this;
	db->Parent_ = this;
	this->Childs_.push_back(db);
	return db;
}

bool DatabaseManager::beginTransaction()
{
	return getDatabase().transaction();
}

bool DatabaseManager::commit()
{
	return getDatabase().commit();
}

bool DatabaseManager::rollback()
{
	return getDatabase().rollback();
}

bool DatabaseManager::truncateTable(const QString& table)
{
	this->exec(QString("TRUNCATE TABLE %1").arg(table));

	return true;
}

QSqlDatabase DatabaseManager::getDatabase()
{
	if (mDatabasePool.hasLocalData())
	{
		return mDatabasePool.localData();
	}
	else
	{
		auto database = QSqlDatabase::addDatabase(dbDriver, QUuid::createUuid().toString());
		mDatabasePool.setLocalData(database);

		return database;
	}
}

bool DatabaseManager::init(const QString& host, int port, const QString& username, const QString& password,
						   const QString& dbname, bool checkDBName, bool newConnection)
{
	auto database = getDatabase();
	bool ret = false;

	if (database.isOpen())
	{
		if (newConnection)
			database.close();
		else
		{
			isSupportTransaction_ = database.driver()->hasFeature(QSqlDriver::Transactions);
			return true;
		}
	}

	database.setHostName(host);
	database.setPort(port);

	if (checkDBName)
	{
		database.open();
		QSqlQuery query(database);
		query.exec(QString("CREATE DATABASE IF NOT EXISTS %1").arg(dbname));
		database.close();
	}

	database.setDatabaseName(dbname);
	database.setUserName(username);
	database.setPassword(password);

	ret = database.open();

	isSupportTransaction_ = database.driver()->hasFeature(QSqlDriver::Transactions);
	qDebug() << "Database support transaction :" << isSupportTransaction_;

	qDebug() << "Database open status :" << ret << ":" << database.lastError().text();
	if (!ret)
		LastError_ = database.lastError();
	reset();
	return ret;
}

void DatabaseManager::postQuery(QSqlQuery* query)
{
	LastError_ = query->lastError();
	LastQuery_ = query->lastQuery();
	InsertedId_ = query->lastInsertId();
}

QString DatabaseManager::dataToString(const QVariantMap& map)
{
	QString str;
	str.append(QStringLiteral("{"));
	QMapIterator<QString, QVariant> i(map);
	while (i.hasNext())
	{
		i.next();
		str.append(i.key()).append(QStringLiteral(": ")).append(i.value().toString()).append(QStringLiteral(","));
	}
	str.append(QStringLiteral("}"));
	return str;
}
