int main()
{
    int pre = 1;
    int cur = 1;
    int n = 10;

    for (int i = 2; i < n; i=i+1) {
        int t = cur;
        cur = cur + pre;
        pre =  t;
    }

    printf("So fibonacci in order %d is : %d\n",n,cur);

    return 0;
}
