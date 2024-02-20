#include "Grammar.h"

Grammar::Grammar(const std::vector<char>& Vn, const std::vector<char>& Vt, const char& caracterStart,
	const std::vector<Productie>& productie) : 
	m_neterminale{ Vn }, m_terminale{ Vt }, m_startCaracter{ caracterStart }, m_productie{productie}
{
}

void Grammar::PrintGrammar(std::ostream& os) const noexcept
{
	os << "GRAMATICA:\n";
	os << *this;
}

void Grammar::ReadGrammarFromFile(const std::string& nume_fisier)
{
	std::ifstream fin{ nume_fisier };
	fin >> *this;
	fin.close();
}

void Grammar::InitializeVnAndVt(const std::string& sir_caractere, const bool& inVn)
{
	if (inVn)
		for (int index = 0; index < sir_caractere.size(); index += 2)
		{
			m_neterminale.emplace_back(sir_caractere[index]);
		}
	else
		for (int index = 0; index < sir_caractere.size(); index += 2)
		{
			m_terminale.emplace_back(sir_caractere[index]);
		}
}

void Grammar::SetStartCharacter(const char& caracterInceput)
{
	m_startCaracter=caracterInceput;
}

std::string Grammar::GenerateWord() const
{
	if (!this->VerifyGrammar())
		throw std::exception("ERROR: Not able to generate word. Grammar not valid.");
	std::string sir_modificat;
	sir_modificat.push_back(m_startCaracter);
	std::vector<uint16_t> indiceProductiePosibila;
	do
	{
		indiceProductiePosibila.clear();
		for (uint16_t index=0; index<m_productie.size(); index++)
		{
			if (sir_modificat.find(m_productie[index].m_stanga) != std::string::npos)
				indiceProductiePosibila.emplace_back(index);
		}
		if (indiceProductiePosibila.empty())
			break;
		uint16_t indiceRandom = GetIndiceRandom(indiceProductiePosibila.size()-1);
		ApplyProduction(indiceProductiePosibila[indiceRandom], sir_modificat);
	} while (!indiceProductiePosibila.empty());
	if (sir_modificat.length() == 0)
		return std::string{ k_lambda };
	return sir_modificat;
}

bool Grammar::CanGenerateLambda() const noexcept
{
	auto foundProduction = std::find_if(m_productie.begin(), m_productie.end(), [this](const Productie& productie) {
		if (productie.m_stanga[0] == m_startCaracter)
			if (productie.m_dreapta[0] == k_lambda)
				return true;
		return false;
		});
	if (foundProduction != m_productie.end())
		return true;
	return false;
}

void Grammar::SortProductions()
{
	std::sort(m_productie.begin(), m_productie.end(), [](Productie& prod1, Productie& prod2) {
		return prod1.m_stanga < prod2.m_stanga; });
}

PushDownAutomaton Grammar::GrammarToAutomaton() const noexcept
{
	PushDownAutomaton a;

	std::vector<char> states = { 'q' };

	a.SetInitialState('q');
	a.SetAlphabet(m_terminale);
	a.SetStackAlphabet(m_neterminale);
	a.SetStates(states);
	a.SetFirstStackChar(m_startCaracter);

	for (const auto& production : m_productie)
	{
		states.clear();
		for (const auto& state : production.m_dreapta)
		{
			if (state >= 'A' && state <= 'Z')
			{
				states.emplace_back(state);
			}
		}
		if (states.empty())
		{
			states.emplace_back(PushDownAutomaton::kLambda);
		}
		a.AddTransition({'q',production.m_dreapta[0],production.m_stanga[0] }, {'q',states});

	}
	std::cout << a << std::endl;
	return a;
}

void Grammar::ConvertIDCToFNC()
{
	RemoveRenames();
	ReplaceProductionMDWithOnlyNeterminals();
	ReplaceProductionWhereMDHasSizeBiggerThan2();
}

void Grammar::RemoveRenames()
{
	auto isInNeterminale = [this](const char& caracter) {
		return std::ranges::find(m_neterminale, caracter) != m_neterminale.end();
		};


	std::queue<Productie> productionsThatNeedRename;
	std::vector<Productie> newProductions;
	for (const auto& prod : m_productie)
	{
		if (prod.m_stanga.size() == 1 && prod.m_dreapta.size() == 1
			&& isInNeterminale(prod.m_stanga[0]) && isInNeterminale(prod.m_dreapta[0]))
		{
			productionsThatNeedRename.push(prod);
		}
		else
		{
			newProductions.push_back(prod);
		}
	}
	Productie current;
	while (!productionsThatNeedRename.empty())
	{
		current = productionsThatNeedRename.front();
		productionsThatNeedRename.pop();
		for (const auto& prod : m_productie)
		{
			if (prod.m_stanga.size() == 1 && prod.m_stanga[0] == current.m_dreapta[0])
			{
				if (prod.m_dreapta.size() == 1 && isInNeterminale(prod.m_dreapta[0]))
				{
					Productie new_prod{ current.m_stanga, prod.m_dreapta };
					productionsThatNeedRename.push(new_prod);
				}
				else
				{
					Productie new_prod{ current.m_stanga, prod.m_dreapta };
					newProductions.push_back(new_prod);
				}
			}
		}
	}
	m_productie = newProductions;
}

void Grammar::ReplaceProductionMDWithOnlyNeterminals()
{
	auto isIn = [this](const std::vector<char>& here, const char& caracter) {
		return std::ranges::find(here, caracter) != here.end();
		};

	for (int index = 0; index < m_productie.size(); index++)
	{
		if (m_productie[index].m_dreapta.size() < 2)
			continue;
		for (const auto& caracter : m_productie[index].m_dreapta)
		{
			if (isIn(m_terminale, caracter))
			{
				ReplaceTerminalWithNeterminal(caracter);
				//Verify all productions again
				index = -1;
				break;
			}
		}

	}
}

void Grammar::ReplaceProductionWhereMDHasSizeBiggerThan2()
{
	auto isIn = [this](const std::vector<char>& here, const char& caracter) {
		return std::ranges::find(here, caracter) != here.end();
		};
	for (int index=0; index<m_productie.size();index++)
	{
		if (m_productie[index].m_dreapta.size() <= 2)
			continue;
		ReplaceTwoNeterminalsWithNewNeterminal(m_productie[index]);
		index = -1;
	}
}

void Grammar::ConvertFNCToFNG()
{
	auto isIn = [this](const std::vector<char>& here, const char& caracter) {
		return std::ranges::find(here, caracter) != here.end();
		};

	auto getPositionInOf = [this](const std::vector<char>& here, const char& caracter) {
		for (int index = 0; index < here.size(); index++)
			if (here[index] == caracter) return index;
		return -1;
		};

	//PASUL 1
	bool modified = true;
	while (modified)
	{
		modified = false;
		for (const auto& prod : m_productie)
		{
			if (getPositionInOf(m_neterminale, prod.m_stanga[0]) >= getPositionInOf(m_neterminale, prod.m_dreapta[0])
				&& isIn(m_neterminale, prod.m_stanga[0]) && isIn(m_neterminale, prod.m_dreapta[0]))
			{
				if (prod.m_stanga[0] == prod.m_dreapta[0])
				{
					Lema2(prod.m_stanga[0]);
				}
				else
				{
					Lema1(prod, 0);
				}
				modified = true;
				break;
			}
		}
	}
	//PASUL 2
	for (int index = m_neterminale.size() - 2; index >= 0; index--)
	{
		modified = true;
		while (modified)
		{
			modified = false;
			for (auto it = m_productie.begin(); it != m_productie.end(); ++it)
			{
				if (it->m_stanga[0] != m_neterminale[index])
					continue;
				if (isIn(m_neterminale, it->m_dreapta[0]))
				{
					if (getPositionInOf(m_neterminale, it->m_dreapta[0]) > getPositionInOf(m_neterminale, it->m_stanga[0]))
					{
						Lema1(*it, 0);
						modified = true;
						break;
					}
				}
			}
		}
	}
	//PASUL 3
	for (const auto& neterminal : m_neterminaleAddedInFNGConversion)
	{
		bool modified = true;
		while (modified)
		{
			modified = false;
			for (const auto& productie : m_productie)
			{
				if (productie.m_stanga[0] == neterminal && isIn(m_neterminale, productie.m_dreapta[0]))
				{
					Lema1(productie, 0);
					modified = true;
					break;
				}
			}
		}
	}
	for (const auto& element : m_neterminaleAddedInFNGConversion)
		m_neterminale.emplace_back(element);
	m_neterminaleAddedInFNGConversion.clear();
}

void Grammar::ReplaceTerminalWithNeterminal(char caracterToBeReplaced)
{
	char newNeterminal{ GetNewNeterminal() };
	//Add production newNeterminal -> caracterToBeReplaced
	m_neterminale.emplace_back(newNeterminal);
	m_productie.emplace_back(std::string{ newNeterminal }, std::string{ caracterToBeReplaced });
	//Replace in all productions where md>=2
	for (int index = 0; index < m_productie.size(); index++)
	{
		if (m_productie[index].m_dreapta.size() < 2)
			continue;
		for (auto& caracter : m_productie[index].m_dreapta)
		{
			if (caracter == caracterToBeReplaced)
			{
				caracter = newNeterminal;
			}
		}
	}
}

void Grammar::ReplaceTwoNeterminalsWithNewNeterminal(Productie& productionToBeReplaced)
{
	char newNeterminal{ GetNewNeterminal() };

	//Replace in current production
	std::string whatIsReplaced{ productionToBeReplaced.m_dreapta.substr(productionToBeReplaced.m_dreapta.size()-2, 2)};
	productionToBeReplaced.m_dreapta.pop_back();
	productionToBeReplaced.m_dreapta.pop_back();
	productionToBeReplaced.m_dreapta.push_back(newNeterminal);

	m_neterminale.emplace_back(newNeterminal);
	m_productie.emplace_back(std::string{ newNeterminal }, whatIsReplaced);
	
	//Replace in all productions where md>2
	for (auto& productie : m_productie)
	{
		if (productie.m_dreapta.size() <= 2)
			continue;
		if (auto whereToReplace = productie.m_dreapta.find(whatIsReplaced); whereToReplace!=std::string::npos)
			productie.m_dreapta.replace(whereToReplace, whatIsReplaced.length(), std::string{ newNeterminal });
	}

}

char Grammar::GetNewNeterminal()
{
	auto isIn = [this](const std::vector<char>& here, const char& caracter) {
		return std::ranges::find(here, caracter) != here.end();
		};

	char newNeterminal = m_neterminale[1];
	while (isIn(m_neterminale, newNeterminal) || isIn(m_terminale, newNeterminal))
		newNeterminal++;
	return newNeterminal;
}


void Grammar::Lema1(Productie production, int replaceIndex)
{
	char characterThatNeedsToBeReplaced = production.m_dreapta[replaceIndex];
	//contains the productions that need to be replaced in given production
	std::vector<Productie> productionsThatStartWithReplaceChar;
	//contains the new version of productions
	std::vector<Productie> newProductions;

	//Get all productions beside the parameter production
	for (const auto& prod : m_productie)
	{
		if (prod.m_stanga == production.m_stanga && prod.m_dreapta == production.m_dreapta)
			continue;
		newProductions.push_back(prod);
	}
	//Find all productions that need to be replaced in parameter production
	for (const auto& prod : newProductions)
	{
		if (prod.m_stanga[0] == characterThatNeedsToBeReplaced)
		{
			productionsThatStartWithReplaceChar.push_back(prod);
		}
	}
	//Add production with replacement
	for (const auto& prod : productionsThatStartWithReplaceChar)
	{
		Productie new_production = production;
		new_production.m_dreapta.replace(replaceIndex, 1, prod.m_dreapta);
		newProductions.push_back(new_production);
	}
	m_productie = newProductions;
}

void Grammar::Lema2(char neterminal)
{
	auto isIn = [this](const std::vector<char>& here, const char& caracter) {
		return std::ranges::find(here, caracter) != here.end();
		};

	//contin productiile de forma neterminal -> neterminal + ...
	std::vector<Productie> complexProductions;
	//contin productiile de forma neterminal -> terminal
	std::vector<Productie> simpleProductions;
	//contin toate productiile noi
	std::vector<Productie> newProductions;

	char newNeterminal = 'A';
	while (isIn(m_neterminale, newNeterminal) || isIn(m_neterminaleAddedInFNGConversion, newNeterminal))
	{
		newNeterminal++;
	}
	m_neterminaleAddedInFNGConversion.emplace_back(newNeterminal);

	for (const auto& prod : m_productie)
	{
		if (prod.m_stanga[0] == neterminal)
		{
			if (prod.m_dreapta[0] == neterminal)
			{
				complexProductions.push_back(prod);
				continue;
			}
			if(prod.m_dreapta.size() == 1 && isIn(m_terminale, prod.m_dreapta[0]))
			{
				simpleProductions.push_back(prod);
				continue;
			}
			//case for new Z
			newProductions.push_back(prod);
		}
		else
		{
			newProductions.push_back(prod);
		}
	}
	//add productions: Z->ab and Z->abZ
	for (auto& prod : complexProductions)
	{
		prod.m_stanga[0] = newNeterminal;
		prod.m_dreapta.erase(0, 1);
		newProductions.push_back(prod);
		prod.m_dreapta += newNeterminal;
		newProductions.push_back(prod);
	}
	//add productions: A-> a and A->aZ
	for (auto& prod : simpleProductions)
	{
		newProductions.push_back(prod);
		prod.m_dreapta += newNeterminal;
		newProductions.push_back(prod);
	}
	m_productie = newProductions;
}


bool Grammar::VerifyGrammar() const
{
	if (VnIsPartOfVt()) return false;
	if (!StartCaracterIsInVn()) return false;
	if (!EveryProductionHasOneVn()) return false;
	if (!ExistsOneProductionWithStartCaracter()) return false;
	if (!ProductionsContainOnlyVnAndVt()) return false;
	return true;
}

bool Grammar::IsIDC() const
{
	auto isInNeterminale = [this](const char& caracter) {
		return std::ranges::find(m_neterminale, caracter) != m_neterminale.end();
		};
	//Check if ms.size==1 
	//and ms.value==neterminal => already verified in VerifyGrammar
	for (const auto& prod : m_productie)
	{
		if (prod.m_stanga.size() != 1)
			return false;
		/*if (!isInNeterminale(prod.m_stanga[0]))
			return false;*/
	}
	return true;
}

bool Grammar::VnIsPartOfVt() const
{
	for (auto const& caracter : m_neterminale)
	{
		if (std::find(m_terminale.begin(), m_terminale.end(), caracter) != m_terminale.end())
			return true;
	}
	return false;
}

bool Grammar::StartCaracterIsInVn() const
{
	return std::find(m_neterminale.begin(), m_neterminale.end(), m_startCaracter) != m_neterminale.end();

}

bool Grammar::EveryProductionHasOneVn() const
{
	for (const auto& productie : m_productie)
	{
		bool hasAtLeastOneCaracter = false;
		for (const auto& caracter : m_neterminale)
		{
			if (std::find(productie.m_stanga.begin(), productie.m_stanga.end(), caracter) != productie.m_stanga.end())
			{
				hasAtLeastOneCaracter = true;
				break;
			}
		}
		if (!hasAtLeastOneCaracter)
			return false;
	}
	return true;
}

bool Grammar::ExistsOneProductionWithStartCaracter() const
{
	for (auto const& productie : m_productie)
	{
		if (productie.m_stanga.size() != 1)
			continue;
		if (std::find(productie.m_stanga.begin(), productie.m_stanga.end(), m_startCaracter) != productie.m_stanga.end())
			return true;
	}
	return false;
}

bool Grammar::ProductionsContainOnlyVnAndVt() const
{
	std::unordered_set<char> caractere;
	caractere.emplace(k_lambda);
	for (auto const& caracter : m_neterminale)
		caractere.emplace(caracter);
	for (auto const& caracter : m_terminale)
		caractere.emplace(caracter);
	for (auto const& productie : m_productie)
	{
		for (auto const& letter : productie.m_stanga)
		{
			if (caractere.find(letter) == caractere.end())
				return false;
		}
		for (auto const& letter : productie.m_dreapta)
		{
			if (caractere.find(letter) == caractere.end())
				return false;
		}
	}
	return true;
}

uint16_t Grammar::GetIndiceRandom(const uint16_t& maxim) const
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(0, maxim);
	return distrib(gen);
}

void Grammar::ApplyProduction(const uint16_t& index_productie, std::string& cuvant_modificat) const
{
	std::vector<uint16_t> indiciAparitii = GetIndiciAparitii(index_productie, cuvant_modificat);
	uint16_t indiceAparitieRandom = GetIndiceRandom(indiciAparitii.size()-1);
	ReplaceInString(index_productie, cuvant_modificat, indiciAparitii[indiceAparitieRandom]);
}

std::vector<uint16_t> Grammar::GetIndiciAparitii(const uint16_t& index_productie, const std::string& cuvant_modificat) const
{
	std::vector<uint16_t> indiciAparitii;
	const std::regex pattern(m_productie[index_productie].m_stanga);
	std::smatch base_match;
	auto it = std::sregex_iterator(cuvant_modificat.begin(), cuvant_modificat.end(), pattern);
	for (it; it != std::sregex_iterator(); it++)
	{
		base_match = *it;
		indiciAparitii.emplace_back(base_match.position());
	}
	return indiciAparitii;
}

void Grammar::ReplaceInString(const uint16_t& index_productie, std::string& cuvant_modificat, const uint16_t& indiceStart) const
{
	//PrintProduction(index_productie, cuvant_modificat, indiceStart);
	if (m_productie[index_productie].m_dreapta[0] == k_lambda)
	{
		cuvant_modificat.replace(indiceStart,
			m_productie[index_productie].m_stanga.size(), "");
	}
	else
	{
		cuvant_modificat.replace(indiceStart,
			m_productie[index_productie].m_stanga.size(), m_productie[index_productie].m_dreapta);
	}
}

void Grammar::PrintProduction(const uint16_t& index_productie, const std::string& cuvant_modificat, const uint16_t& indiceStart) const
{
	std::cout << "CUVANT CURENT: " << cuvant_modificat << "\n";
	std::cout << "LA INDEXUL " << indiceStart << " SE APLICA " << m_productie[index_productie].m_stanga << " -> " << m_productie[index_productie].m_dreapta <<"\n";
}

std::istream& operator>>(std::istream& in, Grammar& gramatica)
{
	std::string auxiliar;
	std::getline(in, auxiliar);
	gramatica.InitializeVnAndVt(auxiliar, true);
	std::getline(in, auxiliar);
	gramatica.InitializeVnAndVt(auxiliar, false);
	std::getline(in, auxiliar);
	gramatica.m_startCaracter = auxiliar[0];
	{
		std::string stanga, dreapta;
		while (!in.eof())
		{
			in >> stanga >> dreapta;
			gramatica.m_productie.emplace_back(Productie{stanga,dreapta});
		}
	}
	return in;
}

std::ostream& operator<<(std::ostream& out, const Grammar& gramatica)
{
	out << "Vn = { ";
	for (const auto& element : gramatica.m_neterminale)
		out << element << " ";
	out << "}\nVt = { ";
	for (const auto& element : gramatica.m_terminale)
		out << element << " ";
	out << "}\nCaracter start: " << gramatica.m_startCaracter;
	out << "\nLambda: " << gramatica.k_lambda;
	out << "\nProductii:\n";
	for (const auto& productie : gramatica.m_productie)
		out << productie.m_stanga << " -> " << productie.m_dreapta<< "\n";
	return out;
}
