#include "Money.h"
#include <cmath>

Money::Money(long long cents) : cents(cents) {}


Money Money::FromDouble(double amount) {
    return Money(static_cast<long long>(std::round(amount * 100)));
}

long long Money::GetCents() const { 
    return cents;
}

Money Money::operator-() const {
    return Money(-cents);
}

Money& Money::operator+=(const Money& other) {
    cents += other.cents;
    return *this;
}

Money Money::operator+(const Money& other) const
{ 
    return Money(cents + other.cents); 
}
Money Money::operator-(const Money& other) const
{ 
    return Money(cents - other.cents);
}
Money Money::operator*(double other) const {
    return Money(static_cast<long long>(std::round(cents * other)));
}

bool Money::operator==(const Money& other) const{
    return cents == other.cents; 
}
bool Money::operator!=(const Money& other) const {
    return cents != other.cents;
}
bool Money::operator<(const Money& other) const{ 
    return cents < other.cents;
}
bool Money::operator<=(const Money& other) const {
    return cents <= other.cents; 
}
bool Money::operator>(const Money& other) const { 
    return cents > other.cents; 
}

Money Money::Abs() const {
    return Money(cents < 0 ? -cents : cents);
}

std::string Money::ToString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << (cents / 100.0);
    return oss.str();
}