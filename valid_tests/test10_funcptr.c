int target(int x)
{
    return x * 3;
}

int (*fp)(int) = target;

int call_fp(int v)
{
    return fp(v);
}
