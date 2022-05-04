#include<iostream>
#include <windows.h>
#include<pthread.h>
#include <stdlib.h>
#include<semaphore.h>
using namespace std;
float A[2000][2000],B[2000][2000],C[2000][2000],D[2000][2000];
int n;
typedef struct{
    int k;
    int t_id;
}threadParam_t;
void *threadFunc(void* param){//动态线程版本
    threadParam_t *p=(threadParam_t*)param;
    int k=p->k; //消去轮次
    int t_id=p->t_id; //线程编号
    int i=k+t_id+1; //自己的计算任务

    float tmp2=B[i][k];
    for(int j=k+1;j<n;j++)
        B[i][j]-=tmp2*B[k][j];
    B[i][k]=0;
    pthread_exit(NULL);
}
typedef struct{
    int t_id;
}threadParam_t2;
const int num_threads=7;
sem_t sem_leader;
sem_t sem_Division[num_threads-1];
sem_t sem_Elimination[num_threads-1];
void *threadFunc2(void* param){//循环划分
    threadParam_t2 *p=(threadParam_t2*)param;
    int t_id=p->t_id; //线程编号
    for(int k=0;k<n;k++)
    {
        if(t_id==0){
            float tmp=A[k][k];
            for(int j=k;j<n;j++)
                A[k][j]/=tmp;

        }
        else{
            sem_wait(&sem_Division[t_id-1]);
        }
		if(t_id==0){
            for(int i=0;i<num_threads-1;i++)
                sem_post(&sem_Division[i]);
		}
		for(int i=k+1+t_id;i<n;i+=num_threads)
		{
			float tmp2=A[i][k];
			for(int j=k+1;j<n;j++){
				A[i][j]-=tmp2*A[k][j];}
			A[i][k]=0;
		}
		if(t_id==0){
            for(int i=0;i<num_threads-1;i++)
                sem_wait(&sem_leader);
            for(int i=0;i<num_threads-1;i++)
                sem_post(&sem_Elimination[i]);
        }
        else{
            sem_post(&sem_leader);
            sem_wait(&sem_Elimination[t_id-1]);
        }
	}
	pthread_exit(NULL);

}

void *threadFunc3(void* param){//块划分
    threadParam_t2 *p=(threadParam_t2*)param;
    int t_id=p->t_id; //线程编号
    for(int k=0;k<n;k++)
    {
        if(t_id==0){
            float tmp=B[k][k];
            for(int j=k;j<n;j++)
                B[k][j]/=tmp;

        }
        else{
            sem_wait(&sem_Division[t_id-1]);
        }
		if(t_id==0){
            for(int i=0;i<num_threads-1;i++)
                sem_post(&sem_Division[i]);
		}
		int block=(n-k-1)%num_threads==0?(n-k-1)/num_threads:(n-k-1)/num_threads+1;
		int start=k+1+t_id*block;
		for(int i=k+1+t_id*block;i<n&&i<block+start;i++)
		{
			float tmp2=B[i][k];
			for(int j=k+1;j<n;j++){
				B[i][j]-=tmp2*B[k][j];}
			B[i][k]=0;
		}
		if(t_id==0){
            for(int i=0;i<num_threads-1;i++)
                sem_wait(&sem_leader);
            for(int i=0;i<num_threads-1;i++)
                sem_post(&sem_Elimination[i]);
        }
        else{
            sem_post(&sem_leader);
            sem_wait(&sem_Elimination[t_id-1]);
        }
	}
	pthread_exit(NULL);
}
void Initialize(int n)//初始化
{
	int i,j,k;
	for(i=0;i<n;i++)
	{
		for(j=0;j<i;j++){
			A[i][j]=0;//下三角元素初始化为零
			B[i][j]=0;
			C[i][j]=0;
			D[i][j]=0;
		}
		A[i][i]=1.0;//对角线元素初始化为1
		B[i][i]=1.0;
		C[i][i]=1.0;
		D[i][i]=1.0;
		for(j=i+1;j<n;j++){
			A[i][j]=rand();//上三角元素初始化为随机数
			B[i][j]=A[i][j];
			C[i][j]=A[i][j];
			D[i][j]=A[i][j];
		}
	}
	for(k=0;k<n;k++)
		for(i=k+1;i<n;i++)
			for(j=0;j<n;j++){
				A[i][j]+=A[k][j];//最终每一行的值是上一行的值与这一行的值之和
				B[i][j]+=B[k][j];
				C[i][j]+=C[k][j];
				D[i][j]+=D[k][j];
			}
}
void Guass_para_circle(){
	int i;
	sem_init(&sem_leader,0,0);
	for(i=0;i<num_threads-1;i++){
        sem_init(&sem_Division[i],0,0);
        sem_init(&sem_Elimination[i],0,0);
	}
	pthread_t *handles=new pthread_t[num_threads];//创建对应的handle
    threadParam_t2 *param=new threadParam_t2[num_threads];//创建对应的线程数据结构
    for(int t_id=0;t_id<num_threads;t_id++){
        param[t_id].t_id=t_id;
    }
    for(int t_id=0;t_id<num_threads;t_id++){
        pthread_create(&handles[t_id],NULL,threadFunc2,&param[t_id]);
    }
    for(int t_id=0;t_id<num_threads;t_id++){
        pthread_join(handles[t_id],NULL);
    }
	sem_destroy(sem_Division);
	sem_destroy(sem_Elimination);
	sem_destroy(&sem_leader);
}
void Gauss_para_blocks(){
    int i;
	sem_init(&sem_leader,0,0);
	for(i=0;i<num_threads-1;i++){
        sem_init(&sem_Division[i],0,0);
        sem_init(&sem_Elimination[i],0,0);
	}
	pthread_t *handles=new pthread_t[num_threads];//创建对应的handle
    threadParam_t2 *param=new threadParam_t2[num_threads];//创建对应的线程数据结构
    for(int t_id=0;t_id<num_threads;t_id++){
        param[t_id].t_id=t_id;
    }
    for(int t_id=0;t_id<num_threads;t_id++){
        pthread_create(&handles[t_id],NULL,threadFunc3,&param[t_id]);
    }
    for(int t_id=0;t_id<num_threads;t_id++){
        pthread_join(handles[t_id],NULL);
    }
	sem_destroy(sem_Division);
	sem_destroy(sem_Elimination);
	sem_destroy(&sem_leader);
}
void Print(int n,float m[][2000]){//打印结果
	int i,j;
	for(i=0;i<n;i++){
		for(j=0;j<n;j++)
			cout<<m[i][j]<<" ";
		cout<<endl;
	}
}
int main(){
	int N,count,cycle=1,step=10;
	N=2000;
	LARGE_INTEGER t1,t2,tc1,t3,t4,tc2;
	Initialize(2000);
	for(n=2000;n<=N;n+=step){
		count=1;
		if(n<=50)
            cycle=1000;
        else
            if(n<=70)
                cycle=100;
            else
                if(n<=400)
                    cycle=10;
                    else
                        cycle=1;

		QueryPerformanceFrequency(&tc1);
		QueryPerformanceCounter(&t1);
		while(count<=cycle){
			Guass_para_circle();
            count++;}
        QueryPerformanceCounter(&t2);
        cout<<n<<" "<<count-1<<" "<<((t2.QuadPart - t1.QuadPart)*1000.0 / tc1.QuadPart);


        count=1;
		QueryPerformanceFrequency(&tc2);
		QueryPerformanceCounter(&t3);
		while(count<=cycle){
			Gauss_para_blocks();
            count++;}
        QueryPerformanceCounter(&t4);
        cout<<" "<<((t4.QuadPart - t3.QuadPart)*1000.0 / tc2.QuadPart)<<endl;
		if(n==100)
            step=100;
        if(n==1000)
            step=500;

	}
}
