struct Point
{
    int x;
    int y;
};

struct Point origin = {0, 0};

struct Point make_point(int x, int y)
{
    struct Point p;

    p.x = x;
    p.y = y;
    return p;
}
