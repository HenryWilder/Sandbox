#pragma once
#define GRAPHICS_DEFINES 1

#define UNSIGNED 1
#if UNSIGNED
typedef unsigned char Val_t;
#else
typedef char Val_t;
#endif

#define BIGRAMP 1
#if BIGRAMP
#define RAMP {(Val_t)' ',(Val_t)'.',(Val_t)'\'',(Val_t)'`',(Val_t)'^',(Val_t)'\"',(Val_t)',',(Val_t)':',(Val_t)';',(Val_t)'I',(Val_t)'l',(Val_t)'!',(Val_t)'i',(Val_t)'>',(Val_t)'<',(Val_t)'~',(Val_t)'+',(Val_t)'_',(Val_t)'-',(Val_t)'?',(Val_t)']',(Val_t)'[',(Val_t)'}',(Val_t)'{',(Val_t)'1',(Val_t)')',(Val_t)'(',(Val_t)'|',(Val_t)'\\',(Val_t)'/',(Val_t)'t',(Val_t)'f',(Val_t)'j',(Val_t)'r',(Val_t)'x',(Val_t)'n',(Val_t)'u',(Val_t)'v',(Val_t)'c',(Val_t)'z',(Val_t)'X',(Val_t)'Y',(Val_t)'U',(Val_t)'J',(Val_t)'C',(Val_t)'L',(Val_t)'Q',(Val_t)'0',(Val_t)'O',(Val_t)'Z',(Val_t)'m',(Val_t)'w',(Val_t)'q',(Val_t)'p',(Val_t)'d',(Val_t)'b',(Val_t)'k',(Val_t)'h',(Val_t)'a',(Val_t)'o',(Val_t)'*',(Val_t)'#',(Val_t)'M',(Val_t)'W',(Val_t)'&',(Val_t)'8',(Val_t)'%',(Val_t)'B',(Val_t)'@',(Val_t)'$'}
#else
#define RAMP {(Val_t)' ',(Val_t)'.',(Val_t)':',(Val_t)'-',(Val_t)'=',(Val_t)'+',(Val_t)'*',(Val_t)'#',(Val_t)'%',(Val_t)'@'}
#endif