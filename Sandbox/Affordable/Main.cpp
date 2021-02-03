#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <tuple>

struct NameWithPlural
{
	std::string m_singular;
	std::string m_plural;

	std::string& NameForCount(int n)
	{
		if (n == 1) return m_singular;
		else return m_plural;
	}
};

struct ItemsAtPriceRange
{
	std::vector<NameWithPlural> names;

	std::string Together(int n)
	{
		if (!names.empty())
		{
			std::string set = names[0].NameForCount(n);
			for (int i = 0; i < names.size(); ++i)
			{
				set.append("/");
				set.append(names[i].NameForCount(n));
			}
			return set;
		}
		else return "\0";
	}
};
struct PriceRange
{
	int price;
	ItemsAtPriceRange items;
};

static std::unordered_map<int, ItemsAtPriceRange> g_menu = {
	{1, {{{"Fry", "Fries"}, {"UwUFry", "UwUFries"}}}},
	{2, {{{"BigFry", "BigFries"}, {"Drink", "Drinks"}, {"BabyBuger", "BabyBugers"}}}},
	{3, {{{"PremeFry", "PremeFries"}}}},
	{5, {{{"CoolBurger", "CoolBurgers"}, {"SexyBurger", "SexyBurgers"}}}},
};

ItemsAtPriceRange AtPriceRange(int price)
{
	ItemsAtPriceRange items;
	auto it = g_menu.find(price);
	if (it != g_menu.end()) items = it->second;
	return items;
}

using SetOfRanges = std::vector<PriceRange>;

SetOfRanges AllBelowPrice(int maxPrice)
{
	SetOfRanges items;
	for (; maxPrice > 0; --maxPrice)
	{
		items.push_back({ maxPrice, AtPriceRange(maxPrice) });
	}
	return items;
}

void OneCombo(int budget)
{
	SetOfRanges ranges = AllBelowPrice(budget);
	int i = 0;
	for (PriceRange list : ranges)
	{
		if (!list.items.names.empty())
		{
			int maxCombo = budget / list.price;
			printf(" %s %i %s", (!i?"+":"OR"), maxCombo, list.items.Together(maxCombo).c_str());
		}
		i++;
	}
}

int Combo(int budget)
{
	int total = 0;
	SetOfRanges ranges = AllBelowPrice(budget);
	for (PriceRange list : ranges)
	{
		auto [price, itemsAtPrice] = list;
		if (list.items.names.size())
		{
			total++;

			int maxCombo = budget / price;

			printf("-%i %s", maxCombo, list.items.Together(maxCombo).c_str());

			int remainder = budget - (maxCombo * price);
			if (remainder > 0)
			{
				OneCombo(remainder);
			}
			printf("\n");
		}
	}
	return total;
}

int main()
{
	printf("Menu:\n");

	for (auto pricerange : g_menu) {
		for (auto name : pricerange.second.names)
		{
			printf("$%i - %s\n", pricerange.first, name.NameForCount(1).c_str());
		}
	}
	printf("\n");

	int budget;
	std::cout << "Budget: $";
	std::cin >> budget;

	printf("\nYou can afford:\n");
	for (; budget > 0; --budget) {
		printf("For $%i:\n", budget);
		Combo(budget);
		printf("\n");
	}
	int done;
	std::cin >> done;

	return 0;
}