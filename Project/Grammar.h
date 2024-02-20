#pragma once
#include "Productie.h"
#include "PushDownAutomaton.h"
#include <iostream>
#include <string>
#include <fstream>
#include <random>
#include <regex>
#include <unordered_set>
#include <queue>
#include <unordered_map>

class Grammar
{
public:
	Grammar(const std::vector<char>&, const std::vector<char>&, const char&, const std::vector<Productie>&);
	Grammar() = default;

	void PrintGrammar(std::ostream&) const noexcept;
	void ReadGrammarFromFile(const std::string& nume_fisier);

	friend std::istream& operator>>(std::istream& in, Grammar&);
	friend std::ostream& operator<<(std::ostream& out, const Grammar&);

	void SetStartCharacter(const char&);
	std::string GenerateWord() const;
	bool CanGenerateLambda() const noexcept;

	void SortProductions();

	//Barem
	bool VerifyGrammar() const;
	bool IsIDC() const;
	PushDownAutomaton GrammarToAutomaton() const noexcept;

	void ConvertIDCToFNC();
	void RemoveRenames();
	void ReplaceProductionMDWithOnlyNeterminals();
	void ReplaceProductionWhereMDHasSizeBiggerThan2();

	void ConvertFNCToFNG();
	void Lema1(Productie production, int replaceIndex);
	void Lema2(char neterminal);


	

private:
	static const char k_lambda = '@';
private:
	std::vector<char> m_neterminale;
	std::vector<char> m_neterminaleAddedInFNGConversion;
	std::vector<char> m_terminale;
	char m_startCaracter;
	std::vector<Productie> m_productie;
	std::unordered_map<char, char> m_newNames;
private:
	//USED TO OBTAIN FNC
	void ReplaceTerminalWithNeterminal(char caracterToBeReplaced);
	void ReplaceTwoNeterminalsWithNewNeterminal(Productie& productionToBeReplaced);
	char GetNewNeterminal();


	//Initialize grammar
	void InitializeVnAndVt(const std::string&, const bool&);

	//Generate word in grammar
	uint16_t GetIndiceRandom(const uint16_t&) const;
	void ApplyProduction(const uint16_t&, std::string&) const;
	std::vector<uint16_t> GetIndiciAparitii(const uint16_t&, const std::string&) const;
	void ReplaceInString(const uint16_t&, std::string&, const uint16_t&) const;
	void PrintProduction(const uint16_t&, const std::string&, const uint16_t&) const;

	//Conditions for grammar
	bool VnIsPartOfVt() const;
	bool StartCaracterIsInVn() const;
	bool EveryProductionHasOneVn() const;
	bool ExistsOneProductionWithStartCaracter() const;
	bool ProductionsContainOnlyVnAndVt() const;
};