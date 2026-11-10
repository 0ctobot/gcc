/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-vrp -fdump-tree-dom-details" } */



static inline long load(long *p)
{
        long ret;
        /* The asm has to cost more than the load of *p for DOM to replace
           the load with ret, so its template holds two instructions.  */
        asm ("xyzzy      %1,%0\n\tnop\n\t" : "=r" (ret) : "m" (*p));
        if (ret != *p)
                __builtin_unreachable();
        return ret;
}

long foo(long *mem)
{
        long ret;
        ret = load(mem);
        return ret + *mem;
}

/* { dg-final { scan-tree-dump "Folded to: _\[0-9\]+ = _\[0-9\]+ \\* 2"  "dom2" } } */
