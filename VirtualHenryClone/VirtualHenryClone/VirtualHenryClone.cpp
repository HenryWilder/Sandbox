#include <iostream>
#include <string>

int main()
{
    while (true)
    {
        std::cout << "User: ";
        std::string input;
        std::string output = "hm?";
        std::cin >> input;
        std::cout << "\n";
        // Process
        {
            // Tiny
            {
                printf("Tiny: ");
                if (input.size() == 1)
                {
                    output = input[0] + 1;
                    goto FINISH;
                }
                printf("false\n");
            }
            // Homogenous
            // aaaaaaa, bbbbbbb, AAAAAAAAA, etc
            {
                printf("Homogenous: ");
                bool b_broken = false;
                for (char c : input)
                {
                    if (c != input[0]) {
                        b_broken = true;
                        break;
                    }
                }
                if (!b_broken)
                {
                    output = input;
                    goto FINISH;
                }
                printf("false\n");
            }
        }
    FINISH:
        printf("true\n");
        printf("\nComp:: %s\n", output.c_str());
    }
}