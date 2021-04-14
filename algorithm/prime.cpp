#include"cstdio"
#include"cstring"
using namespace std;
#define MAX 55300380//求MAX范围内的素数
long long su[MAX],cnt;
bool isprime[MAX];
void prime()
{
    cnt=1;
    memset(isprime,1,sizeof(isprime));//初始化认为所有数都为素数
    isprime[0]=isprime[1]=0;//0和1不是素数
    for(long long i=2;i<=MAX;i++)
    {
        if(isprime[i])
            su[cnt++]=i;//保存素数i
        for(long long j=1;j<cnt&&su[j]*i<MAX;j++)
        {
            isprime[su[j]*i]=0;//筛掉小于等于i的素数和i的积构成的合数
        }
    }
}
int main()
{
    prime();
    long long result = 0;
    int64_t hao = 0;
    for(long long i=1;i<cnt;i++) {
        if (i%10000 == 0) {
            printf("%lld\n", i);
        }
        ++hao;
        result = su[i];
    }
    printf("result_%lld, cnt_%lld\n", result, hao);
    return 0;
}

