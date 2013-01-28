
#include <iostream>
using namespace std;

#include "DFA.h"
#include "compiler.h"

int main(int argc, char* argv[])
{
	Lexer lexer;

	string inp;
	getline(cin, inp);

	cout<<endl<<"----------------------------------------" <<endl<<endl;

	cout<<"Tokens List:"<<endl<<endl;

	auto tk = lexer.lex(inp);
	for(auto i=tk.begin(); i<tk.end(); i++)
		cout<< i->lexeme << " : " << token_name(i->type) << endl;

	cout<<endl<<"----------------------------------------" <<endl<<endl;

	Parser parser;
	parser.parse(tk);


	cin.get();
	return 0;
}
