#include <iostream>
#include <math.h>
#include <vector>
#include <map>
#include <cstdarg>
/*
using SubTerm = std::pair<char, double>;
char VariableName(std::pair<char, double> sub)
{
	return sub.first;
}
double ExponentPart(std::pair<char, double> sub)
{
	return sub.second;
}
struct Term
{
	Term() : coefficient(0.0), variables{} {}
	Term(double c) : coefficient(c), variables{} {}									// 2
	Term(char v) : coefficient(1.0), variables{ { v, 1.0 } } {}						// x
	Term(char v, double e) : coefficient(1.0), variables{ { v, e } } {}				// x^2
	Term(double c, char v, double e) : coefficient(c), variables{ { v, e } } {}		// 2x^2
	Term(double c, size_t count...)													// 2 (4) x^2 y^3 z^1 w^2
	{
		coefficient = c;
		va_list args;
		va_start(args, count);
		while (count) {
			char var = va_arg(args, char);
			double exp = va_arg(args, double);
			variables.insert({ var, exp });
			--count;
		}
		va_end(args);
	}

	double coefficient;
	std::map<char,double> variables;

	SubTerm Get(char name)
	{
		auto it = variables.find(name);
		if (it != variables.end())
			return { it->first, it->second };
		else
			return { '\0', 0.0 };
	}
};
std::ostream& operator<<(std::ostream& stream, const SubTerm& subTerm)
{
	if (VariableName(subTerm) && ExponentPart(subTerm) != 0.0) // ^0 makes it not show
	{
		stream << VariableName(subTerm);
		if (ExponentPart(subTerm) != 1.0) // Hide the exponent when it is 1
			stream << '^' << ExponentPart(subTerm);
	}
	return stream;
}
std::ostream& operator<<(std::ostream& stream, const Term& term)
{
	if (term.coefficient == 0.0) // Anything * 0 = 0 and there's no reason to print just the 0
		return stream;

	if (term.coefficient != 1.0)
		stream << term.coefficient;

	for (SubTerm it : term.variables)
	{
		stream << SubTerm(it);
	}
	return stream;
}

Term operator/(const Term& a, const Term& b)
{
	Term product;
	product.coefficient = a.coefficient / b.coefficient;
	for (size_t i = 0; i < a.variables.size(); ++i)
	{
		product.variables.push_back({ a.variables[i].variable, a.variables[i].exponent });
		for (size_t j = 0; j < b.variables.size(); ++j)
		{
			if (a.variables[i].variable == b.variables[j].variable)
			{
				product.variables.back().exponent -= b.variables[j].exponent;
				break;
			}
		}
	}
	for (size_t i = 0; i < b.variables.size(); ++i)
	{
		for (size_t j = 0; j < product.variables.size(); ++j)
		{
			if (b.variables[i].variable == product.variables[j].variable)
			{
				goto SKIP;
			}
		}
		product.variables.push_back(b.variables[i]);
	SKIP:
		continue;
	}
	return product;
}
Term operator*(const Term& a, const Term& b)
{
	Term product;
	product.coefficient = a.coefficient * b.coefficient;
	for (size_t i = 0; i < a.variables.size(); ++i)
	{
		product.variables.push_back({ a.variables[i].variable, a.variables[i].exponent });
		for (size_t j = 0; j < b.variables.size(); ++j)
		{
			if (a.variables[i].variable == b.variables[j].variable)
			{
				product.variables.back().exponent += b.variables[j].exponent;
				break;
			}
		}
	}
	for (size_t i = 0; i < b.variables.size(); ++i)
	{
		for (size_t j = 0; j < product.variables.size(); ++j)
		{
			if (b.variables[i].variable == product.variables[j].variable)
			{
				goto SKIP;
			}
		}
		product.variables.push_back(b.variables[i]);
	SKIP:
		continue;
	}
	return product;
}
Term abs(Term term)
{
	term.coefficient = abs(term.coefficient);
	return term;
}

double VectorMax(std::vector<double> numbers)
{
	double max = -INFINITY;
	for (double n : numbers)
	{
		if (n > max) max = n;
	}
	return max;
}
double VectorMin(std::vector<double> numbers)
{
	double min = INFINITY;
	for (double n : numbers)
	{
		if (n < min) min = n;
	}
	return min;
}

bool IsEvenlyDivisible(double a, double b)
{
	return (floor(a / b) * b) == a;
}
double GCF(double a, double b)
{
	for (double guess = std::max(a,b); guess > 0.0; --guess)
	{
		if (IsEvenlyDivisible(a, guess) && IsEvenlyDivisible(b, guess))
			return guess;
	}
}
double MultiGCF(std::vector<double> numbers)
{
	double max = -INFINITY;
	for (double n : numbers)
	{
		if (n > max) max = n;
	}
	for (double guess = max; guess > 0.0; --guess)
	{
		for (double n : numbers)
		{
			if (!IsEvenlyDivisible(n, guess))
				goto LOOP;
		}
		return guess;
	LOOP:
		continue;
	}
}
#define ADD_OR_SUB(x) (x > 0.0 ? '+' : '-')
void Binomial(Term a, Term b) // Uses GCF method
{
	std::cout << a << ADD_OR_SUB(b.coefficient) << abs(b) << std::endl << '=';
	if (a.variables.empty())
	{
		if (b.variables.empty())
		{
			std::cout << a.coefficient << ADD_OR_SUB(b.coefficient) << abs(b.coefficient);
		}
		else
		{
			std::cout << a.coefficient << ADD_OR_SUB(b.coefficient) << abs(b);
		}
	}
	else if (b.variables.empty())
	{
		std::cout << a << ADD_OR_SUB(b.coefficient) << abs(b.coefficient);
	}
	else if (a.variables[0].variable == b.variables[0].variable)
	{		
		Term gcfTerm( GCF(a.coefficient, b.coefficient), { SubTerm( a.variables[0].variable, std::min(a.variables[0].exponent, b.variables[0].exponent) ) } );
		for (size_t i = 0; i < std::max(a.variables.size(), b.variables.size()); ++i)
		{
			if ()
		}
		Term a1 = a / gcfTerm;
		Term b1 = b / gcfTerm;
		std::cout << gcfTerm << '(' << a1 << ADD_OR_SUB(b1.coefficient) << abs(b1) << ')' << std::endl << std::endl;
	}
}

std::vector<double> CoefficientsOf(std::vector<Term>& terms)
{
	std::vector<double> coefficients;
	coefficients.reserve(terms.size());
	for (Term& term : terms)
	{
		coefficients.push_back(term.coefficient);
	}
	return coefficients;
}
std::vector<SubTerm> SubTermsOf(std::vector<Term>& terms)
{
	std::vector<SubTerm> subs;

	size_t total = 0;
	for (Term& term : terms)
	{
		total += term.variables.size();
	}
	subs.reserve(total);

	for (Term& term : terms)
	{
		for (SubTerm& sub : term.variables)
		{
			subs.push_back(sub);
		}
	}

	return subs;
}
std::vector<char> VariablesOf(const Term& term)
{
	std::vector<char> vars;
	for (const SubTerm& sub : term.variables)
	{
		vars.push_back(sub.variable);
	}
	return vars;
}
std::vector<char> VariablesOf(std::vector<Term>& terms)
{
	std::vector<char> vars;
	for (Term& term : terms)
	{
		for (SubTerm& sub : term.variables)
		{
			char v = sub.variable;
			for (char c : vars)
			{
				if (v == c)
					goto SKIP;
			}
			vars.push_back(v);
		SKIP:
			continue;
		}
	}
	return vars;
}

void Trinomial(Term a, Term b, Term c) // Uses AC method
{
	std::cout << a << ADD_OR_SUB(b.coefficient) << abs(b) << ADD_OR_SUB(c.coefficient) << abs(c) << std::endl;


}

void Polynomial(std::vector<Term> terms) // Uses AC method
{
	std::cout << terms[0];
	for (Term& term : terms)
	{
		std::cout << (term.coefficient < 0.0 ? '-' : '+') << abs(term);
	}
	std::cout << std::endl;
}

int mainn()
{
	Term a{ 12, { SubTerm{ 'x',2 }, SubTerm{ 'y',2 } } };
	Term b{ 15, { SubTerm{ 'x',2 }, SubTerm{ 'y',2 }, SubTerm{ 'z',3 } } };
	std::cout << a << std::endl << b << std::endl;
	Binomial({ 12.0, { { 'y', 2.0 } } }, { 15.0, { { 'y', 3.0 } } });
	//Binomial({ 5.0, 'a' }, { -4.0, 'a', 2.0 });
	//Polynomial({ { 4,'x',2 }, { 3,'x' }, { -18 } });
	return 0;
}
*/