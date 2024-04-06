#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <utility>

// Osservazione
// Il numero di operatori e' uguale al numero delle parentesi aperte + 1

std::string error_type;

int arithmetic(int op1, int op, int op2)
{
	switch (op)
	{
		case '+':
			if (op1 > 0 && op2 > 0 && op1 + op2 < 0)
			{
				error_type = "Overflow";
				throw std::invalid_argument(error_type);
			}
			return op1 + op2;
		case '-':
			if (op1 < 0 || op2 < 0 && op1 - op2 > 0)
			{
				error_type = "Overflow";
				throw std::invalid_argument(error_type);
			}
			return op1 - op2;
		case '*':
			if (op1 > 0 && op2 > 0 && op1 * op2 < 0)
			{
				error_type = "Overflow";
				throw std::invalid_argument(error_type);
			}
			return op1 * op2;
		case '/':
			if (op2 == 0)
			{
				error_type = "Divisione per 0";
				throw std::invalid_argument(error_type);
			}
			return op1 / op2;
		default:
			throw std::exception("Invalid operator");
	}
	throw std::exception("Invalid operator");
}


enum token_t
{
	NUMBER,
	OPEN_PAREN,
	CLOSE_PAREN,
	OPERATOR,
};

int i = 0;
int eval(std::vector<std::pair<int, token_t>> tokens) 
{
	if (tokens.at(i).second == OPEN_PAREN)
	{
		i += 1;
		int r = eval(tokens);
		if (i >= tokens.size() || tokens.at(i).second != CLOSE_PAREN)
		{
			throw std::invalid_argument("Expected CLOSE PAREN token");
		}
		tokens.at(i).first = r; // overwriting ")" with the result. Expecting ")" to overwrite.
		tokens.at(i).second = NUMBER;
		if (i+1 >= tokens.size())
		{
			i++;
			return r;
		}
		return eval(tokens);
	}
	if (tokens.at(i).second == NUMBER && tokens.at(i + 1).second == NUMBER)
	{
		throw std::invalid_argument("Expected operator or parenthesis, but received NUMBER token");
	}
	if (i + 2 >= tokens.size())
	{
		return tokens.at(i).first;
	}
	if (tokens.at(i).second == NUMBER && tokens.at(i + 2).second == NUMBER)
	{
		int op1 = tokens.at(i).first;
		int op = tokens.at(i + 1).first;
		int op2 = tokens.at(i + 2).first;
		i += 3; // closing )
		return arithmetic(op1, op, op2);
	}
	/*if (tokens.at(i).second == NUMBER && tokens.at(i + 2).second == OPERATOR ||
		tokens.at(i).second == OPERATOR && tokens.at(i + 2).second == NUMBER)
	{
		throw std::invalid_argument("Expected NUMBER token, received OPERATOR token");
	}*/
	if (tokens.at(i).second == NUMBER && tokens.at(i + 2).second == OPEN_PAREN)
	{
		int op1 = tokens.at(i).first;
		int op = tokens.at(i+1).first;
		i += 2;
		return arithmetic(op1, op, eval(tokens));
	}
	if (tokens.at(i).second != NUMBER)
	{
		throw std::invalid_argument("Expected a NUMBER token.");
	}

	return tokens.at(i++).first;
}



// returns 1 == too many parenthesis
// returns 4 == too many operators
// returns 2 == missing operator (NUMBER NUMBER)
// returns 3 == missing numbers (OPERATOR OPERATOR)
int grammar_checker(std::vector<std::pair<int, token_t>> tokens)
{
	int open_paren = 0;
	int ops = 0;
	for (auto& token : tokens)
	{
		if (token.second == OPEN_PAREN)
		{
			open_paren++;
		}
		if (token.second == OPERATOR)
		{
			ops++;
		}
	}

	for (int i = 1; i < tokens.size(); i++)
	{
		if (tokens.at(i - 1).second == tokens.at(i).second && (tokens.at(i).second != OPEN_PAREN || tokens.at(i).second != CLOSE_PAREN))
		{
			if (tokens.at(i).second == NUMBER)
			{
				return 2;
			}
			if (tokens.at(i).second == OPERATOR)
			{
				return 3;
			}
		}
	}
	if (ops == open_paren + 1)
	{
		return 0; // OK
	}
	if (ops > open_paren + 1)
	{
		return 4;
	}
	return 1;
}

std::vector<std::pair<int, token_t>> lex(std::string program)
{
	std::vector<std::pair<int, token_t>> result;
	int i = 0;

	std::function<char()> current = [&]() -> char
		{
			if (i >= program.length())
			{
				return '\0';
			}
			return program[i];
		};


	std::function<void()> advance = [&]() -> void
		{
			i++;
		};

	while (current() != '\0')
	{
		while (current() == ' ')
		{
			advance();
		}
		if (isdigit(current()))
		{
			int start = i;
			while (isdigit(current()))
			{
				advance();
			}
			int length = i - start;
			int number = std::stoi(program.substr(start, length));
			result.push_back( {number, NUMBER} );
			continue;
		}
		
		switch (current())
		{
		case '+': 
		case '-':
		case '*':
		case '/':
			result.push_back({ current(), OPERATOR });
			break;
		case '(':
			result.push_back({ current(), OPEN_PAREN });
			break;
		case ')':
			result.push_back({ current(), CLOSE_PAREN });
			break;
		default:
			throw std::invalid_argument("Invalid character found.");
		}
		advance();
	}
	return result;
}

struct test_data
{
	std::string expr;
	int expected_result;
};

int main()
{
	std::vector<test_data> test_datas;
	test_datas.push_back({ "((1+2)*(3*2))-(1+(1024/3))", -324 }); // too many parenthesis
	test_datas.push_back({ "((00000-2)*(1024+1024)) / 2", (int)((00000 - 2) * (1024 + 1024)) / 2 });
	test_datas.push_back({ "1+(1+(1+(1+(1+(1+(1+0))))))", (int)(1 + (1 + (1 + (1 + (1 + (1 + (1 + 0)))))) ) });
	test_datas.push_back({ "2*(2*(2*(2*(2*(2*(2*(2*(2*(2*(2*(1024*1024)))))))))))", (int)(2 * (2 * (2 * (2 * (2 * (2 * (2 * (2 * (2 * (2 * (2 * (1024 * 1024)))))))))))) }); // overflow
	test_datas.push_back({ "2147483647+0", (int)(2147483647 + 0) });
	test_datas.push_back({ "2147483647+1", (int)(2147483647 + 1) }); // overflow
	test_datas.push_back({ "(0-2147483647)-1", (int)((0 - 2147483647) - 1) });
	test_datas.push_back({ "(0-2147483647)-2", (int)((0 - 2147483647) - 2) }); // overflow

	std::vector<std::string> programs = {
		"2 5",
		"3 +* 2",
		"2 + (6 * 5",
		"2 + 6 * 5",
		"- 5 + 6",
		"(2 + 5)",
		"((1+2)*(3*2))-(1+(1024/3))",
		"((00000-2)*(1024+1024)) / 2",
		"1+(1+(1+(1+(1+(1+(1+0))))))",
		"2*(2*(2*(2*(2*(2*(2*(2*(2*(2*(2*(1024*1024)))))))))))",
		"2147483647+0",
		"2147483647+1",
		"(0-2147483647)-1",
		"(0-2147483647)-2",
	};

	for (auto& program : programs)
	{
		try
		{
			std::vector<std::pair<int, token_t>> tokens = lex(program);
			int grammar_result = grammar_checker(tokens);
			if (grammar_result != 0)
			{
				if (grammar_result == 1)
				{
					throw std::invalid_argument("Too many parenthesis");
				}
				if (grammar_result == 2)
				{
					throw std::invalid_argument("Missing operator (NUMBER NUMBER)");
				}
				if (grammar_result == 3)
				{
					throw std::invalid_argument("Missing numbers (OPERATOR OPERATOR)");
				}
				if (grammar_result == 4)
				{
					throw std::invalid_argument("Too many operators.");
				}
			}
			int result = eval(tokens);
			std::cout << "Result: " << result << std::endl;
		}
		catch (std::invalid_argument e)
		{
			std::cout << "Invalid Argument: " << e.what() << std::endl;
		}
		catch (std::exception e)
		{
			std::cout << "General Exception: " << e.what() << std::endl;
		}
		i = 0;
		
	}
}


