#ifndef DBRESULT_H
#define DBRESULT_H

#include <QJsonArray>
#include <QJsonObject>

namespace DB
{

/**
 * @brief The DbResult class represents a result set returned from a database query.
 *
 * This class encapsulates the data returned from a database operation and provides methods
 * to access and manipulate the result. It stores the result in a JSON array format.
 */
class DbResult
{
public:
    /**
     * @brief Default constructor for DbResult.
     *
     * Initializes an empty DbResult object.
     */
    DbResult();

    /**
     * @brief Constructs a DbResult from a list of QVariant data.
     *
     * @param data A QList<QVariant> containing data to be stored in the result.
     * The data is converted into a JSON array of QJsonObjects.
     */
    DbResult(const QList<QVariant>& data);

    /**
     * @brief Checks if the result set is empty.
     *
     * @return True if the result set is empty, false otherwise.
     */
    bool isEmpty() const;

    /**
     * @brief Retrieves the first item in the result set.
     *
     * @return A QJsonObject representing the first item. If the result set is empty, returns an empty QJsonObject.
     */
    QJsonObject first() const;

    /**
     * @brief Gets the number of items in the result set.
     *
     * @return The number of items in the result set.
     */
    int size() const;

    /**
     * @brief Retrieves data at a specific position in the result set.
     *
     * @param pos The index position of the item to retrieve.
     * @return A QJsonObject representing the data at the specified position.
     * Returns an empty QJsonObject if the position is out of bounds or if the result set is empty.
     */
    QJsonObject data(int pos) const;

    /**
     * @brief Gets all the data in the result set.
     *
     * @return A QJsonArray containing all the items in the result set.
     */
    QJsonArray data() const;

    /**
     * @brief Converts the result set to a JSON string.
     *
     * @return A QString containing the JSON representation of the result set.
     */
    QString toJsonString() const;

private:
    QJsonArray Data_; ///< Holds the result data in a JSON array format.
};

} // namespace DB

#endif // DBRESULT_H
