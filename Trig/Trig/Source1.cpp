#include <iostream>
#include <map>
#include <unordered_set>
#include <vector>
#include <math.h>
#include <cstdarg>

/*
struct Term
{
	double coef;
	std::map<char, double> vars;
};
using Variable_t = std::pair<const char, double>;
Term operator*(Term a, Term b)
{
	a.coef *= b.coef;
	for (Variable_t& varA : a.vars)
	{
		auto varB = b.vars.find(varA.first);
		if (varB != b.vars.end())
		{
			varA.second = varA.second + varB->second;
		}
	}
	for (const Variable_t& search : b.vars)
	{
		auto it = a.vars.find(search.first);
		if (it == a.vars.end())
		{
			a.vars.insert(search);
		}
	}
	return a;
}
Term operator/(Term a, Term b)
{
	a.coef /= b.coef;
	for (Variable_t& varA : a.vars)
	{
		auto varB = b.vars.find(varA.first);
		if (varB != b.vars.end())
		{
			varA.second = varA.second - varB->second;
			if (varA.second < 0.0)
			{
				varA.second = varA.second;
			}
		}
	}
	for (const Variable_t& search : b.vars)
	{
		auto it = a.vars.find(search.first);
		if (it == a.vars.end())
		{
			a.vars.insert({ search.first, -search.second });
		}
	}
	return a;
}


void PrintVar(const Variable_t& var)
{
	if (var.second != 0.0)
	{
		printf("%c", var.first);
		if (var.second != 1.0)
			printf("^%1.0f", var.second);
	}
}
void PrintTerm(const Term& term)
{
	if (term.coef != 0.0)
	{
		if (term.coef != 1.0)
			std::cout << term.coef;

		bool divisor = false;
		for (const Variable_t& var : term.vars)
		{
			if (var.second < 0.0)
			{
				divisor = true;
				break;
			}
		}
		if (divisor)
		{
			printf("((");
			for (const Variable_t& var : term.vars)
			{
				if (var.second > 0.0)
					PrintVar(var);
			}
			printf(")/(");
			for (const Variable_t& var : term.vars)
			{
				if (var.second < 0.0)
					PrintVar({ var.first, abs(var.second) });
			}
			printf("))");
		}
		else
		{
			for (const Variable_t& var : term.vars)
			{
				PrintVar(var);
			}
		}
	}
}
void PrintAddedTerm(Term term)
{
	printf("%c",(term.coef < 0.0 ? '-' : '+'));
	term.coef = abs(term.coef);
	PrintTerm(term);
}

bool IsEvenlyDivisible(double a, double b)
{
	return (floor(a / b) * b) == a;
}

double GCF(const std::vector<Term>& terms)
{
	double max = -INFINITY;
	for (const Term& t : terms)
	{
		if (t.coef > max) max = t.coef;
	}
	for (double guess = max; guess > 0.0; --guess)
	{
		for (const Term& t : terms)
		{
			if (!IsEvenlyDivisible(t.coef, guess))
				goto LOOP;
		}
		return guess;
	LOOP:
		continue;
	}
}
std::map<char,double> MinExponents(const std::vector<Term>& terms)
{
	double min = INFINITY;
	for (const Term& t : terms)
	{
		if (t.coef < min) min = t.coef;
	}
	return min;
}

void FactorGCF(std::vector<Term> terms)
{
	PrintTerm(terms[0]);
	for (const Term& t)
	{
		PrintAddedTerm(t);
	}
	printf("\n=");

	Term gcfTerm;
	gcfTerm.coef = GCF(terms);
	gcfTerm.vars = 
	{ SubTerm(a.variables[0].variable, std::min(a.variables[0].exponent, b.variables[0].exponent)) });
	for (size_t i = 0; i < std::max(a.variables.size(), b.variables.size()); ++i)
	{
		if ()
	}
	for (Term& t : terms)
	{
		t = t / gcfTerm;
	}

	PrintTerm(terms[0]);
	for (const Term& t)
	{
		printf("(");
		PrintTerm(t);
		printf(")");
	}
	std::cout << gcfTerm << '(' << a1 << ADD_OR_SUB(b1.coefficient) << abs(b1) << ')' << std::endl << std::endl;
}
void FactorAC(std::vector<Term> terms)
{

}

int mainnn()
{
	Term t1{ 2.0, { { 'a', 3.0 }, { 'b', 1.0 }, { 'c', 2.0 } } };
	Term t2{ 5.0, { { 'a', 2.0 }, { 'b', 2.0 }, { 'c', 4.0 }, { 'd', 2.0} } };
	Term prod = t1 * t2;
	Term div = t1 / t2;
	PrintTerm(prod);
	std::cout << std::endl;
	PrintTerm(div);
}
*/