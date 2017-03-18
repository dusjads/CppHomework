#include "big_integer.h"

#include <cstring>
#include <stdexcept>
#include <algorithm>

const unsigned int BASE = (1 << 30) - 1;

int get_new_size(unsigned int* from, int start_size) {
    while (start_size > 0 && from[start_size - 1] == 0) start_size--;
    return (start_size == 0 ? 1 : start_size);
}

void copy_array(unsigned int* from, unsigned int* &to, int size) {
    for (int i = 0; i < size; ++i) {
        to[i] = from[i];
    }
}

void clear_array(unsigned int* &array, int size) {
    for (int i = 0; i < size; ++i) {
        array[i] = 0;
    }
}

void trim_to_size(unsigned int* &digits, int size) {
	unsigned int* tmp = digits;
	digits = new unsigned int[size];
	for (int i = 0; i < size; ++i) {
		digits[i] = tmp[i];
	}
	delete[] tmp;
}

void set_new_size_with_copy(unsigned int* &array, int size, int new_size) {
    unsigned int *tmp = array;
    array = new unsigned int[new_size];
    clear_array(array, new_size);
    for (int i = 0; i < size; ++i) {
        array[i] = tmp[i];
    }
    delete[] tmp;
}

void negate(unsigned int* &array, int size, int place) {
	for (int i = 0; i < size - 1; ++i) {
		array[i] ^= BASE;
	}
	array[size - 1] += 1 << place;
	place--;
	while (place != -1) {
		bool tmp = ((1 << place) & array[size - 1]);
		array[size - 1] = array[size - 1] - (tmp << place) + (!tmp << place);
		place--;
	}
}

bool fill_array_from_signed_binary(unsigned int* from, unsigned int* &to, int size, int place) {
    bool negative = place == 31 ? 0 : ((1 << place) & from[size - 1]);
    for (int i = 0; i < size - 1; ++i) {
        to[i] = negative ? BASE ^ from[i] : from[i];
    }
    int last_res = from[size - 1];
    while (place != -1 && negative) {
        bool tmp = ((1 << place) & from[size - 1]);
        last_res = last_res - (tmp << place) + (!tmp << place);
        place--; 
    }
    to[size - 1] = last_res;
    return negative;
}

void fill_array_from_unsigned_binary(bool* from, unsigned int* &to, int size) {
    for (int i = 0, place = 0; i < size; ++place) {
        for (int j = 0; j < 30 && i < size; ++j, ++i) {
            to[place] = (from[i] << j) + to[place]; 
        }
    }
}

int get_first_bit(unsigned int* digits, int size) {
    int place = 31;
    while (((1 << place) & digits[size - 1]) == 0) place--;
    return place + 1;
}

big_integer::big_integer() :size(1), digits(new unsigned int[size]), negative(false)
{
    digits[0] = 0;
}

big_integer::big_integer(big_integer const& other) : size(other.size), digits(new unsigned int[size]), negative(other.negative)
{
    copy_array(other.digits, digits, size);
}

big_integer::big_integer(int a_from)
{
    negative = a_from < 0;
    unsigned int a = a_from;
    if (negative) a = ~a + 1;
    //if (a < 0) a = -a;
    unsigned int a1 = (a >> 30);
    unsigned int a2 = (a & BASE);
    size = a1 ? 2 : 1;
    digits = new unsigned int[size];
    if (a1) digits[1] = a1;
    digits[0] = a2;

}

big_integer::big_integer(std::string const& str)
{
    std::string number = str;
    unsigned int start_place = 0;
    if (str[0] == '-') {
        negative = true;
        start_place = 1;
    } else negative = false;

    bool* res = new bool[str.size() * 4];   
    bool only_zeros = false;
    unsigned int res_size = 0;
    //decimal -> binary
    while (!only_zeros) {
        only_zeros = true;
        unsigned int carry = 0;
        for (unsigned int i = start_place; i < str.size(); ++i) {
            int num = number[i] - '0';
            number[i] = char(((carry ? (num + 10) : num) >> 1) + '0');
            if (number[i] != '0') {
                only_zeros = false;
            }
            carry = (carry ? (num + 10) : num) & 1;
        }
        res[res_size++] = carry;
    }
    digits = new unsigned int[(res_size + 29) / 30];
    clear_array(digits, (res_size + 29) / 30);
    unsigned int place = 0;
    //binary -> 2^30
    for (unsigned int i = 0; i < res_size; ++place) {
        for (unsigned int j = 0; j < 30 && i < res_size; ++j, ++i) {
            digits[place] += (res[i] << j);
        }
    }
    size = place;
    delete[] res;
}

big_integer::~big_integer()
{
    delete[] digits;
}

big_integer& big_integer::operator=(big_integer const& other)
{
    if (*this != other) {
        size = other.size;
        negative = other.negative;
        delete[] digits;
        digits = new unsigned int[size];
        for (int i = 0; i < size; ++i)
            digits[i] = other.digits[i];
    }   
    return *this;
}

big_integer& big_integer::operator+=(big_integer const& rhs)
{
    //-a + b
    if (negative && !rhs.negative) {
        big_integer c = rhs;
        c -= -*this;
        *this = c;
        return *this;
    }

    // a + -b ->
    if (!negative && rhs.negative) {        
        *this -= -rhs;
        return *this;
    }

    unsigned int* tmp;
    if (rhs.size > size) {
        tmp = rhs.digits;
    } else {
        tmp = digits;
    }

    int size_min = std::min(rhs.size, size);
    int size_max = std::max(rhs.size, size);
    unsigned int *new_digits = new unsigned int[size_max + 1];
    int new_size = size_max;

    unsigned int c = 0;
    for (int i = 0; i < size_min; ++i) {
        unsigned int sum = rhs.digits[i] + digits[i] + c;
        c = sum >> 30;
        sum &= BASE;
        new_digits[i] = sum;
    }
    for (int i = size_min; i < size_max; ++i) {
        unsigned int sum = tmp[i] + c;
        c = sum >> 30;
        sum &= BASE;
        new_digits[i] = sum;
    }
    if (c != 0) {
        ++new_size;
        new_digits[size_max] = c;
    }
    std::swap(new_digits, digits);
    delete[] new_digits;
    size = new_size;
    return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs)
{

    if (negative && !rhs.negative) {
        big_integer c = rhs; 
        c += -*this; //
        *this = -c;
        return *this;
    }

    //-a -= -b => -a + b;
    if (negative && rhs.negative) {
        big_integer c = -rhs;
        c -= -*this;
        *this = c;
        return *this;
    }

    //a -= -b => a += b
    if (!negative && rhs.negative) {
        big_integer c = -rhs;
        *this += c;
        return *this;
    }

    // a - b, but b > a
    if (*this < rhs) {
        big_integer c = rhs;
        c -= *this;
        *this = -c;
        return *this;
    }

    unsigned int c = 0;
    for (int i = 0; i < size; ++i) {
        unsigned int tmp = (i < rhs.size) ? rhs.digits[i] : 0;
        if (digits[i] < tmp + c) {
            digits[i] = ((unsigned int)1 << 30) + digits[i] - tmp - c;
            c = 1;
        } else {
            digits[i] = digits[i] - tmp - c;
            c = 0;
        }
    }
    size = get_new_size(digits, size);
    trim_to_size(digits, size);

    return *this;
}

big_integer& big_integer::operator*=(big_integer const& rhs)
{
    unsigned int *new_digits = new unsigned int[rhs.size + size + 1];
    clear_array(new_digits, rhs.size + size + 1);
    unsigned long long carry, add;
    for (int i = 0; i < rhs.size; ++i) {
        for (int j = 0; j < size; ++j) {
            carry = (unsigned long long)digits[j] * rhs.digits[i];
            add = carry & BASE;
            carry >>= 30;
            int k = 0;
            while (carry + add) {
            	carry += (new_digits[i + j + k] + add) >> 30;
            	new_digits[i + j + k] = (new_digits[i + j + k] + add) & BASE;
            	add = carry;
            	carry = 0;
            	++k;
            }
        }
    }
    std::swap(digits, new_digits);
    size = get_new_size(digits, rhs.size + size + 1);
    trim_to_size(digits, size);
    negative = negative ^ rhs.negative;
    delete[] new_digits;
    return *this;
}

void mul_with_left(big_integer &res, big_integer &rhs, unsigned long long mul, int left) {
    clear_array(res.digits, res.size);
    unsigned long long next, now;
    for (int j = 0; j < rhs.size; ++j) {
        next = mul * rhs.digits[j];
        now = next & BASE;
        next >>= 30;
        next += (res.digits[left + j] + now) >> 30;

        res.digits[left + j] = (res.digits[left + j] + now) & BASE;
		res.digits[left + j + 1] = (res.digits[left + j + 1] + next) & BASE;
    }
    res.negative = false;
    res.size = get_new_size(res.digits, rhs.size + left + 2);	
}

void div_short(big_integer &res, unsigned long long div) {
	unsigned long long carry = 0;
	unsigned long long temp;
	for (int i = res.size - 1; i >= 0; --i) {
		temp = (carry << 30) + res.digits[i];
		res.digits[i] = (unsigned int)(temp / div);
		carry = temp - div * res.digits[i];
	}
}

big_integer& big_integer::operator/=(big_integer const& rhs)
{
	if (rhs.size == 1) {
		div_short(*this, rhs.digits[0]);
		negative ^= rhs.negative;
		size = get_new_size(digits, size);
		return *this;
	}

    big_integer a = negative ? -*this : *this;
    big_integer b = rhs.negative ? -rhs : rhs;
    big_integer res;
    delete[] res.digits;
    res.size = rhs.size + size + 1;
    res.digits = new unsigned int[res.size];
    res.negative = false;

    unsigned int l, r, med;
    for (int i = size - 1; i >= 0; --i) {
    	r = 1 << 30; l = 0;

    	if (b.size + i > a.size ||
    		(b.size + i == a.size && b.digits[b.size - 1] > a.digits[a.size - 1]
    			)) r = 0;
    		
    	while (r > 1 + l) {
    		med = (r + l) >> 1;
    		mul_with_left(res, b, med, i);
    		//res = med * b
    		//res <<= (i * 30)
    		if (res > a) {
    			r = med;
    		} else {
    			l = med;
    		}
    	}
    	digits[i] = l;
		mul_with_left(res, b, l, i);
		a -= res;

    }
    size = get_new_size(digits, size);
    trim_to_size(digits, size);
    negative ^= rhs.negative;
	return *this;    
}

big_integer& big_integer::operator%=(big_integer const& rhs)
{
    big_integer a = negative ? -*this : *this;
    big_integer b = a / rhs;
    b *= rhs;
    a -= b;

    std::swap(digits, a.digits);
    size = a.size;

    return *this;
}

big_integer& big_integer::operator&=(big_integer const& rhs)
{
	if (*this == 0 || rhs == 0) {
        *this = 0;
        return *this;
    }
    binary_operation(*this, rhs, 0);
    return *this;
}

big_integer& big_integer::operator|=(big_integer const& rhs)
{
	if (rhs == 0) {
		return *this;
	} else if (*this == 0) {
		*this = rhs;
		return *this;
	}
	binary_operation (*this, rhs, 1);
    return *this;
}

big_integer& big_integer::operator^=(big_integer const& rhs)
{
    if (rhs == 0) {
		return *this;
	} else if (*this == 0) {
		*this = rhs;
		return *this;
	}
	binary_operation(*this, rhs, 2);
    return *this;
}

void binary_operation(big_integer &res, big_integer const& rhs, int operation) {
    int size_max = std::max(res.size, rhs.size);
    int size_min = std::min(res.size, rhs.size);
    big_integer a = res, b = rhs;
    int neg_bit;

    int a_max = get_first_bit(a.digits, a.size), b_max = get_first_bit(b.digits, b.size);

    if (a.size < b.size || (a.size == b.size && a_max < b_max)) {
        std::swap(a, b);
        std::swap(a_max, b_max);
    } 

    if ((a.negative || b.negative) && a_max == 31) {
        neg_bit = 0;
        ++size_max;
    } else {
        neg_bit = a_max;
    }

    set_new_size_with_copy(a.digits, res.size, size_max);
    set_new_size_with_copy(b.digits, rhs.size, size_max);

    unsigned int* new_digits = new unsigned int[size_max];

    if (a.negative) {
        a.negative = false;
        negate(a.digits, a.size, neg_bit);
        a += 1;
    } 
    if (b.negative) {
        b.negative = false;
        negate(b.digits, b.size, neg_bit);        
        b += 1;
    }

    for (int i = 0; i < size_max; ++i) {
    	switch (operation) {
        	case 0: new_digits[i] = (i < size_min) ? a.digits[i] & b.digits[i] : 0; break;
        	case 1: new_digits[i] = a.digits[i] | b.digits[i]; break;
        	case 2: new_digits[i] = a.digits[i] ^ b.digits[i];
    	}
    }

    res.negative = fill_array_from_signed_binary(new_digits, res.digits, size_max, neg_bit);
    res.size = get_new_size(res.digits, size_max);
    trim_to_size(res.digits, res.size);

    delete[] new_digits;
    if (res.negative) {
    	--res;
    }
}

big_integer& big_integer::operator<<=(int rhs)
{
    if (rhs == 0)
        return *this;

    int out_array = rhs / 30;
    int in_array = rhs % 30;
    int new_size = out_array + size + (in_array ? 1 : 0);

    unsigned int *new_digits = new unsigned int[new_size];

    clear_array(new_digits, new_size);
    unsigned int now = 0, carry = 0;

    for (int i = 0; i < size; ++i) {
        now = digits[i] << in_array;
        carry = digits[i] >> (30 - in_array);
        now &= BASE;

        new_digits[i + out_array] += now;
        new_digits[i + out_array + 1] += carry;
    }
    size = get_new_size(new_digits, new_size);
    std::swap(new_digits, digits);
    delete[] new_digits;
    if (negative)
    	--*this;
    return *this;
}

big_integer& big_integer::operator>>=(int rhs)
{
    if (rhs == 0)
        return *this;

    int out_array = rhs / 30;
    int in_array = rhs % 30;
    int new_size = size;

    unsigned int *new_digits = new unsigned int[size];

    clear_array(new_digits, size);
    for (int i = size - 1; i >= out_array; i--) {
        unsigned int prev = digits[i] << (32 - in_array);
        unsigned int now = (digits[i] >> in_array); 
        prev >>= 2;
        new_digits[i - out_array] += now;
        if (i - out_array - 1 >= 0)
            new_digits[i - out_array - 1] += prev;
    }
    size = get_new_size(new_digits, new_size);
    std::swap(new_digits, digits);
    trim_to_size(digits, size);

    delete[] new_digits;
    if (negative)
    	--*this;
    return *this;
}

big_integer big_integer::operator+() const
{
    big_integer r = big_integer(*this);
    return r;
}

big_integer big_integer::operator-() const
{
    big_integer r = big_integer(*this);
    r.negative ^= 1;
    return r;
}

big_integer big_integer::operator~() const
{
    big_integer r = big_integer(*this);
    r.negative = !r.negative;
    --r;
    return r;
}

big_integer& big_integer::operator++()
{
    if (negative) {
    	negative = false;
    	--*this;
    	negative = true;
    } else if (size == 1 && digits[0] == 0) {
    	digits[0] = 1;
    	negative = false;
    } else {
    	unsigned int c = 1;
    	for (int i = 0; i < size && c; ++i) {
        	digits[i] = digits[i] + 1;
        	c = digits[i] >> 30;
        	digits[i] &= BASE;
    	}
    	if (c) {
    		delete[] digits;
    		digits = new unsigned int[++size];
    		clear_array(digits, size);
    		digits[size - 1] = 1;
    	}
    }
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
    if (negative) {
    	negative = false;
    	++*this;
    	negative = true;
    } else if (size == 1 && digits[0] == 0) {
    	digits[0] = 1;
		negative = true;
    } else {
    	unsigned int c = 1;
		unsigned int add = 1 << 30;
	    for (int i = 0; i < size && c; ++i) {
	        if (digits[i] < c) {
	            digits[i] = add + digits[i] - c;
	            c = 1;
	        } else {
	            digits[i] = digits[i] - c;
	            c = 0;
	        }
	    }
	    size = get_new_size(digits, size);
    	trim_to_size(digits, size);
    }
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
    return !((a < b) || (b < a));
}

bool operator!=(big_integer const& a, big_integer const& b)
{
    return !(a == b);
}

bool operator<(big_integer const& a, big_integer const& b)
{
    if (a.size == 1 && a.digits[0] == 0 && b.size == 1 && b.digits[0] == 0) return 0; //0 !< 0
    if (a.negative && !b.negative) return 1; // -a < b
    if (b.negative && !a.negative) return 0; // a !< -b
    if (a.size < b.size) return 1;
    if (a.size > b.size) return 0;
    for (int i = a.size - 1; i >= 0; i--) {
        if (a.digits[i] < b.digits[i]) return 1;
        if (a.digits[i] > b.digits[i]) return 0;
    }
    return 0;
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

std::string to_string(big_integer const& a)
{
    if (a.digits[0] == 0 && a.size == 1) {
        return "0";
    }
    std::string tmp = "";
    big_integer b = a.negative ? -a : a;
    big_integer ten = big_integer(10);
    while (b != 0) {
        big_integer c = (b % ten);
        tmp += std::to_string(c.digits[0]);
        b /= 10;
    }
    std::reverse(tmp.begin(), tmp.end());
    return (a.negative ? "-": "") + tmp;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a)
{
    return s << to_string(a);
}