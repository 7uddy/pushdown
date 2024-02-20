#include "Grammar.h"

static void showMenu(Grammar& grammar)
{
	Grammar grammarIDC{ grammar };
	grammar.ConvertIDCToFNC();
	Grammar grammarFNC{ grammar };
	grammar.ConvertFNCToFNG();
	Grammar grammarFNG{ grammar };
	PushDownAutomaton automaton = grammarFNG.GrammarToAutomaton();

	int choice{};
	std::string inputWord{};
	std::string word{};

	do
	{
		std::cout << "\nMENU:\n";
		std::cout << "1. Print grammar.\n";
		std::cout << "2. Generate word in grammar.\n";
		std::cout << "3. Print FNC.\n";
		std::cout << "4. Print FNG.\n";
		std::cout << "5. Generate word in grammar and check in automaton.\n";
		std::cout << "6. Check if input word is accepted by automaton.\n";
		std::cout << "0. Exit menu.\n";
		std::cout << "\nYOUR CHOICE: ";
		std::cin >> choice;
		switch (choice)
		{
		case 1:
			std::cout << '\n';
			grammarIDC.PrintGrammar(std::cout);
			std::cout << "\n";
			break;
		case 2:
			std::cout << "\nWORD IS:\n" << grammarIDC.GenerateWord() << "\n";
			break;
		case 3:
			std::cout << '\n';
			grammarFNC.PrintGrammar(std::cout);
			std::cout << "\n";
			break;
		case 4:
			std::cout << '\n';
			grammarFNG.PrintGrammar(std::cout);
			std::cout << "\n";
			break;
		case 5:
			word = grammar.GenerateWord();
			std::cout << "\nWORD IS:\n" << grammarIDC.GenerateWord() << "\n";
			if (automaton.CheckWord(word))
			{
				std::cout << "The word is valid.\n";
			}
			else std::cout << "The word isn't valid.\n";
			break;
		case 6:
			std::cout << "Enter word to check: ";
			std::cin >> word;
			if (automaton.CheckWord(word))
			{
				std::cout << "\nThe word is valid.\n";
			}
			else std::cout << "\nThe word isn't valid.\n";
			break;
		case 0:
			std::cout << "\nYOU HAVE CHOSEN TO EXIT MENU.\n";
			break;
		default:
			std::cout << "\nINVALID CHOICE. PLEASE ENTER ANOTHER VALUE.\n";
			break;
		}

	} while (choice != 0);
}

int main()
{
	Grammar grammar;
	grammar.ReadGrammarFromFile("TestGrammar.txt");
	if (!grammar.VerifyGrammar())
	{
		std::cout << "GRAMMAR IS NOT VALID.";
		return 1;
	}
	if (!grammar.IsIDC())
	{
		std::cout << "GRAMMAR IS NOT IDC.";
		return 1;
	}

	showMenu(grammar);

	return 0;
}