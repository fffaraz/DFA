#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

#include "DFA.h"

enum TokenType
{
	TK_OP_INVALID     = 100, 
	TK_PARAN_OPEN,    // (
	TK_PARAN_CLOSE,   // )
	TK_OP_PLUS,       // +
	TK_OP_TIMES,      // *
	TK_OP_ASSIGNMENT, // =
	TK_WHITESPACE,    // white space
	TK_ID,			  // Identifier
	TK_INTEGER,       // Integer
	TK_EOS,			  // $, End of String
};

struct Token
{
	TokenType type;
	string lexeme;
};

static string token_name(TokenType token)
{
	switch (token)
	{
	case TK_OP_INVALID:		return "TK_OP_INVALID";
	case TK_PARAN_OPEN:		return "TK_PARAN_OPEN";
	case TK_PARAN_CLOSE:	return "TK_PARAN_CLOSE";
	case TK_OP_PLUS:		return "TK_OP_PLUS";
	case TK_OP_TIMES:		return "TK_OP_TIMES";
	case TK_OP_ASSIGNMENT:	return "TK_OP_ASSIGNMENT";
	case TK_WHITESPACE:		return "TK_WHITESPACE";
	case TK_ID:				return "TK_ID";
	case TK_INTEGER:		return "TK_INTEGER";
	case TK_EOS:			return "TK_EOS";
	default:				return "default";
	}
}


class Lexer
{

public:
	explicit Lexer() :
		dfa(0, false)
	{
		dfa.add_state(TokenType::TK_PARAN_OPEN    , true);
		dfa.add_state(TokenType::TK_PARAN_CLOSE   , true);
		dfa.add_state(TokenType::TK_OP_PLUS       , true);
		dfa.add_state(TokenType::TK_OP_TIMES      , true);
		dfa.add_state(TokenType::TK_OP_ASSIGNMENT , true);
		dfa.add_state(TokenType::TK_WHITESPACE    , true);
		dfa.add_state(TokenType::TK_ID            , true);
		dfa.add_state(TokenType::TK_INTEGER       , true);

		dfa.add_transition(0, '(', TokenType::TK_PARAN_OPEN);
		dfa.add_transition(0, ')', TokenType::TK_PARAN_CLOSE);
		dfa.add_transition(0, '+', TokenType::TK_OP_PLUS);
		dfa.add_transition(0, '*', TokenType::TK_OP_TIMES);
		dfa.add_transition(0, '=', TokenType::TK_OP_ASSIGNMENT);

		dfa.add_transition(0, ' ', TokenType::TK_WHITESPACE);
		dfa.add_transition(TokenType::TK_WHITESPACE, ' ', TokenType::TK_WHITESPACE);

		for(char c='a'; c<='z'; c++)
		{
			dfa.add_transition(0, c, TokenType::TK_ID);
			dfa.add_transition(TokenType::TK_ID, c, TokenType::TK_ID);
		}

		for(char c='0'; c<='9'; c++)
		{
			dfa.add_transition(TokenType::TK_ID, c, TokenType::TK_ID);
		}


		for(char c='0'; c<='9'; c++)
		{
			dfa.add_transition(0, c, TokenType::TK_INTEGER);
			dfa.add_transition(TokenType::TK_INTEGER, c, TokenType::TK_INTEGER);
		}

	}

	vector<Token> lex(string input)
	{
		vector<Token> result;
		string lexeme;
		dfa.reset();
		input+='$';
		for(auto i=input.begin(); i<input.end()-1; i++)
		{
			lexeme += *i;
			dfa.input(*i);
			char next = *(i+1);
			if(dfa.is_accepting() && !dfa.is_accepting(next))
			{
				Token tk;
				tk.lexeme = lexeme;
				tk.type = (TokenType)dfa.state();
				result.push_back(tk);
				lexeme.clear();
				dfa.reset();
			}
		}
		Token eos;
		eos.type = TokenType::TK_EOS;
		eos.lexeme = "$";
		result.push_back(eos);
		return result;
	}

private:
	DFA<char> dfa;
};

enum Symbols
{
	NTS = 200,
	// Non-terminal symbols:
	NTS_E,
	NTS_T,
	NTS_X,
	NTS_Y,
};

class Parser
{
public:
	explicit Parser()
	{
		// initialize the symbols stack
		m_stack.push((Symbols)TokenType::TK_EOS);
		m_stack.push(Symbols::NTS_E);

		// setup the parsing table
		m_table[Symbols::NTS_E][(Symbols)TokenType::TK_INTEGER]		= 1; // 1. E -> T X
		m_table[Symbols::NTS_E][(Symbols)TokenType::TK_PARAN_OPEN]	= 1; // 1. E -> T X
		m_table[Symbols::NTS_X][(Symbols)TokenType::TK_OP_PLUS]		= 2; // 2. X -> + E
		m_table[Symbols::NTS_X][(Symbols)TokenType::TK_PARAN_CLOSE]	= 3; // 3. X -> 
		m_table[Symbols::NTS_X][(Symbols)TokenType::TK_EOS]			= 3; // 3. X -> 
		m_table[Symbols::NTS_T][(Symbols)TokenType::TK_INTEGER]		= 4; // 4. T -> int Y
		m_table[Symbols::NTS_T][(Symbols)TokenType::TK_PARAN_OPEN]	= 5; // 5. T -> ( E )
		m_table[Symbols::NTS_Y][(Symbols)TokenType::TK_OP_TIMES]	= 6; // 6. Y -> * T
		m_table[Symbols::NTS_Y][(Symbols)TokenType::TK_OP_PLUS]		= 7; // 7. Y -> 
		m_table[Symbols::NTS_Y][(Symbols)TokenType::TK_PARAN_CLOSE]	= 7; // 7. Y -> 
		m_table[Symbols::NTS_Y][(Symbols)TokenType::TK_EOS]			= 7; // 7. Y -> 

	}

	void parse(vector<Token> tokens)
	{
		auto tk = tokens.begin();
		while(m_stack.size() > 0)
		{
			if(tk->type == m_stack.top())
			{
				cout << "Matched symbols: " << tk->lexeme  << " (" << token_name(tk->type) << ") " << endl;
				tk++;
				m_stack.pop();
			}
			else
			{
				int rule = m_table[m_stack.top()][(Symbols)tk->type];
				cout << "Rule " << rule << " : ";

				switch(rule)
				{
				case 1: // 1. E -> T X
					m_stack.pop();
					m_stack.push(NTS_X);
					m_stack.push(NTS_T);
					cout << "E -> T X";
					break;

				case 2: // 2. X -> + E
					m_stack.pop();
					m_stack.push(NTS_E);
					m_stack.push((Symbols)TokenType::TK_OP_PLUS);
					cout << "X -> + E";
					break;

				case 3: // 3. X -> 
					m_stack.pop();
					cout << "X -> ";
					break;

				case 4: // 4. T -> int Y
					m_stack.pop();
					m_stack.push(NTS_Y);
					m_stack.push((Symbols)TokenType::TK_INTEGER);
					cout << "T -> int Y";
					break;

				case 5: // 5. T -> ( E )
					m_stack.pop();
					m_stack.push((Symbols)TokenType::TK_PARAN_CLOSE);
					m_stack.push(NTS_E);
					m_stack.push((Symbols)TokenType::TK_PARAN_OPEN);
					cout << "T -> ( E )";
					break;

				case 6: // Y -> * T
					m_stack.pop();
					m_stack.push(NTS_T);
					m_stack.push((Symbols)TokenType::TK_OP_TIMES);
					cout << "Y -> * T";
					break;

				case 7: // Y -> 
					m_stack.pop();
					cout << "Y -> ";
					break;

				default:
					cout << "parsing table defaulted" << endl;
					return;
					break;
				}

				cout<<endl;
			}
		}
	}

private:
	map< Symbols, map<Symbols, int> > m_table;
	stack<Symbols> m_stack;

};

