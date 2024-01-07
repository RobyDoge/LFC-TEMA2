#include "PolishForm.h"

#include <iostream>
#include <ranges>
#include <algorithm>

PolishForm::PolishForm(std::string expression)
{
	ToPolishForm(expression);

    //std::cout << "Infix Expression: " << expression << std::endl;
    //std::cout << "Polish Form: " << m_result << std::endl;
}

std::string PolishForm::GetResult()
{
	return m_result;
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

void PolishForm::ToPolishForm(const std::string& expression)
{
    for (char c : expression) {
        if (isalpha(c)) {
            m_result += c;  // Operand, append to the result
        }
        else if (c == '(') {
            m_operators.push(c);
        }
        else if (c == ')') {
            while (!m_operators.empty() && m_operators.top() != '(') {
                m_result += m_operators.top();
                m_operators.pop();
            }
            m_operators.pop();  // Pop '('
        }
        else if (c == '|' || c == '.') {
            while (!m_operators.empty() && GetOperatorPrecedence(m_operators.top()) >= GetOperatorPrecedence(c)) {
                m_result += m_operators.top();
                m_operators.pop();
            }
            m_operators.push(c);
        }
        else if (c == '*') {
            m_operators.push(c);
        }
    }

    while (!m_operators.empty()) {
        m_result += m_operators.top();
        m_operators.pop();
    }
}
