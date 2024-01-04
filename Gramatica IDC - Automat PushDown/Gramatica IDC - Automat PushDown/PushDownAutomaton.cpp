#include "PushDownAutomaton.h"
#include <format>
PushDownAutomaton::PushDownAutomaton(Grammar grammar)
{

}

std::ostream& operator<<(std::ostream& os, const PushDownAutomaton& automaton)
{
	os << "Push Down Automaton: M = ({";
	for(const auto state : automaton.m_states)
	{
		os << state;
		if(state != automaton.m_states.back())
		{
			os << ",";
		}
	}
	os << "},{";
	for (const auto symbol : automaton.m_inputAlphabet)
	{
		os << symbol;
		if (symbol != automaton.m_inputAlphabet.back())
		{
			os << ",";
		}
	}
	os << "},{";
	for(const auto stackSymbol : automaton.m_stackAlphabet)
	{
		os << stackSymbol;
		if (stackSymbol != automaton.m_stackAlphabet.back())
		{
			os << ",";
		}
	}
	os << "},P,"<<automaton.m_startingState<<","<<automaton.m_initialStackHead<<",{";
	for(const auto finalState : automaton.m_finalStates)
	{
		os << finalState;
		if (finalState != automaton.m_finalStates.back())
		{
			os << ",";
		}
	}
	os<< "})." << std::endl;
	os<<"P containing the following transitions : \n";
		for (const auto& [inputSymbol, outputSymbols] : automaton.m_transitionFunctions)
		{
			os << std::format("P({},{},{},) -> ({},{})\n",
				inputSymbol[0],inputSymbol[1],inputSymbol[2],outputSymbols[0],outputSymbols[1]);
		}
	return os;
}
