big_integer& big_integer::operator/=(big_integer const& rhs)
{
	if (rhs == 0)
		throw std::runtime_error("division by zero");
	big_integer rhs_tmp = rhs.positive ? rhs : -rhs;
	big_integer tmp = positive ? *this : -*this;
	bool positive_tmp = (rhs.positive == positive); 
	big_integer res = 0;                  
	
	size_t size_rhs = rhs.digits.size();

	int cur_pos = tmp.digits.size() - 1;
	big_integer sub = 0;
	while (cur_pos > -1) {
		//cout << "sub " << to_string_help(sub) << '\n';
		
		sub <<= POW;
		sub += tmp.digits[cur_pos];
		cur_pos--;

		unsigned int res_digit = div(sub, rhs_tmp);
		//cout << "res_digit " << res_digit << '\n';
		res.digits.push_back(res_digit);
		sub -= mul(rhs_tmp, res_digit);
	}
	
	std::reverse(res.digits.begin(), res.digits.end());
	*this = res;
	remove_zeros(*this);
	positive = positive_tmp;
    return *this;
} 