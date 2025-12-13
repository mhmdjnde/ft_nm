int normal_var = 1;
int weak_var __attribute__((weak));

void use_weak(void)
{
    if (&weak_var)
        normal_var++;
}
