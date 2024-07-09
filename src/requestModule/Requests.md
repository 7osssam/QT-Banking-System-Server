# 1. Login Request

## Send Request:

```json

{

    "Request": 1,

    "Data":

        {

        "email": "user@example.com",

        "password": "pass"

        }

}

```

## Receive Response:

### On Success:

```json

{

    "Response": 1,

    "Data": {

        "status": 1,

        "first_name": "User",

        "role": "user"

        "message": "Login successful"

    }

}

```

### On Failure:

```json

{

    "Response": 1,

    "Data": {

        "status": 0

        "message": "email not found"

    }

}

```

#### Or

```json

{

    "Response": 1,

    "Data": {

        "status": 0

        "message": "Invalid password"

    }

}

```

# 2. Get Account Number (for Logged-in user)

## Send Request:

```json

{

    "Request": 2,

    "Data": {

        "email": "user@mail.com"

    }

}

```

## Receive Response:

### On Success:

```json

{

    "Response": 2,

    "Data": {
		 "status": 1,
        "account_number": 123456789

    }

}

```

### On Failure:

```json

{

    "Response": 2,

    "Data": {
		 "status": 0,
        "message": "No account found"
        }

}

```

or
```json

{

    "Response": 2,

    "Data": {
		 "status": 0,
        "message": "email not found"
        }

}


# 3. Get Balance

## Send Request:

```json

{

    "Request": 3,

    "Data": {

        "account_number": 123456789

    }

}

```

## Receive Response:

### On Success:

```json

{

    "Response": 3,

    "Data": {
	    "status": 1,
        "balance": 500.0
       
    }

}

```

### On Failure: (will not Usually happen)

```json

{

    "Response": 3,

    "Data": {
		 "status": 0,
        "message": "No account found"

    }

}

```

# 4. Get Transactions History

## Send Request:

```json

{

    "Request": 4,

    "Data": {

        "account_number": 123456789

    }

}

```

## Receive Response:

```json

{

    "Request": 4,

    "Data": {
		 "status": 1,
        "account_number": 123456789,

        "List":

            [

                {

                    "to_account_number": 987654321,

                    "transaction_amount": 100.0,

					"transaction_type": "+", // "+" for deposit

                    "created_at": "2021-01-01"

                },

                {

                    "to_account_number": 987612321,

                    "transaction_amount": 130.0,

					"transaction_type": "Withdrawal", // "-" for withdrawal

                    "created_at": "2021-01-03"

                }

            ]

        }

    }

}

```

# 5. Make Transaction

## Send Request:

```json

{

    "Request": 5,

    "Data": {

        "from_account_number": 123456789,

        "to_account_number": 1222255555,

        "transaction_amount": 100.0

    }

}

```

## Receive Response:

### On Success:

```json

{

    "Response": 5,

    "Data": {

        "status": 1,

        "message": "Transaction successful"

    }

}

```

### On Failure:

```json

{

    "Response": 5,

    "Data": {

        "status": 0,

        "message": "Insufficient balance"

    }

}

```

# 6. Transfer Amount

## Send Request:

```json

{

    "Request": 6,

    "Data": {

        "from_account_number": 123456789,

        "to_account_number": 987654321,

        "transferAmount": 50.0

    }

}

```

## Receive Response:

### On Success:

```json

{

    "Response": 6,

    "Data": {

        "status": 1,

        "message": "Transfer successful"

    }

}

```

### On Failure:

```json

{

    "Response": 6,

    "Data": {

        "status": 0,

        "message": "Insufficient balance"

    }

}

```

or

```json

{

    "Response": 6,

    "Data": {

        "status": 0,

        "message": "Account not found"

    }

}

```

# 7. Get Database (for admin)

## Send Request:

```json

{

    "Request": 7,

    "Data": {

        "email": "admin@mail.com",

        "password": "adminpass"

    }

}

```

## Receive Response:

```json

{

    "Response": 7,

    "Data": {
		"status": 1,
        "users": [

            {

                "account_number": 123456789, // NULL if admin

                "first_name": "Admin",

                "last_name": "User",

                "email": "email@mail.com",

                "role": "user",

                "balance": 500.0

            },

            {

                "account_number": NULL, // NULL if admin

                "first_name": "User",

                "last_name": "User",

                "email": "admin@mail.com",

                "role": "admin",

                "balance": 500.0

            }

        ]

    }

}

```

# 8. Create New User (for admin)

## Send Request:

```json

{

    "Request": 8,

    "Data": {

        "email": "admin@mail.com",

        "newUser": [

            {

                "first_name": "New",

                "last_name": "User",

                "email": "newuser@example.com",

                "password": "newpass",

                "role": "user",

                "initial_balance": 500.0

            }

        ],

    }

}

```

## Receive Response:

### On Success:

```json

{

    "Response": 8,

    "Data": {

        "status": 1,

        "message": "New user created successfully"

    }

}

```

### On Failure:

```json

{

    "Response": 8,

    "Data": {

        "status": 0,

        "message": "User already exists"

    }

}

```

or

```json

{

    "Response": 8,

    "Data": {

        "status": 0,

        "message": "Cannot create new user. User is not an admin"

    }

}

```





# 9. Delete User (for admin)

## Send Request:

```json

{

    "Request": 9,

    "Data": {

        "email": "admin@mail.com",

        "password": "adminpass",

        "account_number": 123456789

    }

}

```

## Receive Response:

### On Success:

```json

{

    "Response": 9,

    "Data": {

        "status": 1,

        "message": "User deleted successfully"

    }

}

```

### On Failure:

```json

{

    "Response": 9,

    "Data": {

        "status": 0,

        "message": "User not found"

    }

}

```

# 10. Update User (for admin)

## Send Request:

```json

{

    "Request": 10,

    "Data": {

        "email": "admin@mail.com",

        "password": "adminpass",

        "account_number": 123456789,

        "newData": {

            "first_name": "Updated",

            "last_name": "User",

            "email": "updateduser@example.com",

            "role": "admin"

        }

    }

}

```

## Receive Response:

### On Success:

```json

{

    "Response": 10,

    "Data": {

        "status": 1,

        "message": "User updated successfully"

    }

}

```

### On Failure:

```json

{

    "Response": 10,

    "Data": {

        "status": 0,

        "message": "User not found"

    }

}

```
