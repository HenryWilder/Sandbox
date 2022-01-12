#include <iostream>
#include <map>
#include <unordered_set>
#include <vector>
#include <math.h>
#include <cstdarg>

#define DNAN nan("1")

struct Equ
{
	struct Expr
	{
		struct Term
		{
			double coef = 1.0; // Number that all powers are multiplied by

			struct Pow
			{
				Pow() = default;
				Pow(char _var, double _exp) : var(_var), exp(_exp) {}
				Pow(const Pow& src) : var(src.var), exp(src.exp) {}
				Pow(const std::map<char, double>::const_iterator& it) : var(it->first), exp(it->second) {}
				Pow(const std::pair<const char, double>& p) : var(p.first), exp(p.second) {}

				char var = '\0'; // Err if = '\0'
				double exp = 1.0; // Divide if negative; all negatives group at the bottom of the divisor

				bool IsValid() const
				{
					return !!var;
				}
				bool IsDividing() const
				{
					return exp < 0.0;
				}
			};

			std::map<char, double> pows = {}; // Set of powers (variable-exponent pairs)

			Pow GetPow(char var) const
			{
				auto it = pows.find(var);
				if (it == pows.end())
					return { '\0', 0.0 };
				else
					return it;
			}

			double GetDegree() const
			{
				if (pows.empty())
					return 0.0;

				double max = -INFINITY;
				for (auto [var, exp] : pows)
				{
					if (exp > max) max = exp;
				}
				return max;
			}

			// Returns '-' if subtracting, '+' if adding
			char AddSub() const
			{
				return (coef < 0.0 ? '-' : '+');
			}

			// Tells whether this is a constant term or not
			bool IsConstant() const
			{
				return pows.empty();
			}

			bool HasDivisor() const
			{
				for (auto [var, exp] : pows)
				{
					if (exp < 0.0)
						return true;
				}
				return false;
			}
		};

		std::vector<Term> terms = {};

		double GetLeadingCoef() const
		{
			double leading = NAN;
			double max = -INFINITY;
			for (const Term& t : terms)
			{
				double degree = t.GetDegree();
				if (degree > max)
				{
					max = degree;
					leading = t.coef;
				}
			}
			return leading;
		}

		double GetConstantTerm() const
		{
			for (const Term& t : terms)
			{
				if (t.IsConstant())
					return t.coef;
			}
		}
	};
	Expr l = {};
	Expr r = {};
};
using Expr = Equ::Expr;
using Term = Expr::Term;
using Pow = Term::Pow;
std::ostream& operator<<(std::ostream& out, const Pow& pow)
{
	if (pow.exp == 0.0)
		return out;

	out << pow.var;

	if (pow.exp != 1.0)
		out << '^' << pow.exp;

	return out;
}
std::ostream& operator<<(std::ostream& out, const Term& term)
{
	if (term.coef == 0.0)
		return out;

	if (term.coef != 1.0)
		out << term.coef;

	std::vector<Pow> pos, neg;
	for (const Pow& pow : term.pows)
	{
		if (pow.IsDividing())
			neg.push_back(pow);
		else
			pos.push_back(pow);
	}

	if (!neg.empty())
		out << '(';
	if (!pos.empty())
	{
		if (!neg.empty())
			out << '(';

		for (const Pow& pow : pos)
		{
			out << pow;
		}

		if (!neg.empty())
			out << ')';
	}
	else
	{
		out << 1;
	}
	if (!neg.empty())
	{
		out << "/(";
		for (const Pow& pow : neg)
		{
			out << Pow(pow.var, abs(pow.exp));
		}
		out << ")";
	}
}
std::ostream& operator<<(std::ostream& out, const Expr& expr)
{
	if (!expr.terms.empty())
	{
		out << expr.terms[0];
		for (size_t i = 1; i < expr.terms.size(); ++i)
		{
			if (expr.terms[i].coef >= 0.0)
				out << '+';

			out << expr.terms[i];
		}
	}
	return out;
}
std::ostream& operator<<(std::ostream& out, const Equ& equ)
{
	return out << equ.l << '=' << equ.r;
}

struct Triangle
{
	double a, b, c; // Legs
	double A, B, C; // Angles

	double Val(char name)
	{
		switch (name)
		{
		case 'a': return a;
		case 'b': return b;
		case 'c': return c;

		case 'A': return A;
		case 'B': return B;
		case 'C': return C;

		default: return DNAN;
		}
	}
	char Hyp()
	{
		if (a > b)
		{
			if (a > c) return 'a';
			else return 'c';
		}
		else
		{
			if (b > c) return 'b';
			else return 'c';
		}
	}
	char Opp(char name)
	{
		switch (name)
		{
		case 'A': return 'a';
		case 'B': return 'b';
		case 'C': return 'c';

		case 'a': return 'A';
		case 'b': return 'B';
		case 'c': return 'C';

		default: return '?';
		}
	}
	char Adj(char angleName)
	{
		switch (angleName)
		{
		case 'A': return (Hyp()=='b' ? 'c' : 'b');
		case 'B': return (Hyp()=='a' ? 'c' : 'a');
		case 'C': return (Hyp()=='b' ? 'a' : 'b');

		case 'a': return (Hyp()=='b' ? 'C' : 'B');
		case 'b': return (Hyp()=='a' ? 'C' : 'A');
		case 'c': return (Hyp()=='b' ? 'A' : 'B');

		default: return '?';
		}
	}
};

int mmain()
{
START:
	std::cout << "Mode\n-Polynomials (0)\n-Trig functions (1)\n";
	int mode;
	std::cin >> mode;

	switch (mode)
	{
	default:
		std::cout << "Invalid\n";
		goto START;
		break;
	}

	std::cout << Equ{ Expr{ { Term{ 1.0, { { 'y', 1.0 } } } } }, Expr{ { Term{ 1.0, { { 'm', 1.0 }, { 'x', 2.0 } } }, Term{ 1.0, { { 'b', 1.0 } } } } } };
}