int a;
int main()
{
    int b;
    b = 1;
    printf("local-1 b: %d\n",b);
    {
        int c;
        c = 2;
        printf("local-2 c: %d\n",c);
        {
            c = c+1;
            printf("local-3 c: %d\n",c);
        }

        {
            {
                c = c+1;
                printf("local-4 c: %d\n",c);

            }
        }

    }
    {
        a = a+1;
        printf("local-5 a: %d\n",a);

    }

}
