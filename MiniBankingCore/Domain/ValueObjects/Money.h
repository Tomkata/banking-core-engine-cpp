#pragma once
#include <ostream>
#include <sstream>
#include <iomanip>
class Money {
private:
    long long cents;

public:
    explicit Money(long long cents = 0);

    static Money FromDouble(double amount);
    long long GetCents() const;

    Money operator-() const;
    Money& operator+=(const Money& other);
    Money operator+(const Money& other) const;
    Money operator-(const Money& other) const;
    Money operator*(double amount) const;

    bool operator==(const Money& other) const;
    bool operator!=(const Money& other) const;
    bool operator<(const Money& other) const;
    bool operator<=(const Money& other) const;
    bool operator>(const Money& other) const;
    Money Abs() const;

    std::string ToString() const;
};
 
#include <ostream>
inline std::ostream& operator<<(std::ostream& os, const Money& m) {
    return os << m.GetCents() / 100 << "." << m.GetCents() % 100;
}