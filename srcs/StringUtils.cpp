#include "StringUtils.hpp"
#include <cctype>

static int	nbrLen(int n)
{
	int		size = 1;
	long	nb = static_cast<long>(n);

	if (n < 0) {
		nb = -nb;
		size++;
	}
	while (nb >= 10) {
		size++;
		nb /= 10;
	}
	return (size);
}

std::string	StringUtils::itoa(int n)
{
	int		i = nbrLen(n);
	long	nb = static_cast<long>(n);
	std::string	str;
	str.resize(i + 1);

	if (n == 0)
		str[i] = '0';
	if (n < 0) {
		nb = -nb;
		str[0] = '-';
	}
	while (i >= 0 && nb > 0) {
		str[i] = nb % 10 + 48;
		nb /= 10;
		i--;
	}
	return (str);
}

bool	StringUtils::isPositiveNumber(const std::string &str) {
	for (int i = 0; str[i]; i++) {
		if (!std::isdigit(str[i]))
			return false;
	}
	return true;
}
