#include "PolishForm.h"

#include <iostream>
#include <ranges>
#include <algorithm>

PolishForm::PolishForm(std::string expression)
{
    std::string polishForm = ToPolishForm(expression);

    std::cout << "Infix Expression: " << expression << std::endl;
    std::cout << "Polish Form: " << polishForm << std::endl;
}

int PolishForm::GetOperatorPrecedence(char op)
{
    switch (op)
    {
    case '*':
        return 3;
    case '.':
        return 2;
    case '|':
        return 1;
    default:
        return 0;
    }
}

std::string PolishForm::ToPolishForm(const std::string& expression)
{
    // Pair of operator and its arity
    std::stack<std::pair<char, int>> arityStack;
    std::string polishForm;


    // Iterate through the expression, add the letters to the polish form and the symbols to the operator stack
    for (char c : expression)
    {
        if (isalpha(c))
        {
            polishForm += c;
        }
        //if the operator is an open parenthesis, we push it to the stack
        else if (c == '(')
        {
            m_operators.push(c);
        }
        //if the operator is a close parenthesis, we pop all the operators from the stack, adding them to the polish form
        else if (c == ')')
        {
            while (!m_operators.empty() && m_operators.top() != '(')
            {
                polishForm += m_operators.top();
                m_operators.pop();
            }

            m_operators.pop();  // Pop '('

            if (!arityStack.empty() && arityStack.top().second == 1) //the only operator that can be unary is the Kleene closure
            {
                polishForm += '*';  // Add Kleene closure - this only happens if the expression is of the form (a)*
                arityStack.pop();
            }
        }
        //if the operator is a concatenation, we pop all the operators with higher or equal precedence from the stack, adding them to the polish form
        //if the operator is an OR, we pop all the operators with higher precedence from the stack, adding them to the polish form
        else if (c == '|' || c == '.')
        {
            //so we would only pop 
            while (!m_operators.empty() && GetOperatorPrecedence(m_operators.top()) >= GetOperatorPrecedence(c))
            {
                polishForm += m_operators.top();
                m_operators.pop();
            }

            m_operators.push(c);

            if (c == '|')
            {
                arityStack.push(std::make_pair(c, 2));  // OR operator is binary
            }
            else if (c == '.')
            {
                arityStack.push(std::make_pair(c, 2));  // Concatenation operator is binary
            }
        }
        else if (c == '*') {
            m_operators.push(c);
            arityStack.push(std::make_pair(c, 1));  // Kleene closure is unary
        }
    }

    while (!m_operators.empty()) {
        polishForm += m_operators.top();
        m_operators.pop();
    }

    return polishForm;
}
