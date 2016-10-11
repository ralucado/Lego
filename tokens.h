#ifndef tokens_h
#define tokens_h
/* tokens.h -- List of labelled tokens and stuff
 *
 * Generated from: lego.g
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * ANTLR Version 1.33MR33
 */
#define zzEOF_TOKEN 1
#define NUM 2
#define GRID 3
#define LP 4
#define RP 5
#define LB 6
#define RB 7
#define COMA 8
#define NORTH 9
#define EAST 10
#define WEST 11
#define SOUTH 12
#define PLACE 13
#define AT 14
#define MOVE 15
#define PUSH 16
#define POP 17
#define WHILE 18
#define FITS 19
#define HEIGHT 20
#define DEF 21
#define GT 22
#define LT 23
#define AND 24
#define ASIG 25
#define ID 26
#define END 27
#define SPACE 28

#ifdef __USE_PROTOS
void lego(AST**_root);
#else
extern void lego();
#endif

#ifdef __USE_PROTOS
void grid(AST**_root);
#else
extern void grid();
#endif

#ifdef __USE_PROTOS
void ops(AST**_root);
#else
extern void ops();
#endif

#ifdef __USE_PROTOS
void fact(AST**_root);
#else
extern void fact();
#endif

#ifdef __USE_PROTOS
void place(AST**_root);
#else
extern void place();
#endif

#ifdef __USE_PROTOS
void pp(AST**_root);
#else
extern void pp();
#endif

#ifdef __USE_PROTOS
void moviment(AST**_root);
#else
extern void moviment();
#endif

#ifdef __USE_PROTOS
void bucle(AST**_root);
#else
extern void bucle();
#endif

#ifdef __USE_PROTOS
void cond(AST**_root);
#else
extern void cond();
#endif

#ifdef __USE_PROTOS
void atom(AST**_root);
#else
extern void atom();
#endif

#ifdef __USE_PROTOS
void comp(AST**_root);
#else
extern void comp();
#endif

#ifdef __USE_PROTOS
void height(AST**_root);
#else
extern void height();
#endif

#ifdef __USE_PROTOS
void fits(AST**_root);
#else
extern void fits();
#endif

#ifdef __USE_PROTOS
void defs(AST**_root);
#else
extern void defs();
#endif

#ifdef __USE_PROTOS
void dir(AST**_root);
#else
extern void dir();
#endif

#ifdef __USE_PROTOS
void reop(AST**_root);
#else
extern void reop();
#endif

#endif
extern SetWordType zzerr1[];
extern SetWordType zzerr2[];
extern SetWordType setwd1[];
extern SetWordType zzerr3[];
extern SetWordType zzerr4[];
extern SetWordType zzerr5[];
extern SetWordType zzerr6[];
extern SetWordType zzerr7[];
extern SetWordType zzerr8[];
extern SetWordType setwd2[];
extern SetWordType zzerr9[];
extern SetWordType zzerr10[];
extern SetWordType zzerr11[];
extern SetWordType setwd3[];
