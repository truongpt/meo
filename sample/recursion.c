int fibonacci(int n)
{
    if (n < 2) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

int main()
{
    int k = 15;
    int i = fibonacci(k);
    printf("fibonacci order %d = %d\n",k,i);
    return 1;
}
