void main()
{
    int a;
    int b;
    a = 1;
    b = 2;
    if (a >= b)
    {
        a = 3;
    } else {
        b = 4;
    }

    if (a <= b)
        a = 5;
    else
        b = 6;

    if (a > b)
    {
        a = 7;
    } else if (a == b) {
        a = 8;
    } else if (a <= b) {
        a = 9;
    } else {
        a = 10;
    }
    return a;
}
