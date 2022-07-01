#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include<string.h>
#include<semaphore.h>

sem_t wrt;
pthread_mutex_t lockingUDZ;

int document=1;
int countReader=0;


void *readerFunction(void *readerid)//making readers here
{	
	pthread_mutex_lock(&lockingUDZ);//reader locking mutex because it will modify shared countreaderdata
	countReader=countReader+1;//number of reader
	if(countReader==1)
	{//if it is first reader then blocking writer
		sem_wait(&wrt);
	}
	
	pthread_mutex_unlock(&lockingUDZ);//unlocking mutex then new readers access this part
	
	printf("Reader %d reading document number: %d from database\n",*((int *)readerid),document);
	
	pthread_mutex_lock(&lockingUDZ);//reader locking here again because it read the document then reader will decrease own number and here shared data too so we locked
	
	countReader=countReader-1;
	
	if(countReader==0)
	{
		sem_post(&wrt);//there is last reader, it will wake up writer now
	}
	pthread_mutex_unlock(&lockingUDZ);//unlock again
}

void* writerFunction(void *writerno)//making writers here
{
	sem_wait(&wrt);//waiting untill readers done their jobs
	document=document*2;
	printf("Writer %d update %d document from database\n",*((int *)writerno),document);
	sem_post(&wrt);//done wirters 

}


int main()
{
	int n_readers,n_writers,i;
	printf("Enter number of readers:");
	scanf("%d" ,&n_readers);
	printf("Enter number of writers:");
	scanf("%d" ,&n_writers);
	
	pthread_t readers[n_readers],writers[n_writers];//we defined our threads as writer and readers
	pthread_mutex_init(&lockingUDZ,NULL);//initialize mutex with null value
	
	int readersarrayval[n_readers];
	int writersarrayval[n_writers];
	for(i=0;i<n_readers;i++){
		readersarrayval[i]=i+1;
	}
	for(i=0;i<n_writers;i++){
		writersarrayval[i]=i+1;
	}
	sem_init(&wrt,0,1);//we gave 0 and 1 value because we want that every time just 1 reader or writer made their own job
	
	for(i=0;i<n_readers;i++)
	{
		pthread_create(&readers[i],NULL,readerFunction,&readersarrayval[i]);//creation reader 
	}
	
	for(i=0;i<n_writers;i++)
	{
		pthread_create(&writers[i],NULL,writerFunction,&writersarrayval[i]);//creation writer
	}
	
	for(i=0;i<n_readers;i++){
		pthread_join(readers[i],NULL);
	}
	for(i=0;i<n_writers;i++){
		pthread_join(writers[i],NULL);
	}
	
	pthread_mutex_destroy(&lockingUDZ);//we finished our mutex jobs then we can destroy
	
	sem_destroy(&wrt);//finished our tasks with semaphores,destroyed
	
	
}




