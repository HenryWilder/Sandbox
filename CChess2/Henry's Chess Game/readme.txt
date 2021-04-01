Hi!
This is my second program ever, made with C++ using Visual Studio 2019 Comunity as an IDE!

CChess2 (the second iteration of my Console Chess project) is a chess game which runs in the console. It uses the windows.h header for software rendering and mouse input.

If the game seems to act weird upon loading (i.e. clicking causes parts of the board to turn black and stop rendering), you may have to press a key to get the console's attention. Please note that the slow rendering is not an artistic choice, but rather a limitation of the rendering method. Windows.h has a "SetPixel()" function which is very handy, but it draws the entire frame every time it is called, meaning each pixel-draw takes approximately a frame to complete. To combat this, about 98.5% of the game isn't actually drawn at any given time. As a side-effect, anything which changes the color of pixels (for example, selecting part of the console as if it were displaying text) causes them to stop being displayed until redrawn (you can cause a redraw update on any space simply by hovering the mouse over it, making the game think you are attempting to select a game piece and displaying the selection color on the background).

The program will most likely not run on any OS other than Windows. If you are using Windows, however, it is a very lightweight program with no external dependencies other than the Windows operating system itself! Even the game sprites are stored on the stack as const char* arrays whose colors are interpreted by the render function!

Please note that there is currently no AI opponent for the game. You can either play against yourself or have someone else play as the opposite team.

Controls:
- Left-click a piece with the mouse to select it, then select one of the legal moves (shown as green spaces, or red spaces if the move will capture one of the opponent's pieces) to move the piece.
- Selecting a different piece on your team while you have one already selected will automatically deselect your current piece and select the new piece instead.
- When you are ready to quit the game, simply close the window.

Known bugs:
- Selecting outside of the game board--specifically below it--causes the program to crash. I'm not entirely sure what causes this, but my best guess is that tries calling the function to display all the moves made in the game before the "history" memory has been initialized.
- As stated above, the program will tend to start up without recieving mouse input, requiring keyboard input to ready it for play.
- While it does recognise en passant as a legal move, and making that move takes the pawn as it should, the move does not display as a (red) capturing move when making the selection.

I hope you enjoy it!
--Henry