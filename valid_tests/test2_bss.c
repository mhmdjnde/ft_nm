int g_uninit;
static int s_uninit;

void bump(void)
{
    g_uninit++;
    s_uninit++;
}
