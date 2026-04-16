#pragma once
#include "../enums/TransactionEntryType.h"
#include "../ValueObjects/Money.h"

#include <iostream>
#include <chrono>
#include <vector>

struct StatementLine {
	std::chrono::system_clock::time_point CreatedAt;
	std::string Desciption;
	Money Amount;
	TransactionEntryType trEntryType;

};