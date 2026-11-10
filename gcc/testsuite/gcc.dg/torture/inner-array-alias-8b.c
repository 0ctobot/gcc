/* { dg-do compile } */

struct s { int a; int arr[1]; int b; int flex[]; };
struct s x = { 1, { 2 }, 3, { 4, 5, 6 } };
