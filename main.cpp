
#include <iostream>
using namespace std;

#include "DFA.h"

int main(int argc, char* argv[])
{
	DFA<char> dfa(0, false);
	dfa.add_state(1, false);
	dfa.add_state(2, true);
	dfa.add_state(3, true);
	dfa.add_transition(0,'a',1);
	dfa.add_transition(1,'b',2);
	dfa.add_transition(1,'c',3);

	char inp;
	while(true)
	{
		cout << "State: " << dfa.state() << "  " << dfa.is_accepting()?"true":"false";
		cout << endl;
		cin >> inp;
		if(cin.eof()) break;
		dfa.input(inp);
	}

	return 0;
}
