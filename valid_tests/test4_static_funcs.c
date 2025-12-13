static int helper(int x)
{
    return x * 2;
}

int public_api(int x)
{
    return helper(x) + 1;
}
