#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H
#include <cstddef>
#include <iosfwd>
#include <vector>

struct big_integer
{
    big_integer();
    big_integer(big_integer const& other);
    big_integer(int a);
    big_integer(unsigned int a);
    explicit big_integer(std::string const& str);
    ~big_integer();

    big_integer& operator=(big_integer const& other);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend bool abs_compare(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);
    friend std::string to_string_help(big_integer const& a);

    friend void bin(big_integer &a, size_t size);
    friend void base(big_integer &a);
    friend void remove_zeros(big_integer& a);
    friend unsigned int div(big_integer const& a, big_integer const& rhs);
    friend bool check(big_integer const& rhs, unsigned int digit, big_integer const& a);
    friend unsigned int find_digit(big_integer const& sub, big_integer const& rhs);
    friend void short_mul(big_integer& res, big_integer const& rhs, unsigned int digit);
    friend void short_mod(big_integer& res, big_integer const& a, unsigned int digit);
    friend void div(big_integer& x, big_integer const& y, big_integer& q, big_integer& r);
    friend void long_div(big_integer const& x, big_integer const& y, big_integer& q, big_integer& r, int n, int m);
    friend bool smaller(big_integer const& r, big_integer const& dq, int k, int m);
    friend void diff(big_integer& r, big_integer const& dq, int km, int m);
    friend unsigned long long trial(big_integer& r, big_integer& d, int m, int k);
    friend void short_div(big_integer& res, big_integer& a, unsigned int digit);





private:
    std::vector <unsigned int> digits;
    bool positive;
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);

#endif // BIG_INTEGER_H
