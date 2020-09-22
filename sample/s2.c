int f(int a)
{

    return a+1;
}
int main()
{
    int a = f(1);
    int b = 2 + f(a+1);
    printf("f(1) %d\n",b);
}
