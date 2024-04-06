#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <utility>

int arithmetic(int op1, int op, int op2)
{
	switch (op)
	{
	case '+':
		return op1 + op2;
	case '-':
		return op1 - op2;
	case '*':
		return op1 * op2;
	case '/':
		return op1 / op2;
	default:
		throw std::invalid_argument("Invalid operator");
	}
	
	throw std::invalid_argument("Invalid operator");
}

enum token_t
{
	NUMBER,
	OPEN_PAREN,
	CLOSE_PAREN,
	PLUS,
	MINUS,
	STAR,
	SLASH
};

int i = 0;
int eval(std::vector<std::pair<int, token_t>> tokens) 
{
	if (tokens[i].second == OPEN_PAREN)
	{
		i += 1;
		int r = eval(tokens);
		tokens[i].first = r; // overwriting ")" with the result. Expecting ")" to overwrite.
		tokens[i].second = NUMBER;
		if (i+1 >= tokens.size())
		{
			return r;
		}
		return eval(tokens);
	}
	if (i + 2 >= tokens.size())
	{
		return tokens[i].first;
	}
	if (tokens[i].second == NUMBER && tokens[i+2].second == NUMBER)
	{
		int op1 = tokens[i].first;
		int op = tokens[i + 1].first;
		int op2 = tokens[i + 2].first;
		i += 3; // closing )
		return arithmetic(op1, op, op2);
	}
	if (tokens[i].second == NUMBER && tokens[i+2].second == OPEN_PAREN)
	{
		int op1 = tokens[i].first;
		int op = tokens[i+1].first;
		i += 2;
		return arithmetic(op1, op, eval(tokens));
	}
	return tokens[i++].first;
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
		}
		
		switch (current())
		{
		case '+': 
			result.push_back({ current(), PLUS });
			break;
		case '-':
			result.push_back({ current(), MINUS });
			break;
		case '*':
			result.push_back({ current(), STAR });
			break;
		case '/':
			result.push_back({ current(), SLASH });
			break;
		case '(':
			result.push_back({ current(), OPEN_PAREN });
			break;
		case ')':
			result.push_back({ current(), CLOSE_PAREN });
			break;
		}
		advance();
	}
	return result;
}


int main()
{
	std::string program = "(0-2147483647)-1";
	std::vector<std::pair<int, token_t>> tokens = lex(program);
	int result = eval(tokens);
	std::cout << "Result: " << result;

}