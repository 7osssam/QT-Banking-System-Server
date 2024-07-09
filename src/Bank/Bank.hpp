#ifndef BANK_HPP
#define BANK_HPP

#include "BankAccount.hpp"
#include <vector>
#include <algorithm>

class Bank
{
private:
	std::vector<BankAccount> accounts;

public:
	// Default constructor
	Bank();

	// Move constructor
	Bank(Bank&& obj);

	void   addAccount(const BankAccount& account);
	double getTotalBalance() const;
	void   displayAllAccounts() const;

	void SortAccounts();

	// getter for accounts
	std::vector<BankAccount> getAccounts() const;
};

#endif // BANK_HPP