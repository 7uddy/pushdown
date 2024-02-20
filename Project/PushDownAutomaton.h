#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <tuple>
#include <map>
#include <algorithm>

//Automatul accepta limbajul prin stiva vida
//In algoritm, m_states va coincide cu m_initialState (m_states va avea un singur caracter)
class PushDownAutomaton
{
public:
	PushDownAutomaton()=default;
	~PushDownAutomaton()=default;
	
	bool CheckWord(const std::string& word);
	bool IsDeterministic();
	void PrintAutomaton() const noexcept;

	//Setters
	void SetStates(const std::vector<char>& states);
	void SetAlphabet(const std::vector<char>& alphabet);
	void SetStackAlphabet(const std::vector<char>& stack_alphabet);
	void SetInitialState(const char& startState);
	void SetFirstStackChar(const char& firstStackChar);
	void AddTransition(const std::tuple<char, char, char>& trans, const std::pair<char, std::vector<char>>& result);


	friend std::ostream& operator<<(std::ostream& out, const PushDownAutomaton& pda);
public:
	static const char kLambda = '@';


private:
	bool CheckWordBacktraking(std::string word, std::stack<char> statesStack, char currentState);
	bool ContainsOnlyAlphabetLetters(const std::string& word);

private:
	std::vector<char> m_states;
	char m_initialState;
	std::vector<char> m_alphabet;
	//a b c se duc in {(a,b), (d,efg)}
	std::map<std::tuple<char, char, char>, std::vector<std::pair<char, std::vector<char>>>> m_transitions;
	std::vector<char> m_stackAlphabet;
	char m_firstStackChar;
};

