#pragma once
#include <string>
class Productie
{
public:
	std::string m_stanga;
	std::string m_dreapta;

	Productie(const std::string&, const std::string&);
	Productie() = default;

	void operator=(const Productie& newProd);
};

