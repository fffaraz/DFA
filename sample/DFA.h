#pragma once

#include <vector>
#include <set>
#include <map>

using namespace std;

template<typename T>
class DFA
{
public:
	explicit DFA(int initial, bool isfinal);
	~DFA(void);
	void add_state(int s, bool isfinal);
	void add_transition(int src, T input, int dest);
	bool is_accepting();
	bool is_accepting(T inp);
	void reset();
	int input(T inp);
	int state();


private:
	int m_initial;
	int m_state;
	set<int> m_states;
	set<int> m_final_states;
	map<pair<int,T>, int> m_transitions;
};
