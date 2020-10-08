int main()
{
    int *a;
    int b = 10;
    a = &b;
    b = 11;
    printf("value a pointer = %d\n",*a);
    return a;
}
