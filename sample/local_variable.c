int a;
int main()
{
    int a;
    a = 1;
    printf("local-1 a: %d\n",a);
    {
        int a;
        a = 2;
        printf("local-2 a: %d\n",a);
        {
            a = 3;
            int b;
        }
        printf("local-2 a: %d\n",a);
        {
            int b = 2;
        }
    }
}
