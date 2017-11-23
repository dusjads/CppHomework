#include "big_integer.h"

#include <vector>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <iostream>

using std::vector;
using std::max;
using std::cout;

int const POW = 30;
unsigned long long const BASE = ((unsigned long long)1 << POW);
big_integer const TEN = 10;

big_integer::big_integer()
{
    positive = true;
}

big_integer::big_integer(big_integer const& other)
{
    positive = other.positive;
    digits = other.digits;
}

big_integer::big_integer(int a)
{
    if (a == 0)
        positive = true;
    else
    {
        positive = a > 0;
        digits.push_back((a > 0) ? a : -a);
    }
}

big_integer::big_integer(unsigned int a)
{
    positive = true;
    if (a > 0)
        digits.push_back(a);
}

big_integer::big_integer(std::string const& str)
{
    std::string str_tmp = str;
    big_integer ten_pow = 1;
    positive = true;
    bool positive_tmp = true;
    size_t num_size = str_tmp.size();
    std::reverse(str_tmp.begin(), str_tmp.end());
    if (str_tmp.back() == '-'){
        num_size--;
        positive_tmp = false;
    }
    for (size_t i = 0; i < num_size; i++) {
        *this += big_integer(str_tmp[i] - '0') * ten_pow;
        ten_pow *= TEN;
    }

    positive = positive_tmp;
    if (digits.size() == 0)
        positive = true;
}

big_integer::~big_integer()
{}

void remove_zeros(big_integer& a) {
    while (a.digits.size() > 0 && a.digits.back() == 0)
        a.digits.pop_back();
}

big_integer& big_integer::operator=(big_integer const& other)
{
    positive = other.positive;
    digits = other.digits;
    return *this;
}

big_integer& big_integer::operator+=(big_integer const& rhs)
{
    if (positive != rhs.positive)
    {
        if (positive)
            *this -= -rhs;
        else
        {
            big_integer tmp = rhs - (-*this);
            *this = tmp;
        }
        return *this;
    }
    unsigned int carry = 0;
    for (size_t i = 0; i < max(digits.size(), rhs.digits.size()); i++)
    {
        if (digits.size() < rhs.digits.size())
            digits.push_back(0);

        unsigned long long sum = 0;
        if (i < rhs.digits.size())
        {
            sum += rhs.digits[i];
        }
        sum += (unsigned long long)digits[i] + carry;
        carry = sum >> POW;
        digits[i] = sum % BASE;
    }

    if (carry)
        digits.push_back(carry);
    remove_zeros(*this);
    return *this;
}

bool abs_compare(big_integer const& a, big_integer const& b) // a > b
{
    if (a.digits.size() != b.digits.size())
        return a.digits.size() > b.digits.size();
    for (size_t i = a.digits.size(); i > 0; i--)
    {
        if (a.digits[i-1] != b.digits[i-1])
            return a.digits[i-1] > b.digits[i-1];
    }
    return false;
}

big_integer& big_integer::operator-=(big_integer const& rhs)
{
    if (positive != rhs.positive)
    {
        *this += -rhs;
        return *this;
    }
    if (rhs == *this) {
        *this = 0;
        return *this;
    }
    if (abs_compare(rhs, *this))
    {
        big_integer tmp = rhs - *this;
        *this = tmp;
        positive = !positive;
        return *this;
    }
    int carry = 0;
    for (size_t i = 0; i < digits.size(); i++)
    {
        long long sub = 0;
        if (i < rhs.digits.size())
        {
            sub -= rhs.digits[i];
        }
        sub += (long long)digits[i] + carry;
        carry = 0;
        if (sub < 0)
        {
            carry = -1;
        }
        digits[i] = sub % BASE;

    }
    remove_zeros(*this);
    return *this;
}

big_integer& big_integer::operator*=(big_integer const& rhs)
{
    positive = (positive == rhs.positive);
    unsigned int carry = 0;
    size_t res_size = digits.size() + rhs.digits.size() + 1;
    vector <unsigned int> next(res_size, 0);
    for (size_t i = 0; i < rhs.digits.size(); i++)
    {
        if (digits.size() < rhs.digits.size())
            digits.push_back(0);

        unsigned long long sum = 0;
        for (size_t j = 0; j < digits.size(); j++)
        {
            sum = (unsigned long long)digits[j] * rhs.digits[i] + carry + next[i + j];
            carry = sum >> POW;
            sum %= BASE;
            next[i + j] = sum;
        }
        if (carry)
        {
            sum = next[i + digits.size()] + carry;
            carry = sum >> POW;
            sum %= BASE;
            next[i + digits.size()] = sum;
        }
    }

    while (digits.size() < res_size)
        digits.push_back(0);
    for (size_t i = 0; i < res_size; i++){
        digits[i] = next[i];
    }
    remove_zeros(*this);
    vector<unsigned int>().swap(next);

    return *this;
}


void short_mul(big_integer& res, big_integer const& a, unsigned int digit){
    unsigned int carry = 0;
    res = 0;
    for (size_t i = 0; i < a.digits.size(); i++) {
        unsigned long long sum = (unsigned long long)a.digits[i] * digit + carry;
        carry = sum >> POW;
        sum %= BASE;
        res.digits.push_back(sum);
    }
    if (carry){
        res.digits.push_back(carry);
    }
}

void short_div(big_integer& res, big_integer& a, unsigned int digit){
    unsigned int carry = 0;
    unsigned long long tmp = 0;
    size_t sz = a.digits.size();
    res.digits.resize(sz, 0);
    for (int i = sz - 1; i > -1; i--){
        tmp = (unsigned long long)carry * BASE + a.digits[i];
        res.digits[i] = tmp / digit;
        carry = tmp % digit;
    }
}

void short_mod(big_integer& res, big_integer const& a, unsigned int digit){
    unsigned int carry = 0;
    for (int i = a.digits.size() - 1; i > -1; i--){
        carry = (unsigned long long)(carry * BASE + a.digits[i]) % digit;
    }
    res = carry;
}


unsigned long long trial(big_integer& r, big_integer& d, int k, int m){
    int km = k + m;
    size_t sz_r = r.digits.size();
    for (int i = sz_r; i < km + 1; i++)
        r.digits.push_back(0);
    size_t sz_d = d.digits.size();
    for (int i = sz_d; i < m; i++)
        d.digits.push_back(0);
    __uint128_t r3 = ((__uint128_t)r.digits[km] * BASE + r.digits[km - 1]) * BASE + r.digits[km - 2];
    __uint128_t d2 = (__uint128_t)d.digits[m - 1] * BASE + d.digits[m - 2];
    __uint128_t t = r3 / d2;
    if (t > BASE - 1)
        return BASE - 1;
    return t;
}

bool smaller(big_integer const& r, big_integer const& dq, int k, int m){
    int i = m;
    while (i > 0 && r.digits[i + k] == dq.digits[i])
        i--;
    return r.digits[i + k] < dq.digits[i];
}

void diff(big_integer& r, big_integer const& dq, int k, int m){
    int borrow = 0;
    unsigned long long df = 0;
    for (int i = 0; i <= m; i++){
        df = BASE + r.digits[i + k] - dq.digits[i] - borrow;
        r.digits[i + k] = df % BASE;
        borrow = 1 - (df >> POW);
    }
}

void long_div(big_integer const& x, big_integer const& y, big_integer& q, big_integer& r, int n, int m){
    unsigned int f = BASE / (y.digits[m - 1] + 1);
    short_mul(r, x, f);
    big_integer d = 0;
    short_mul(d, y, f);
    unsigned long long qt = 0;
    big_integer dq = 0;
    q.digits.resize(n - m + 1, 0);
    for (int k = n - m; k > -1; k--){
        qt = trial(r, d, k, m);
        short_mul(dq, d, qt);
        dq.digits.push_back(0);
        if (smaller(r, dq, k, m)){
            qt--;
            short_mul(dq, d, qt);
        }
        q.digits[k] = qt;
        diff(r, dq, k, m);
    }
    short_div(r, r, f);

}

void div(big_integer& x, big_integer const& y, big_integer& q, big_integer& r){
    size_t sz_y = y.digits.size();
    if (sz_y == 0)
        throw std::runtime_error("division by zero");
    if (sz_y == 1){
        unsigned int y1 = y.digits[sz_y - 1];
        short_div(q, x, y1);
        short_mod(r, x, y1);

    }
    else {
        size_t sz_x = x.digits.size();
        if (sz_y > sz_x) {
            q = 0;
            r = x;
        }
        else
            long_div(x, y, q, r, sz_x, sz_y);
    }
}


big_integer& big_integer::operator/=(big_integer const& rhs)
{
    if (rhs == 0)
        throw std::runtime_error("division by zero");
    if (*this == 0)
        return *this;
    big_integer rhs_tmp = rhs.positive ? rhs : -rhs;
    big_integer tmp = positive ? *this : -*this;
    bool positive_tmp = (rhs.positive == positive);
    big_integer q = 0;
    big_integer r = 0;
    div(tmp, rhs_tmp, q, r);
    *this = q;
    remove_zeros(*this);
    positive = positive_tmp;
    return *this;
}

big_integer& big_integer::operator%=(big_integer const& rhs)
{
    if (rhs == 0)
        throw std::runtime_error("division by zero");
    if (*this == 0)
        return *this;
    big_integer rhs_tmp = rhs.positive ? rhs : -rhs;
    big_integer tmp = positive ? *this : -*this;
    big_integer q = 0;
    big_integer r = 0;
    div(*this, rhs, q, r);

    *this = r;
    remove_zeros(*this);
    return *this;
}

void bin(big_integer &a, size_t size) {
    while (a.digits.size() < size + 1)
        a.digits.push_back(0);
    if (!a.positive) {
        for (size_t i = 0; i < a.digits.size(); i++) {
            unsigned int new_dig = 0;
            for (int j = 0; j < POW; j++)
                if ((a.digits[i] & (1 << j)) == 0)
                    new_dig |= (1 << j);
            a.digits[i] = new_dig;
        }
        a.positive = true;
        a += 1;
    }
}

void base(big_integer &a) {
    a.positive = true;
    if (a.digits.back()) {
        a -= 1;
        a.positive = false;
        for (size_t i = 0; i < a.digits.size() - 1; i++) {
            unsigned int new_dig = 0;
            for (int j = 0; j < POW; j++)
                if ((a.digits[i] & (1 << j)) == 0)
                    new_dig |= (1 << j);
            a.digits[i] = new_dig;
        }
        a.digits[a.digits.size() - 1] = 0;
    }
    remove_zeros(a);
}

big_integer& big_integer::operator&=(big_integer const& rhs)
{
    bin(*this, std::max(digits.size(), rhs.digits.size()) + 1);
    big_integer tmp = rhs;
    bin(tmp, std::max(digits.size(), rhs.digits.size()) + 1);
    for (size_t i = 0; i < digits.size(); i++) {
        digits[i] &= tmp.digits[i];
    }
    base(*this);
    return *this;
}

big_integer& big_integer::operator|=(big_integer const& rhs)
{
    bin(*this, std::max(digits.size(), rhs.digits.size()) + 1);
    big_integer tmp = rhs;
    bin(tmp, std::max(digits.size(), rhs.digits.size()) + 1);
    for (size_t i = 0; i < digits.size(); i++) {
        digits[i] |= tmp.digits[i];
    }
    base(*this);
    return *this;
}

big_integer& big_integer::operator^=(big_integer const& rhs)
{
    *this = ((~*this & rhs) | (*this & ~rhs));
    return *this;
}

big_integer& big_integer::operator<<=(int rhs)
{
    int shift = rhs / POW;
    int small_shift = rhs % POW;
    std::reverse(digits.begin(), digits.end());
    for (int i = 0; i < shift; i++)
        digits.push_back(0);
    std::reverse(digits.begin(), digits.end());
    if (small_shift){
        digits.push_back(0);
        for (int i = digits.size() - 1; i > shift; i--) {
            unsigned int digit = digits[i - 1];
            digits[i - 1] <<= small_shift;
            digit >>= POW - small_shift;
            digits[i] |= digit;
        }
    }
    remove_zeros(*this);
    return *this;
}

big_integer& big_integer::operator>>=(int rhs)
{
    bin(*this, digits.size() + 1);
    int shift = rhs / POW;
    int small_shift = rhs % POW;
    if (shift >= (int)digits.size()) {
        *this = 0;
        return *this;
    }
    for (int i = 0; i + shift < (int)digits.size(); i++) {
        digits[i] = digits[i + shift];
    }
    for (int i = 0; i < shift; i++)
        digits.pop_back();
    digits[0] >>= small_shift;
    for (size_t i = 1; i < digits.size(); i++) {
        unsigned int digit = digits[i];
        digits[i] >>= small_shift;
        digit <<= POW - small_shift;
        digits[i-1] |= digit;
    }
    base(*this);
    return *this;
}

big_integer big_integer::operator+() const
{
    return *this;
}

big_integer big_integer::operator-() const
{
    big_integer r = *this;
    r.positive = !r.positive;
    return r;
}

big_integer big_integer::operator~() const
{
    big_integer r = *this;
    r.positive = !r.positive;
    --r;
    return r;
}

big_integer& big_integer::operator++()
{
    *this += big_integer(1);
    return *this;
}

big_integer big_integer::operator++(int)
{
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer& big_integer::operator--()
{
    *this -= big_integer(1);
    return *this;
}

big_integer big_integer::operator--(int)
{
    big_integer r = *this;
    --*this;
    return r;
}

big_integer operator+(big_integer a, big_integer const& b)
{
    return a += b;
}

big_integer operator-(big_integer a, big_integer const& b)
{
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b)
{
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b)
{
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b)
{
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const& b)
{
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b)
{
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b)
{
    return a ^= b;
}

big_integer operator<<(big_integer a, int b)
{
    return a <<= b;
}

big_integer operator>>(big_integer a, int b)
{
    return a >>= b;
}

bool operator==(big_integer const& a, big_integer const& b)
{
    //return !((a < b) || (b < a));
    if (a.digits.size() != b.digits.size())
        return false;
    for (size_t i = a.digits.size(); i > 0; i--)
    {
        if (a.digits[i-1] != b.digits[i-1])
            return false;
    }
    return true;
}

bool operator!=(big_integer const& a, big_integer const& b)
{
    return !(a == b);
}

bool operator<(big_integer const& a, big_integer const& b)
{
    if (a.digits.size() == b.digits.size() && a.digits.size() == 0)
        return false;
    if (a.positive != b.positive)
        return b.positive;
    if (a.positive)
        return abs_compare(b, a);
    return abs_compare(a, b);
}

bool operator>(big_integer const& a, big_integer const& b)
{
    return (b < a);
}

bool operator<=(big_integer const& a, big_integer const& b)
{
    return !(a > b);
}

bool operator>=(big_integer const& a, big_integer const& b)
{
    return !(a < b);
}


std::string to_string_help(big_integer const& a)
{

    std::string res = a.positive ? "" : "-";
    for (size_t i = 0; i < a.digits.size(); i++)
        res += std::to_string(a.digits[i]) + " ";
    return res;
}
std::string to_string(big_integer const& a)
{

    std::string res = "";
    if (a.digits.size() == 0) {
        return "0";
    }
    if (a.digits.size() == 1) {
        return a.positive ? std::to_string(a.digits[0]) : "-" + std::to_string(a.digits[0]);
    }

    big_integer tmp = a.positive ? a : -a;
    while (tmp > 0)
    {
        res += to_string(tmp % TEN);
        tmp /= TEN;
    }
    if (!a.positive)
        res += "-";
    std::reverse(res.begin(), res.end());
    return res;

}


std::ostream& operator<<(std::ostream& s, big_integer const& a)
{
    return s << to_string(a);
}


/*int main(int argc, char const *argv[])
{
    cout << "AAAAA" << '\n';
    big_integer a("99999999999999999999999999999999991111111");
    //big_integer a("1000000000000000000000000");
    big_integer b("9999999999999999999999999999999999");
    //unsigned int b = 2000;
    //big_integer c;
    //c = a / b;
    cout << to_string_help(a) << '\n';
    //cout << to_string_help(a * b) << '\n';
    //cout << to_string_help(a) << '\n';
    //cout << to_string_help(a / b) << '\n';
    big_integer t = (a * b) / b;
    cout << to_string_help(a) << '\n';
    cout << to_string_help(t) << '\n';
    cout << (t == a) << '\n';


    return 0;
}*/