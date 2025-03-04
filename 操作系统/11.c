#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int *list; // 整数列表
int count;
double average; // 线程计算的平均值
int maximum; // 线程计算的最大值
int minimum; // 线程计算的最小值

// 函数声明
void *calculate_average(void *param)
{
    double sum=0;
    for(int i = 0;i<count;i++)
        sum+=list[i];
    average = sum/count;
    return NULL;
    
}
void *calculate_maximum(void *param)
{
    maximum = list[0];
    for(int i = 0;i<count;i++)
    {
        if(list[i]>maximum)
            maximum = list[i];
    }
    return NULL;
}
void *calculate_minimum(void *param)
{
    minimum = list[0];
    for(int i = 0;i<count;i++)
    {
        if(list[i]<minimum)
            minimum = list[i];
    }
    return NULL;
}

int main(int argc, char *argv[]) 
{
    count = argc-1;
    pthread_t calaver,calmax,calmin;
    list = malloc(sizeof(int) * (argc-1));
    for (int i = 0; i < argc-1; i++)
        list[i] = atoi(argv[i + 1]);


    pthread_create(&calaver,NULL,calculate_average,NULL);
    pthread_create(&calmax,NULL,calculate_maximum,NULL);
    pthread_create(&calmin,NULL,calculate_minimum,NULL);

    pthread_join(calaver,NULL);
    pthread_join(calmax,NULL);
    pthread_join(calmin,NULL);

    printf("aver:%d\n",average);
    printf("minimum:%d\n",minimum);
    printf("maximum:%d\n",maximum);

    return 0;
}


  

  