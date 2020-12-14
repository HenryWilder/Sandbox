

/*
Heya, Amity, this is a introduction to object oriented design (OOD).

Object oriented programing is a model where you create self contained
pieces that fit together. Much of the code is not accessible from
things ouside the object with the intent to reduce the amount of
code you must reason about in a period of time.

New Keywords:
"class" and "struct" both declare an object with a given name. The
only main difference between the two is that struct has it's member
defaulted to public where as class they are defaulted to private. In
this example I've elected to use "class" exclusivly.

"private:" declared inside a struct or class definition to mark all
subsequent methods not accessable from outside the struct or class
definition.

"private:" declared inside a struct or class definiont to mark all
subsequent methods as accessible from outside the struct or class.

Aside Note: I am generally not a fan of OOD because it tends to become
a mess over time and hits a performance wall which does not
make it ideal for some use cases. If you intend to have no more
than a dozens objects in total, OO is extremely useful to extend
functionality and provide a good framework. Once you get thousands
to millions of objects performance tends to start becoming a problem.

All that being said, OOD is very good for a subset of complexity problems
and all programers must know how it works.
*/

#include <iostream>
#include "Board.h"

#define ClearScreen system("CLS")

bool PlayAgainPrompt()
{
    // @TODO Prompt to play again?
    std::cout << "Want to play again? (y/n)\n";
    char input = 'y';
    std::cin >> input;
    return (input == 'y');
}


int main(void)
{
    std::cout << "\x1b]2;Chess\x07";
    Board b;

    bool checkMate = false;
    bool playAgain = false;
    do
    {
        do
        {
            ClearScreen;
            b.ResetBoard(8, 8);

            b.PrintBoard();

            b.PlayBoard();

        } while (!checkMate);

        playAgain = PlayAgainPrompt();
    } while (playAgain);

    ClearScreen;
    int input;
    std::cin >> input;


    return 0;
}