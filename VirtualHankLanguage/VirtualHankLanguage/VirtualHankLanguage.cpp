#include <iostream>
#include <ctime>
#include "Functions.h"
#include "Interpreter.h"
#include "GUI.h"

const char* versionNumber = R"TXT(Version 0.00.1)TXT";

const char* splashText =
R"TXT(
   _    _      _ _         __          __        _     _ _   
  | |  | |    | | |        \ \        / /       | |   | | |  
  | |__| | ___| | | ___     \ \  /\  / /__  _ __| | __| | |  
  |  __  |/ _ \ | |/ _ \     \ \/  \/ / _ \| '__| |/ _` | |  
  | |  | |  __/ | | (_) |     \  /\  / (_) | |  | | (_| |_|  
  |_|  |_|\___|_|_|\___( )     \/  \/ \___/|_|  |_|\__,_(_)  
                       |/                                    
)TXT";

const char* screen1 =
R"TXT(Hi! My name is VirtualHank! I'm here to help you with tech issues. :)
Unfortunately, because I am only a program, I have some trouble with certain things you may find trivial.
For this reason, I ask that you kindly keep my window open, so that I communicate to you when I need help.
I would also like to let you know ahead of time: I may perform hardware simulation.
I will try my best not to take away control from you. I hope you are able to trust me.
It will make it easier for me to help you if you can trust me to perform my programming.
In case I do scare you at any point, or you feel you need to force-quit me but can't press my close button,
I am programmed to return full control to you if you slam your cursor into any edge/corner of the screen.
Once you've finished reading, press enter and I will begin.
Do not worry. The first step will not have any scary mouse-movement whatsoever. And you have have the option
to put me into manual mode.)TXT";

const char* screen2 =
R"TXT(For security reasons, I need you to sign in yourself.
I could have been coded to access your sign-in data on my own using WTSQueryUserToken(),
but that would have risked a security leak if not used properly. Letting you sign in is much safer.
It would also be greatly appriciated if you would please maximize the Internet Explorer window.
My programming is rather rigid, and I have trouble seeing. I can perform the actions needed to do this for you, but
to know where the buttons to click are, I need IE to be maximized. Otherwise I may accidentally click the wrong spot.
Let me know when you're ready to proceed!
If you'd like to do this manually, type "manual" before pressing enter, and I'll tell you the steps :))TXT";

const char* manualMode =
R"TXT(STEPS FOR SYNCING SHAREPOINT
============================
0. In the bottom-left corner of your Internet Explorer window, you should see a button/link that says
   "Return to classic SharePoint"
   Click it.
1. Once the page finishes loading, click on the "LIBRARY" tab/button in the top-left of the window.
   It will be in the light-gray bar, and to the right of the "FILES" tab/button.
2. This will unroll an actions panel. On that panel, there will be a "Sync" button. Click it.
3. A message box will appear in the center of your screen. Press "Allow".
   The message box is simply making sure you want to sync SharePoint.
If you've completed all these steps, SharePoint should be syncing!)TXT";

const char* explainAllow =
R"TXT(So you know, I have the ability to click Allow right now.
I care about making sure you don't feel as though I'm taking away control from you, though.
If you want to sync SharePoint, you may press "Allow" yourself, now.)TXT";

void PrintRaw(const char* string) { std::cout << string << std::endl; }

constexpr POINT center = { 960, 540 };
constexpr POINT returnToClassic = { 90, 1013 };
constexpr POINT library = { 150, 150 };
constexpr POINT sync = { 600, 200 };
constexpr POINT allow = { 1000, 570 };

POINT operator+(const POINT& p1, const POINT& p2)
{
    return { p1.x + p2.x, p1.y + p2.y };
}

// Credit: https://blog.kazitor.com/2014/12/portal-ascii/
const std::string g_AsciiGraphics[]{
R"TXT(             .,-:;//;:=,
         . :H@@@MM@M#H/.,+$;,
      ,/X+ +M@@M@MM$=,-$HMMM@X/,
     -+@MM; $M@@MH+-,;XMMMM@MMMM@+-
    ;@M@@M- XM@X;. -+XXXXXHHH@M@M#@/.
  ,$MM@@MH ,@$=            .---=-=:=,.
  -@#@@@MX .,              -$HX$$$$$+;
 =-./@M@M$                  .;@MMMM@MM:
 X@/ -$MM/                    .+MM@@@M$
,@M@H: :@:                    . -X#@@@@-
,@@@MMX, .                    /H- ;@M@M=
.H@@@@M@+,                    $MM+..$#$.
 /MMMM@MMH/.                  XM@MH; -;
  /$+$$XHH@$=              , .H@@@@MX,
   .=--------.           -$H.,@@@@@MX,
   .$MM@@@HHHXX$$$$+- .:$MMX -M@@MM$.
     =XMMM@MM@MM#H;,-+HMM@M+ /MMMX=
       =$@M@M#@$-.=$@MM@@@M; $M$=
         ,:+$+-,/H#MMMMMMM@- -,
               =++$$$$+/:-.)TXT",

R"TXT(             =+$HM####@H$;,
          /H###############M$,
          ,@################+
           .H##############+
             X############/
              $##########/
               $########/
                /X/;;+X/

                 -XHHX-
                ,######,
#############X  .M####M.  X#############
##############-   -//-   -##############
X##############$,      ,+##############X
-##############X        X##############-
 $############$          $############$
  $##########;            ;##########$
   ;#######M=              =M#######;
    .+M###@,                ,@###M+.
       :XH.                  .HX:)TXT",

R"TXT(                 =/;;/-
                +:    //
               /;      /;
              -X        H.
.//;;;:;;-,   X=        :+   .-;:=;:;$;.
M-       ,=;;;#:,      ,:#;;:=,       ,@
:$           :$.=/++++/=.$=           $=
 ,$;         $/:+/;,,/++:+/         ;+.
   ,+/.    ,;@+,        ,$H;,    ,/+,
      ;+;;/= @.  .H##X   -X :///+;
      ;+=;;;.@,  .XM@$.  =X.//;=$/.
   ,;:      :@$=        =$H:     .+$-
 ,$=         $;-///==///-//         =$,
;+           :$-;;;;;;;;-X-           +:
@-      .-;;;;M-        =M/;;;-.      -X
 :;;::;;-.    $-        :+    ,-;;-;:==
              ,X        H.
               ;/      $=
                //    +;
                 ,////,)TXT",

R"TXT(                          .,---.
                        ,/XM#MMMX;,
                      -$##########M$,
                     -@######$  $###@=
      .,--,         -H#######$   $###M:
   ,;$M###MMX;     .;##########$;HM###X=
,/@###########H=      ;################+
-+#############M/,      $##############+
$M###############=      /##############:
H################      .M#############;.
@###############M      ,@###########M:.
X################,      -$=X#######@:
/@##################$-     +######$-
.;##################X     .X#####+,
 .;H################/     -X####+.
   ,;X##############,       .MM/
      ,:+$H@M#######M#$-    .$$=
           .,-=;+$@###X:    ;/=.
                  .,/X$;   .::,
                      .,    ..)TXT",

R"TXT(                     -$-
                    .H##H,
                   +######+
                .+#########H.
              -$############@.
            =H###############@  -X:
          .$##################:  @#@-
     ,;  .M###################;  H###;
   ;@#:  @###################@  ,#####:
 -M###.  M#################@.  ;######H
 M####-  +###############$   =@#######X
 H####$   -M###########+   :#########M,
  /####X-   =########$   :M########@/.
    ,;$H@X;   .$###X   :##MM@$+;:-
                 ..
  -/;:-,.              ,,-==+M########H
 -##################@HX$$+$$$$$$+:,,
    .-/H$$$+$$$H@###############M@+=:/+:
/XHX$:#####MH$=    ,---:;;;;/&&XHM,:###$
$@#MX $+;-                      )TXT",

R"TXT(                                     :X-
                                  :X###
                                ;@####@
                              ;M######X
                            -@########$
                          .$##########@
                         =M############-
                        +##############$
                      .H############$=.
         ,/:         ,M##########M;.
      -+@###;       =##########M;
   =$M#######;     :#########M/
-$M###########;   :########/
 ,;X###########; =#######$.
     ;H#########+######M=
       ,+#############+
          /M########@-
            ;M#####$
              +####:
               ,$M-)TXT",

    R"TXT(            .+
             /M;
              H#@:              ;,
              -###H-          -@/
               $####$.  -;  .$#X
                M#####+;#H :M#M.
..          .+/;$#############-
 -/$H$+;-,    +##############/
    .:$M###MH$$+############X  ,--=;-
        -/H#####################H+=.
           .+#################X.
         =$M####################H;.
            /@###############+;;/$$;,
         -$###################$
       ;H######################M=
    ,$#####MH$$;+#####M###-/@####$
  :$H$+;=-      -####X.,H#   -+M##@-
 .              ,###;    ;      =$##+
                .#H,               :XH,
                 +                   .;-)TXT",

    R"TXT(           .-;+$XHHHHHHX$+;-.
        ,;X@@X$/;=----=:/$X@@X/,
      =$@@$=.              .=+H@X:
    -XMX:                      =XMX=
   /@@:                          =H@+
  $@X,                            .$@$
 +@X.                               $@$
-@@,                                .@@=
$@$                                  +@$
H@:                                  :@H
H@:         :HHHHHHHHHHHHHHHHHHX,    =@H
$@$         ;@M@@@@@@@@@@@@@@@@@H-   +@$
=@@,        :@@@@@@@@@@@@@@@@@@@@@= .@@:
 +@X        :@@@@@@@@@@@@@@@M@@@@@@:$@$
  $@$,      ;@@@@@@@@@@@@@@@@@M@@@@@@$.
   +@@HHHHHHH@@@@@@@@@@@@@@@@@@@@@@@+
    =X@@@@@@@@@@@@@@@@@@@@@@@@@@@@X=
      :$@@@@@@@@@@@@@@@@@@@M@@@@$:
        ,;$@@@@@@@@@@@@@@@@@@X/-
           .-;+$XXHHHHHX$+;-.)TXT",

    R"TXT(            ,:/+/-
            /M/              .,-=;//;-
       .:/= ;MH/,    ,=/+$$XH@MM#@:
      -$##@+$###@H@MMM#######H:.    -/H#
 .,H@H@ X######@ -H#####@+-     -+H###@X
  .,@##H;      +XM##M/,     =$@###@X;-
X$-  :M##########$.    .:$M###@$:
M##H,   +H@@@$/-.  ,;$M###@$,          -
M####M=,,---,.-$$H####M$:          ,+@##
@##################@/.         :$H##@$-
M###############H,         ;HM##M$=
#################.    .=$M##M$=
################H..;XM##M$=          .:+
M###################@$=           =+@MH$
@#################M/.         =+H#X$=
=+M###############M,      ,/X#H+:,
  .;XM###########H=   ,/X#H+:;
     .=+HM#######M+/+HM@+=.
         ,:/$XM####H/.
              ,.:=-.)TXT",

    R"TXT(       #+ @      # #              M#@
 .    .X  X.$##@;# #   +@#######X. @H$
   ,==.   ,######M+  -#####$M####M-    #
  :H##M$:=##+ .M##M,;#####/+#######$ ,M#
 .M########=  =@#@.=#####M=M#######=  X#
 :@@MMM##M.  -##M.,#######M#######. =  M
             @##..###:.    .H####. @@ X,
   ############: ###,/####;  /##= @#. M
           ,M## ;##,@#M;/M#M  @# X#$ X#
.$=   ######M## ##.M#:   ./#M ,M #M ,#$
##/         $## #+;#: #### ;#/ M M- @# :
#+ #M@MM###M-;M #:$#-##$H# .#X @ + $#. #
      ######/.: #$=# M#:MM./#.-#  @#: H#
+,.=   @###: /@ $#,@  ##@X #,-#@.##$ .@#
#####+;/##/ @##  @#,+       /#M    . X,
   ;###M#@ M###H .#M-     ,##M  ;@@; ###
   .M#M##H ;####X ,@#######M/ -M###$  -H
    .M###$  X####H  .@@MM@;  ;@#M@
      H#M    /@####/      ,++.  / ==-,
               ,=/:, .+X@MMH@#H  #####$=)TXT",
};

int main()
{
    {
        int sessionNumber;
        {
            std::ifstream log("log.txt", std::ifstream::in);
            log.ignore(64, '.');
            log >> sessionNumber;

            log.close();
        }
        {
            std::ofstream log("log.txt", std::ofstream::out);


            log.clear();
            time_t now = time(0);
            char dt[28];
            ctime_s(dt, sizeof(dt), &now);

            log << "Session No. " << sessionNumber + 1 << " | " << dt << std::endl;

            log.close();
        }
    }

    DrawOverlay();

    SayDebug("Program started.", DebugColor::Msg);

    SayComm("Hi there! :D");
    SayComm("This is a really long line. I'm making this just to test how the GUI functions handle string overflow. I wrote the code so it can recursively wrap the text, prioritizing splitting at a space followed by breaking up words. It seems to be working pretty well :3");
    SayComm("At this point I'm kinda just tryina fill up the comm with text to make sure it acts correctly when filled. It should, hopefully, start producing a sort of \"scrolling\" effect as new lines are added below the existing ones, until the top messages are gone.");
    SayComm("Wow this has a lot more space than I thought");
    SayComm("Aaaa");
    SayComm("Words");
    SayComm("Words");
    SayComm("Words");
    SayComm("More words");
    SayComm("Even more words");
    SayComm("eeeeeeeee");
    SayComm("EEEEEEEEEEE");
    SayComm("AAAAAAAAA");

    SayPretty(g_AsciiGraphics[8]);

    InterpretFile("Test.vhid");

    SetPrintPos(0, 31);

    return 0;

    InterpretFile("Test.vhid");
    return 0;

    PrintRaw(versionNumber);
    PrintRaw(splashText);
    PrintRaw(screen1);

    PromptUser("ready", "to continue");

    PrintRaw(screen2);

    OpenExplorer();

    if (!AskUser("Would you like to run the program in manual mode?"))
    {
        POINT screenOffset = { 0,0 };
        SimulateMouseGoto(center + screenOffset);

        if (!AskUser("I've moved your mouse. Is your cursor on the same screen Intenet Explorer is open on?"))
        {
            screenOffset.x += 1920;
            SimulateMouseGoto(center + screenOffset);

            if (!AskUser("How about now?"))
            {
                std::cout << "You have so many monitors...";
                return 0;
            }
        }

        SimulateMouseClickAt(screenOffset + returnToClassic);
        Sleep(2000); // IE is slow
        SimulateMouseClickAt(screenOffset + library);
        Sleep(800);
        SimulateMouseClickAt(screenOffset + sync);
        Sleep(400);
        SimulateMouseGoto(screenOffset + allow);
        PrintRaw(explainAllow);

    }
    else PrintRaw(manualMode);

    std::cout << "\nMy work here is done.\n";

    PromptUser("quit", "to close the program");

    return 0;
}
