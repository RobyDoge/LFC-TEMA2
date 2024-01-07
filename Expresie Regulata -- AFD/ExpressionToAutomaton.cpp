#include "ExpressionToAutomaton.h"

#include <algorithm>

#include "PolishForm.h"

ExpressionToAutomaton::ExpressionToAutomaton(std::istream& input)
{
	std::getline(input, m_expression);
}

DeterministicFiniteAutomaton ExpressionToAutomaton::CreateAutomatonFromPolishForm()
{
	char index = 'A';

	for(auto& character: m_polishForm)
	{
		if(std::isalnum(character))
		{
			LetterStep(index, character);
		}
		else if(character == '.')
		{
			ConcatenationStep();
		}
		else if(character == '|')
		{
			OrStep(index);
		}
		else if(character == '*')
		{
			KleeneStarStep(index);
		}
	}

	DeterministicFiniteAutomaton automaton = m_intermediateAutomatons.top();
	m_intermediateAutomatons.pop();
	return automaton;
}

bool ExpressionToAutomaton::IsExpressionValid()
{
	return true;
}

std::string ExpressionToAutomaton::GetExpression()
{
	return m_expression;
}

void ExpressionToAutomaton::SetPolishForm()
{
	PolishForm polishForm(m_expression);
	m_polishForm = polishForm.GetResult();
}

// on tis step there are only two states, the start state and the final state
// there is only one transition function, from the start state to the final state, via the letter
void ExpressionToAutomaton::LetterStep(char& index, char character)
{
	DeterministicFiniteAutomaton newAutomaton;
	std::string states;
	std::unordered_multimap<string, char> transitions;

	newAutomaton.SetInputAlphabet({ character });
	newAutomaton.SetStartState(index);
	states += index;

	transitions.insert({ { index, character }, index + 1 });
	newAutomaton.SetTransitions(transitions);

	index++;
	newAutomaton.SetFinalStates({ index });
	states += index;


	newAutomaton.SetStates(states);
	m_intermediateAutomatons.push(newAutomaton);

	//we need to increment the index for the next automaton
	index++;
}


// . - concatenation
// on this step we take the last two automata from the stack and we concatenate them
// this means that we take the final state of the first automaton and the start state of the second automaton and keep only one of them
// there are two transition functions, one from the start state of the first automaton to the start state of the second automaton
// and one from the final state of the first automaton to the final state of the second automaton
void ExpressionToAutomaton::ConcatenationStep()
{
	//we remove the last two automata from the stack
	DeterministicFiniteAutomaton secondAutomaton = m_intermediateAutomatons.top();//states: C, D
	m_intermediateAutomatons.pop();
	DeterministicFiniteAutomaton firstAutomaton = m_intermediateAutomatons.top(); //states: A, B
	m_intermediateAutomatons.pop();

	DeterministicFiniteAutomaton newAutomaton;
	char character = secondAutomaton.GetInputAlphabet()[0];//b
	char finalState = secondAutomaton.GetFinalStates()[0];//D
	// the 3 states: A, B, D
	// C is not a state anymore, it coincides with B
	std::string states = firstAutomaton.GetStates() + finalState;
	std::unordered_multimap<string, char> transitions = firstAutomaton.GetTransitions();// A->B
	std::string inputAlphabet = firstAutomaton.GetInputAlphabet() + secondAutomaton.GetInputAlphabet();

	EliminateDuplicates(inputAlphabet);

	std::ranges::for_each(secondAutomaton.GetTransitions(), [&](const auto& transition)
	{
		if(transition.first[0] == secondAutomaton.GetStartState())
			transitions.insert({ { firstAutomaton.GetFinalStates()[0], transition.first[1] }, transition.second });
		else
		transitions.insert({ { transition.first[0], transition.first[1] }, transition.second });
	});

	newAutomaton.SetStates(states);
	newAutomaton.SetInputAlphabet(inputAlphabet);
	newAutomaton.SetStartState(firstAutomaton.GetStartState());
	newAutomaton.SetFinalStates({finalState});
	newAutomaton.SetTransitions(transitions);

	m_intermediateAutomatons.push(newAutomaton);
}

// | - or
// on this step we take the last two automatons from the stack and we make an or between them
// this means we add a new start state and a new final state (via the index, using epsilon transitions)
// the states of the new automaton are: automaton1 states + automaton2 states + new start state + new final state
// the input alphabet of the new automaton is the union of the input alphabets of the two automatons + epsilon = $
// the transitions of the new automaton are: the transitions of the two automatons +
//											epsilon transitions from the new start state to the start states of the two automatons +
//											epsilon transitions from the final states of the two automatons to the new final state
void ExpressionToAutomaton::OrStep(char& index)
{
	//we remove the last two automatons from the stack
	DeterministicFiniteAutomaton secondAutomaton = m_intermediateAutomatons.top();
	m_intermediateAutomatons.pop();
	DeterministicFiniteAutomaton firstAutomaton = m_intermediateAutomatons.top();
	m_intermediateAutomatons.pop();

	DeterministicFiniteAutomaton newAutomaton;
	std::unordered_multimap<string, char> transitions;

	

	transitions.insert({ { index, '$' }, firstAutomaton.GetStartState() });
	transitions.insert({ { index, '$' }, secondAutomaton.GetStartState() });

	std::ranges::for_each(firstAutomaton.GetTransitions(), [&](const auto& transition)
	{
				transitions.insert({ { transition.first[0], transition.first[1] }, transition.second });
	});
	std::ranges::for_each(secondAutomaton.GetTransitions(), [&](const auto& transition)
	{
						transitions.insert({ { transition.first[0], transition.first[1] }, transition.second });
	});

	//transitions.insert(firstAutomaton.GetTransitions().begin(), firstAutomaton.GetTransitions().end());		//nu merge???
	//transitions.insert(secondAutomaton.GetTransitions().begin(), secondAutomaton.GetTransitions().end());
	transitions.insert({ { firstAutomaton.GetFinalStates()[0], '$' }, index + 1 });
	transitions.insert({ { secondAutomaton.GetFinalStates()[0], '$' }, index + 1 });

	newAutomaton.SetStates(firstAutomaton.GetStates() + secondAutomaton.GetStates() + index + char(index + 1));
	newAutomaton.SetInputAlphabet(firstAutomaton.GetInputAlphabet() + secondAutomaton.GetInputAlphabet() + '$');
	newAutomaton.SetStartState(index);
	newAutomaton.SetFinalStates({ char(index + 1) });
	newAutomaton.SetTransitions(transitions);

	index += 2;
	m_intermediateAutomatons.push(newAutomaton);
}

// * - kleene star
// on this step we take the last automaton from the stack and we make a kleene star on it
// this means we add a new start state and a new final state (via the index, using epsilon transitions)
// the states of the new automaton are: automaton states + new start state + new final state
// the input alphabet of the new automaton is the input alphabet of the automaton + epsilon = $
// the transitions of the new automaton are: the transitions of the automaton +
//											epsilon transitions from the new start state to the start state of the automaton +
//											epsilon transitions from the final state of the automaton to the new final state +
//											epsilon transitions from the final state of the automaton to the start state of the automaton +
//											epsilon transitions from the new start state to the new final state
void ExpressionToAutomaton::KleeneStarStep(char& index)
{
	//we remove the last automaton from the stack
	DeterministicFiniteAutomaton automaton = m_intermediateAutomatons.top();
	m_intermediateAutomatons.pop();

	DeterministicFiniteAutomaton newAutomaton;
	newAutomaton.SetStates(automaton.GetStates() + index + char(index + 1));
	newAutomaton.SetInputAlphabet(automaton.GetInputAlphabet() + '$');
	newAutomaton.SetStartState(index);
	newAutomaton.SetFinalStates({ char(index + 1) });
	newAutomaton.SetTransitions(automaton.GetTransitions());

	index += 2;
	m_intermediateAutomatons.push(newAutomaton);
}

void ExpressionToAutomaton::EliminateDuplicates(std::string& string)
{
	std::ranges::sort(string);
	string.erase(std::ranges::unique(string).begin(), string.end());
}
