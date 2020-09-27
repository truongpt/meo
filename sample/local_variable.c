int main()
{
    int b;
    b = 1;
    {
        printf("b1 = %d\n",b);
        int b = 0;
        b = b+2;
        printf("b2 = %d\n",b);
        {
            b = b+2;
            printf("b3 = %d\n",b);
            int b = 10;
            printf("b4 = %d\n",b);
        }
    }
    printf("b5 = %d\n",b);
}
