/* Reading a plain bit-field mustn't load a volatile member next to it.  */
/* { dg-do compile } */
/* { dg-options "-O2" } */

struct s1 { volatile char x; unsigned a:20; char y; };
unsigned f1 (struct s1 *p) { return p->a; }

struct s2 { _Bool a:1; int c:1; volatile signed char e; short f; };
int f2 (struct s2 *p) { return p->c; }

struct s3 { volatile unsigned char v; unsigned a:3; unsigned b:13; };
int f3 (struct s3 *p) { return p->a; }

struct in { volatile int st; };
struct s4 { struct in in; unsigned a:5, b:11; };
int f4 (struct s4 *p) { return p->b; }

/* The volatile member belongs to the enclosing record.  */
struct s5 { volatile unsigned char st;
	    struct { unsigned char a:4; unsigned short b:6; } f; long l; };
int f5 (struct s5 *p) { return p->f.b; }

struct s6 { volatile unsigned int st;
	    struct { unsigned int a:4; unsigned int b:6; }; long l; };
int f6 (struct s6 *p) { return p->b; }

/* { dg-final { scan-assembler-not {\tldr\t[wx][0-9]+, \[x0\]} } } */
/* { dg-final { scan-assembler-times {\tldrb\tw[0-9]+, \[x0\]\n} 1 } } */
/* { dg-final { scan-assembler-times {\tldrh\tw[0-9]+, \[x0, 1\]\n} 1 } } */
/* { dg-final { scan-assembler-times {\tldrh\tw[0-9]+, \[x0, 2\]\n} 1 } } */
/* { dg-final { scan-assembler-times {\tldrh\tw[0-9]+, \[x0, 4\]\n} 2 } } */
