#include "PolishForm.h"

#include <iostream>
#include <ranges>
#include <algorithm>

PolishForm::PolishForm(std::string expression)
{
    std::string polishForm = ToPolishForm(expression);

    m_result = polishForm;

    std::cout << "Infix Expression: " << expression << std::endl;
    std::cout << "Polish Form: " << polishForm << std::endl;
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

std::string PolishForm::ToPolishForm(const std::string& expression)
{
    std::string polishForm;

    for (char c : expression) {
        if (isalpha(c)) {
            polishForm += c;  // Operand, append to the result
        }
        else if (c == '(') {
            m_operators.push(c);
        }
        else if (c == ')') {
            while (!m_operators.empty() && m_operators.top() != '(') {
                polishForm += m_operators.top();
                m_operators.pop();
            }
            m_operators.pop();  // Pop '('
        }
        else if (c == '|' || c == '.') {
            while (!m_operators.empty() && GetOperatorPrecedence(m_operators.top()) >= GetOperatorPrecedence(c)) {
                polishForm += m_operators.top();
                m_operators.pop();
            }
            m_operators.push(c);
        }
        else if (c == '*') {
            m_operators.push(c);
        }
    }

    while (!m_operators.empty()) {
        polishForm += m_operators.top();
        m_operators.pop();
    }

    return polishForm;
}
