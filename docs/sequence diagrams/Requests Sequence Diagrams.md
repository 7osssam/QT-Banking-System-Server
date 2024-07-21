# Sequence Diagrams for Requests


## Sequence diagram : `LoginRequest` processes login request

```plantuml
@startuml
actor User
participant "LoginRequest" as LR
participant "QJsonObject" as JsonObj
participant "QMutex" as Mutex
participant "QMutexLocker" as Locker
participant "DB::DatabaseManager" as DB
participant "DB::DbResult" as Result

User -> LR: execute(jsonObj, m)
activate LR
LR -> Locker: QMutexLocker(&m)
activate Locker
LR -> JsonObj: extract email and password
JsonObj -> LR: return email and password

alt DB connection invalid
    LR -> LR: isDBConnectionValid(dbManager)
    LR -> LR: CreateDBConnectionError(response, dataObj)
    LR -> User: return response
    deactivate LR
else DB connection valid
    LR -> DB: select("*").table("users").where("email =", email).exec()
    activate DB
    DB -> Result: return result
    deactivate DB
    Result -> LR: return first result (user data)

    alt user not found
        LR -> LR: CreateErrorResponse(response, data, "email not found")
        LR -> User: return response
        deactivate LR
    else user found
        LR -> DB: select("password").table("users").where("id =", user_id).exec()
        activate DB
        DB -> Result: return result
        deactivate DB
        Result -> LR: return first result (password)

        alt password invalid
            LR -> LR: CreateErrorResponse(response, data, "Invalid password")
            LR -> User: return response
            deactivate LR
        else password valid
            LR -> JsonObj: construct success response
            JsonObj -> User: return response
            deactivate JsonObj
        end
    end
end
deactivate Locker
@enduml
```

## Sequence diagram : `CreateNewUserRequest ` processes create new user request

```plantuml

@startuml
actor User
participant "CreateNewUserRequest" as CNURequest
participant "QJsonObject" as JsonObj
participant "QMutex" as Mutex
participant "QMutexLocker" as Locker
participant "DB::DatabaseManager" as DB
participant "DB::DbResult" as Result

User -> CNURequest: execute(jsonObj, m)
activate CNURequest
CNURequest -> Locker: QMutexLocker(&m)
activate Locker
CNURequest -> JsonObj: extract user data
JsonObj -> CNURequest: return user data

alt DB connection invalid
    CNURequest -> CNURequest: isDBConnectionValid(dbManager)
    CNURequest -> CNURequest: CreateDBConnectionError(response, data)
    CNURequest -> User: return response
    deactivate CNURequest
else DB connection valid
    CNURequest -> DB: select("role").table("users").where("email =", email).exec()
    activate DB
    DB -> Result: return result
    deactivate DB
    Result -> CNURequest: return user role

    alt user not found
        CNURequest -> CNURequest: CreateErrorResponse(response, data, "you are not registered user!")
        CNURequest -> User: return response
        deactivate CNURequest
    else user found
        CNURequest -> CNURequest: check if role is admin
        alt role not admin
            CNURequest -> CNURequest: CreateErrorResponse(response, data, "Unauthorized, Cannot create new user.")
            CNURequest -> User: return response
            deactivate CNURequest
        else role is admin
            CNURequest -> JsonObj: validate new user data
            alt missing fields
                CNURequest -> CNURequest: CreateErrorResponse(response, data, "Missing required fields")
                CNURequest -> User: return response
                deactivate CNURequest
            else all fields present
                CNURequest -> DB: check for existing user
                DB -> Result: return result
                deactivate DB
                Result -> CNURequest: return user existence check

                alt user exists
                    CNURequest -> CNURequest: CreateErrorResponse(response, data, "User already exists")
                    CNURequest -> User: return response
                    deactivate CNURequest
                else user does not exist
                    CNURequest -> DB: insert new user
                    activate DB
                    DB -> CNURequest: return success/failure
                    deactivate DB
                    CNURequest -> JsonObj: construct success response
                    JsonObj -> User: return response
                end
            end
        end
    end
end
deactivate Locker
@enduml
```

## Sequence diagram : `DeleteUserRequest` processes delete user request

```plantuml
@startuml
actor User
participant "DeleteUserRequest" as DURequest
participant "QJsonObject" as JsonObj
participant "QMutex" as Mutex
participant "QMutexLocker" as Locker
participant "DB::DatabaseManager" as DB
participant "DB::DbResult" as Result

User -> DURequest: execute(jsonObj, m)
activate DURequest
DURequest -> Locker: QMutexLocker(&m)
activate Locker
DURequest -> JsonObj: extract admin email and account number
JsonObj -> DURequest: return admin email and account number

alt DB connection invalid
    DURequest -> DURequest: isDBConnectionValid(dbManager)
    DURequest -> DURequest: CreateDBConnectionError(response, data)
    DURequest -> User: return response
    deactivate DURequest
else DB connection valid
    DURequest -> DB: select("*").table("users").where("email =", admin_email).exec()
    activate DB
    DB -> Result: return result
    deactivate DB
    Result -> DURequest: return user role

    alt user not found
        DURequest -> DURequest: CreateErrorResponse(response, data, "you are not registered user!")
        DURequest -> User: return response
        deactivate DURequest
    else user found
        DURequest -> DURequest: check if role is admin
        alt role not admin
            DURequest -> DURequest: CreateErrorResponse(response, data, "Unauthorized, Cannot delete user.")
            DURequest -> User: return response
            deactivate DURequest
        else role is admin
            DURequest -> DB: select("*").table("accounts").where("account_number =", account_number).exec()
            activate DB
            DB -> Result: return result
            deactivate DB
            Result -> DURequest: return account existence check

            alt account not found
                DURequest -> DURequest: CreateErrorResponse(response, data, "Account number does not exist")
                DURequest -> User: return response
                deactivate DURequest
            else account found
                DURequest -> DURequest: get user_id from account
                DURequest -> DB: delete account
                activate DB
                DB -> DURequest: return success/failure
                deactivate DB

                alt account deletion failed
                    DURequest -> DURequest: CreateErrorResponse(response, data, "Failed to delete account")
                    DURequest -> User: return response
                    deactivate DURequest
                else account deleted
                    DURequest -> DB: delete user
                    activate DB
                    DB -> DURequest: return success/failure
                    deactivate DB

                    alt user deletion failed
                        DURequest -> DURequest: CreateErrorResponse(response, data, "Failed to delete user")
                        DURequest -> User: return response
                        deactivate DURequest
                    else user deleted
                        DURequest -> JsonObj: construct success response
                        JsonObj -> User: return response
                    end
                end
            end
        end
    end
end
deactivate Locker
@enduml
```

## Sequence diagram : `GetBalanceRequest` processes get balance request

```plantuml
@startuml
actor User
participant "GetBalanceRequest" as GBRequest
participant "QJsonObject" as JsonObj
participant "QMutex" as Mutex
participant "QMutexLocker" as Locker
participant "DB::DatabaseManager" as DB
participant "DB::DbResult" as Result

User -> GBRequest: execute(jsonObj, m)
activate GBRequest
GBRequest -> Locker: QMutexLocker(&m)
activate Locker
GBRequest -> JsonObj: extract account number
JsonObj -> GBRequest: return account number

alt DB connection invalid
    GBRequest -> GBRequest: isDBConnectionValid(dbManager)
    GBRequest -> GBRequest: CreateDBConnectionError(response, data)
    GBRequest -> User: return response
    deactivate GBRequest
else DB connection valid
    GBRequest -> DB: select("balance").table("Accounts").where("account_number =", accountNumber).exec()
    activate DB
    DB -> Result: return result
    deactivate DB
    Result -> GBRequest: return account balance

    alt account not found
        GBRequest -> GBRequest: CreateErrorResponse(response, data, "Account not found")
        GBRequest -> User: return response
        deactivate GBRequest
    else account found
        GBRequest -> JsonObj: construct success response
        JsonObj -> User: return response
    end
end
deactivate Locker
@enduml
```

## Sequence diagram : `GetDatabaseRequest` processes get database request

```plantuml
@startuml
actor User
participant "GetDatabaseRequest" as GDBRequest
participant "QJsonObject" as JsonObj
participant "QMutex" as Mutex
participant "QMutexLocker" as Locker
participant "DB::DatabaseManager" as DB
participant "DB::DbResult" as Result

User -> GDBRequest: execute(jsonObj, m)
activate GDBRequest
GDBRequest -> Locker: QMutexLocker(&m)
activate Locker
GDBRequest -> JsonObj: extract admin email
JsonObj -> GDBRequest: return admin email

alt DB connection invalid
    GDBRequest -> GDBRequest: isDBConnectionValid(dbManager)
    GDBRequest -> GDBRequest: CreateDBConnectionError(response, data)
    GDBRequest -> User: return response
    deactivate GDBRequest
else DB connection valid
    GDBRequest -> DB: select("role").table("Users").where("email =", admin_email).exec()
    activate DB
    DB -> Result: return result
    deactivate DB
    Result -> GDBRequest: return user role

    alt user not found
        GDBRequest -> GDBRequest: CreateErrorResponse(response, data, "you are not registered user!")
        GDBRequest -> User: return response
        deactivate GDBRequest
    else user found
        GDBRequest -> GDBRequest: check admin role

        alt not admin
            GDBRequest -> GDBRequest: CreateErrorResponse(response, data, "Unauthorized, Cannot get database.")
            GDBRequest -> User: return response
            deactivate GDBRequest
        else admin
            GDBRequest -> DB: select("*").table("Users").exec()
            activate DB
            DB -> Result: return all users
            deactivate DB
            Result -> GDBRequest: return user data

            alt no users found
                GDBRequest -> GDBRequest: CreateErrorResponse(response, data, "No data found")
                GDBRequest -> User: return response
                deactivate GDBRequest
            else users found
                GDBRequest -> DB: select("A.account_number, A.balance, U.email").table("Accounts A").join("JOIN Users U ON A.user_id = U.id").exec()
                activate DB
                DB -> Result: return account data
                deactivate DB
                Result -> GDBRequest: return account information

                GDBRequest -> JsonObj: construct success response
                JsonObj -> User: return response
            end
        end
    end
end
deactivate Locker
@enduml
```

## Sequence diagram : `GetTransactionsHistoryRequest` processes get transactions history request

```plantuml
@startuml
actor User
participant "GetTransactionsHistoryRequest" as GTHRequest
participant "QJsonObject" as JsonObj
participant "QMutex" as Mutex
participant "QMutexLocker" as Locker
participant "DB::DatabaseManager" as DB
participant "DB::DbResult" as Result

User -> GTHRequest: execute(jsonObj, m)
activate GTHRequest
GTHRequest -> Locker: QMutexLocker(&m)
activate Locker
GTHRequest -> JsonObj: extract sender email
JsonObj -> GTHRequest: return sender email

alt DB connection invalid
    GTHRequest -> GTHRequest: isDBConnectionValid(dbManager)
    GTHRequest -> GTHRequest: CreateDBConnectionError(response, data)
    GTHRequest -> User: return response
    deactivate GTHRequest
else DB connection valid
    GTHRequest -> DB: select("role").table("Users").where("email =", sender_email).exec()
    activate DB
    DB -> Result: return result
    deactivate DB
    Result -> GTHRequest: return user role

    alt user not found
        GTHRequest -> GTHRequest: CreateErrorResponse(response, data, "you are not registered user!")
        GTHRequest -> User: return response
        deactivate GTHRequest
    else user found
        GTHRequest -> GTHRequest: check user role

        alt user role
            GTHRequest -> DB: select("A.account_number").table("Users U").join("JOIN Accounts A ON U.id = A.user_id").where("U.email =", sender_email).exec()
            activate DB
            DB -> Result: return account number
            deactivate DB
            Result -> GTHRequest: return account information

            alt no account found
                GTHRequest -> GTHRequest: CreateErrorResponse(response, data, "No account found")
                GTHRequest -> User: return response
                deactivate GTHRequest
            else account found
                GTHRequest -> DB: select("*").table("Transactions").where("from_account_number =", accountNumber).whereOr("to_account_number =" + QString::number(accountNumber)).exec()
                activate DB
                DB -> Result: return transaction data
                deactivate DB
                Result -> GTHRequest: return transaction information

                alt no transactions found
                    GTHRequest -> GTHRequest: CreateErrorResponse(response, data, "No transactions found")
                    GTHRequest -> User: return response
                    deactivate GTHRequest
                else transactions found
                    GTHRequest -> JsonObj: construct success response
                    JsonObj -> User: return response
                end
            end
        else admin role
            GTHRequest -> DB: select("*").table("Transactions").exec()
            activate DB
            DB -> Result: return all transaction data
            deactivate DB
            Result -> GTHRequest: return transaction information

            alt no transactions found
                GTHRequest -> GTHRequest: CreateErrorResponse(response, data, "No transactions found")
                GTHRequest -> User: return response
                deactivate GTHRequest
            else transactions found
                GTHRequest -> JsonObj: construct success response
                JsonObj -> User: return response
            end
        end
    end
end
deactivate Locker
@enduml
```

## Sequence diagram : `MakeTransactionRequest` processes make transaction request

```plantuml
@startuml
actor User
participant "MakeTransactionRequest" as MTRRequest
participant "QJsonObject" as JsonObj
participant "QMutex" as Mutex
participant "QMutexLocker" as Locker
participant "DB::DatabaseManager" as DB
participant "DB::DbResult" as Result

User -> MTRRequest: execute(jsonObj, m)
activate MTRRequest
MTRRequest -> Locker: QMutexLocker(&m)
activate Locker
MTRRequest -> JsonObj: extract transaction details
JsonObj -> MTRRequest: return transaction details

alt DB connection invalid
    MTRRequest -> MTRRequest: isDBConnectionValid(dbManager)
    MTRRequest -> MTRRequest: CreateDBConnectionError(response, data)
    MTRRequest -> User: return response
    deactivate MTRRequest
else DB connection valid
    alt to_email provided
        MTRRequest -> DB: select("account_number").table("Users U").join("JOIN Accounts A ON U.id = A.user_id").where("U.email =", to_email).exec()
        activate DB
        DB -> Result: return account number
        deactivate DB

        alt account not found
            MTRRequest -> MTRRequest: CreateErrorResponse(response, data, "This email is not associated with any account")
            MTRRequest -> User: return response
            deactivate MTRRequest
        else account found
            MTRRequest -> MTRRequest: use to_account_number
        end
    end

    MTRRequest -> DB: select("balance").table("Accounts").where("account_number =", from_account_number).exec()
    activate DB
    DB -> Result: return from account balance
    deactivate DB

    alt from account not found
        MTRRequest -> MTRRequest: CreateErrorResponse(response, data, "You don't have an account")
        MTRRequest -> User: return response
        deactivate MTRRequest
    else from account found
        MTRRequest -> DB: select("balance").table("Accounts").where("account_number =", to_account_number).exec()
        activate DB
        DB -> Result: return to account balance
        deactivate DB

        alt to account not found
            MTRRequest -> MTRRequest: CreateErrorResponse(response, data, "Invalid to account number")
            MTRRequest -> User: return response
            deactivate MTRRequest
        else to account found
            MTRRequest -> MTRRequest: check balance
            alt insufficient balance
                MTRRequest -> MTRRequest: CreateErrorResponse(response, data, "Insufficient balance")
                MTRRequest -> User: return response
                deactivate MTRRequest
            else sufficient balance
                MTRRequest -> DB: insert("Transactions", {{"account_number", from_account_number}, {"from_account_number", from_account_number}, {"to_account_number", to_account_number}, {"amount", amount}})
                activate DB
                DB -> Result: return success
                deactivate DB

                alt transaction logging failed
                    MTRRequest -> MTRRequest: CreateErrorResponse(response, data, "Failed to log transaction")
                    MTRRequest -> User: return response
                    deactivate MTRRequest
                else transaction logged
					MTRRequest -> JsonObj: construct success response
					JsonObj -> User: return response
                end
            end
        end
    end
end
deactivate Locker
@enduml
```

## Sequence diagram : `UpdateEmailRequest` processes update email request

```plantuml
@startuml
actor User
participant "UpdateEmailRequest" as UERRequest
participant "QJsonObject" as JsonObj
participant "QMutex" as Mutex
participant "QMutexLocker" as Locker
participant "DB::DatabaseManager" as DB
participant "DB::DbResult" as Result

User -> UERRequest: execute(jsonObj, m)
activate UERRequest
UERRequest -> Locker: QMutexLocker(&m)
activate Locker
UERRequest -> JsonObj: extract email, password, new_email
JsonObj -> UERRequest: return credentials

alt DB connection invalid
    UERRequest -> UERRequest: isDBConnectionValid(dbManager)
    UERRequest -> UERRequest: CreateDBConnectionError(response, data)
    UERRequest -> User: return response
    deactivate UERRequest
else DB connection valid
    UERRequest -> DB: select("*").table("users").where("email =", email).exec()
    activate DB
    DB -> Result: return user data
    deactivate DB

    alt user not found
        UERRequest -> UERRequest: CreateErrorResponse(response, data, "email not found")
        UERRequest -> User: return response
        deactivate UERRequest
    else user found
        UERRequest -> DB: select("password").table("users").where("id =", user_id).exec()
        activate DB
        DB -> Result: return password
        deactivate DB

        alt password invalid
            UERRequest -> UERRequest: CreateErrorResponse(response, data, "Invalid password")
            UERRequest -> User: return response
            deactivate UERRequest
        else password valid
            UERRequest -> DB: select("*").table("users").where("email =", new_email).exec()
            activate DB
            DB -> Result: return existing email check
            deactivate DB

            alt email already exists
                UERRequest -> UERRequest: CreateErrorResponse(response, data, "Email already exists")
                UERRequest -> User: return response
                deactivate UERRequest
            else email available
                UERRequest -> DB: update("users", {{"email", new_email}}).where("email =", email)
                activate DB
                DB -> Result: return success
                deactivate DB

                alt update failed
                    UERRequest -> UERRequest: CreateErrorResponse(response, data, "Failed to update email")
                    UERRequest -> User: return response
                    deactivate UERRequest
                else update successful
					UERRequest -> JsonObj: construct success response
					JsonObj -> User: return response
                end
            end
        end
    end
end
deactivate Locker
@enduml
```

## Sequence diagram : `UpdatePasswordRequest` processes update password request

```plantuml
@startuml
actor User
participant "UpdatePasswordRequest" as UPRRequest
participant "QJsonObject" as JsonObj
participant "QMutex" as Mutex
participant "QMutexLocker" as Locker
participant "DB::DatabaseManager" as DB
participant "DB::DbResult" as Result

User -> UPRRequest: execute(jsonObj, m)
activate UPRRequest
UPRRequest -> Locker: QMutexLocker(&m)
activate Locker
UPRRequest -> JsonObj: extract email, password, new_password
JsonObj -> UPRRequest: return credentials

alt DB connection invalid
    UPRRequest -> UPRRequest: isDBConnectionValid(dbManager)
    UPRRequest -> UPRRequest: CreateDBConnectionError(response, data)
    UPRRequest -> User: return response
    deactivate UPRRequest
else DB connection valid
    UPRRequest -> DB: select("*").table("users").where("email =", email).exec()
    activate DB
    DB -> Result: return user data
    deactivate DB

    alt user not found
        UPRRequest -> UPRRequest: CreateErrorResponse(response, data, "email not found")
        UPRRequest -> User: return response
        deactivate UPRRequest
    else user found
        UPRRequest -> DB: select("password").table("users").where("id =", user_id).exec()
        activate DB
        DB -> Result: return password
        deactivate DB

        alt password invalid
            UPRRequest -> UPRRequest: CreateErrorResponse(response, data, "Invalid password")
            UPRRequest -> User: return response
            deactivate UPRRequest
        else password valid
            UPRRequest -> DB: update("users", {{"password", new_password}}).where("email =", email)
            activate DB
            DB -> Result: return success
            deactivate DB

            alt update failed
                UPRRequest -> UPRRequest: CreateErrorResponse(response, data, "Failed to update password")
                UPRRequest -> User: return response
                deactivate UPRRequest
            else update successful
				UPRRequest -> JsonObj: construct success response
				JsonObj -> User: return response
            end
        end
    end
end
deactivate Locker
@enduml
```

## Sequence diagram : `UpdateUserRequest` processes update user request

```plantuml
@startuml
actor User
participant "UpdateUserRequest" as UURRequest
participant "QJsonObject" as JsonObj
participant "QMutex" as Mutex
participant "QMutexLocker" as Locker
participant "DB::DatabaseManager" as DB
participant "DB::DbResult" as Result

User -> UURRequest: execute(jsonObj, m)
activate UURRequest
UURRequest -> Locker: QMutexLocker(&m)
activate Locker
UURRequest -> JsonObj: extract email, account_number, newData
JsonObj -> UURRequest: return user data

alt DB connection invalid
    UURRequest -> UURRequest: isDBConnectionValid(dbManager)
    UURRequest -> UURRequest: CreateDBConnectionError(response, data)
    UURRequest -> User: return response
    deactivate UURRequest
else DB connection valid
    UURRequest -> DB: select("role").table("users").where("email =", email).exec()
    activate DB
    DB -> Result: return user role
    deactivate DB

    alt user not found
        UURRequest -> UURRequest: CreateErrorResponse(response, data, "you are not registered user!")
        UURRequest -> User: return response
        deactivate UURRequest
    else user found
        UURRequest -> UURRequest: Check if user is admin
        alt not admin
            UURRequest -> UURRequest: CreateErrorResponse(response, data, "Unauthorized, Cannot update user")
            UURRequest -> User: return response
            deactivate UURRequest
        else admin
            UURRequest -> DB: select("*").table("accounts").where("account_number =", account_number).exec()
            activate DB
            DB -> Result: return account data
            deactivate DB

            alt account not found
                UURRequest -> UURRequest: CreateErrorResponse(response, data, "Account number does not exist")
                UURRequest -> User: return response
                deactivate UURRequest
            else account found
                UURRequest -> DB: select("*").table("users").where("email =", new_email).exec()
                activate DB
                DB -> Result: return user data
                deactivate DB

                alt email in use
                    UURRequest -> UURRequest: CreateErrorResponse(response, data, "Email is associated with another account")
                    UURRequest -> User: return response
                    deactivate UURRequest
                else email available
                    UURRequest -> DB: update("users", {new_first_name, new_last_name, new_email, new_role}).where("id =", user_id)
                    activate DB
                    DB -> Result: return success
                    deactivate DB

                    alt update failed
                        UURRequest -> UURRequest: CreateErrorResponse(response, data, "Failed to update user")
                        UURRequest -> User: return response
                        deactivate UURRequest
                    else update successful
						UURRequest -> JsonObj: construct success response
						JsonObj -> User: return response
                    end
                end
            end
        end
    end
end
deactivate Locker
@enduml
```

## Sequence diagram : `UserInitRequest` processes user init request

```plantuml
@startuml
actor User
participant "UserInitRequest" as UIRequest
participant "QJsonObject" as JsonObj
participant "QMutex" as Mutex
participant "QMutexLocker" as Locker
participant "DB::DatabaseManager" as DB
participant "DB::DbResult" as Result

User -> UIRequest: execute(jsonObj, m)
activate UIRequest
UIRequest -> Locker: QMutexLocker(&m)
activate Locker
UIRequest -> JsonObj: extract email, password
JsonObj -> UIRequest: return user data

alt DB connection invalid
    UIRequest -> UIRequest: isDBConnectionValid(dbManager)
    UIRequest -> UIRequest: CreateDBConnectionError(response, data)
    UIRequest -> User: return response
    deactivate UIRequest
else DB connection valid
    UIRequest -> DB: select("*").table("users").where("email =", email).exec()
    activate DB
    DB -> Result: return user data
    deactivate DB

    alt user not found
        UIRequest -> UIRequest: CreateErrorResponse(response, data, "email not found")
        UIRequest -> User: return response
        deactivate UIRequest
    else user found
        UIRequest -> DB: select("password").table("users").where("id =", user_id).exec()
        activate DB
        DB -> Result: return password
        deactivate DB

        alt password invalid
            UIRequest -> UIRequest: CreateErrorResponse(response, data, "Invalid password")
            UIRequest -> User: return response
            deactivate UIRequest
        else password valid
            UIRequest -> data: insert role, first_name, email
            alt role is user
                UIRequest -> DB: select("account_number, balance").table("accounts").where("user_id =", user_id).exec()
                activate DB
                DB -> Result: return account data
                deactivate DB
                UIRequest -> data: insert account_number, current_balance
            end
			UIRequest -> JsonObj: construct success response
			JsonObj -> User: return response
        end
    end
end
deactivate Locker
@enduml
```