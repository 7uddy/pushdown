#include "Productie.h"

Productie::Productie(const std::string& stanga, const std::string& dreapta): m_stanga(stanga), m_dreapta(dreapta)
{
}

void Productie::operator=(const Productie& newProd)
{
	m_stanga = newProd.m_stanga;
	m_dreapta = newProd.m_dreapta;
}
