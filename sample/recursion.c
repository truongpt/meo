int fibonacci(int n)
{
    if (n < 2) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

int main()
{
    int n = 15;
    int i = fibonacci(n);
    printf("fibonacci order %d = %d\n",n,i);
    return 1;
}
