static int table[3] = {1, 2, 3};

int get_table(int i)
{
    if (i < 0 || i >= 3)
        return -1;
    return table[i];
}
