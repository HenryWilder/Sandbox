#include <raylib.h>
#include "Logic.h"
#include <bitset>
#include <iostream>
#include <string>

int main()
{

	0b0100111001101001011000110110010100100001;

	return 0;

	enum class Token : uint64_t {
		Apple = ('a' | ('p' << 010) | ('p' << 020) | ('l' << 030) | ('e' << 040)),
	};
	std::string input;
	std::cin >> input;
	uint64_t token = *reinterpret_cast<const uint64_t*>(input.c_str());
	printf("\n%s\n", (char*)&token);

	return 0;

	auto BoolStr = [](bool b) {
		return (b ? "True" : "False");
	};
	auto Evaluate = [&](Gate::Mode method, std::vector<bool> in) {
		return BoolStr(Gate::Eval(method, in));
	};
	auto EvaluateEach = [&](std::vector<bool> in) {
		printf("AND: %s\nOR:  %s\nNOR: %s\nXOR: %s\n", Evaluate(Gate::Mode::AND, in), Evaluate(Gate::Mode::OR, in), Evaluate(Gate::Mode::NOR, in), Evaluate(Gate::Mode::XOR, in));
	};
	auto PrintArray = [&](std::vector<bool> in) {
		printf("{ ");
		for (bool b : in) {
			printf("%s, ", BoolStr(b));
		}
		printf("}\n");
	};

	/*
	std::vector<bool> in{0,0,0,0};
	PrintArray(in);
	EvaluateEach(in);

	in = { 1,0,0,0 };
	PrintArray(in);
	EvaluateEach(in);

	in = { 1,1,1,1 };
	PrintArray(in);
	EvaluateEach(in);

	in = { 0,1,0,0 };
	PrintArray(in);
	EvaluateEach(in);
	*/

	//unsigned long long a = '\0a';
	//unsigned long long ab = '\0ba';
	//unsigned long long abc = '\0cba';
	//printf("%s : %i\n", &a, a);
	//std::cout << std::bitset<sizeof(unsigned long long) * 8>(a) << '\n';
	//printf("%s : %i { %i, %i }\n", &ab, ab, ab >> 010, ab % 256);
	//std::cout << std::bitset<sizeof(unsigned long long) * 8>(ab) << '\n';
	//printf("%s : %i { %i, %i, %i }\n", &abc, abc, abc >> 020, (abc >> 010) % 256, abc % 256);
	//std::cout << std::bitset<sizeof(unsigned long long) * 8>(abc) << '\n';

	//Gate::Mode m = Gate::Mode::AND;
	//for (int i = 0; i < 10; ++i) {
	//	printf("%c%c%c\n", Char(m, 0), Char(m, 1), Char(m, 2));
	//	m = ++m;
	//}
}