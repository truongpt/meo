int main()
{
    int b;
    b = 1;
    {
        printf("b1 = %d\n",b);
        int b = 0;
        b = b+2;
        printf("b2 = %d\n",b);
    }
    printf("b3 = %d\n",b);
}
