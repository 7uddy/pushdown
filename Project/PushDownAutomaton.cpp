#include "PushDownAutomaton.h"

bool PushDownAutomaton::CheckWord(const std::string& word)
{
    if (word.empty())
    {
        std::cout << "\nNo word provided.\n";
        return false;
    }
    if (word.size() == 1 && word[0] == kLambda)
        ;
    else
        if (!ContainsOnlyAlphabetLetters(word))
        {
            std::cout << "\nThe provided word has characters that are not in alphabet of automaton.\n";
            return false;
        }
    std::stack<char> states_stack;
    states_stack.push(m_firstStackChar);
    return CheckWordBacktraking(word, states_stack, m_initialState);
}

bool PushDownAutomaton::ContainsOnlyAlphabetLetters(const std::string& word)
{
    auto isNotInAlphabet = [this](char letter) { return std::find(m_alphabet.begin(), m_alphabet.end(), letter) == m_alphabet.end(); };
    auto found = std::find_if(word.begin(), word.end(), isNotInAlphabet);
    if (found != word.end())
        return false;
    return true;
}

bool PushDownAutomaton::CheckWordBacktraking(std::string word, std::stack<char> statesStack, char currentState)
{
    if (word.empty() && statesStack.empty())
        return true;
    if (word.empty() || statesStack.empty())
        return false;

    auto& result = m_transitions[{currentState, word[0], statesStack.top()}];
    statesStack.pop();
    for (auto it = result.begin(); it != result.end(); it++)
    {
        if (it->second.size() == 1 && it->second[0] == kLambda)
            ;
        else
            for (auto stackCharacter = it->second.rbegin(); stackCharacter != it->second.rend(); stackCharacter++)
            {
                statesStack.push((*stackCharacter));
            }

        bool found = CheckWordBacktraking(word.substr(1, word.length() - 1), statesStack, currentState);
        if (found)
            return true;

        if (it->second.size() == 1 && it->second[0] == kLambda)
            ;
        else
            for (size_t numberOfPops = it->second.size(); numberOfPops>0u; numberOfPops--)
            {
                statesStack.pop();
            }
    }
    return false;
}

bool PushDownAutomaton::IsDeterministic()
{
    for (const auto& tran : m_transitions)
    {
        if (tran.second.size() != 1)
        {
            return false;
        }
    }
    return true;
}

void PushDownAutomaton::PrintAutomaton() const noexcept
{
    std::cout << *this;
}

void PushDownAutomaton::SetStates(const std::vector<char>& states)
{
    m_states = states;
}

void PushDownAutomaton::SetAlphabet(const std::vector<char>& alphabet)
{
    m_alphabet = alphabet;
}

void PushDownAutomaton::SetStackAlphabet(const std::vector<char>& stack_alphabet)
{
    m_stackAlphabet = stack_alphabet;
}

void PushDownAutomaton::SetInitialState(const char& startState)
{
    m_initialState = startState;
}

void PushDownAutomaton::SetFirstStackChar(const char& firstStackChar)
{
    m_firstStackChar = firstStackChar;
}

void PushDownAutomaton::AddTransition(const std::tuple<char, char, char>& trans, const std::pair<char, std::vector<char>>& result)
{
    auto& savedTrans = m_transitions[trans];
    savedTrans.push_back(result);
}

std::ostream& operator<<(std::ostream& out, const PushDownAutomaton& pda)
{
    auto print = [](const char& n) { std::cout << n << ' '; };
    out << "M = ({ ";
    std::ranges::for_each(pda.m_states, print);
    out << "},{ ";
    std::ranges::for_each(pda.m_alphabet, print);
    out << "},{ ";
    std::ranges::for_each(pda.m_stackAlphabet, print);
    out << "}, delta, ";
    out << pda.m_initialState << " , ";
    out << pda.m_firstStackChar << " , multimea_vida)\n";
    out << "delta:\n";
    for (const auto& transition : pda.m_transitions)
    {
        out << "d(" << std::get<0>(transition.first) << "," << std::get<1>(transition.first);
        out << "," << std::get<2>(transition.first) << ") = {";
        for (const auto& p : transition.second)
        {
            out << "( " << p.first << ", ";
            std::ranges::for_each(p.second, print);
            out << ") ";
        }
        out << "}\n";
    }
    return out;
}
