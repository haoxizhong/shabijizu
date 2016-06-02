#include "cache.h"

int *bus,*mem,*msg0,*msg1;

int main()
{
	printf("%d\n",CACHESTART(0));
	printf("%d\n",CACHESTART(1));
	printf("%d\n",MSGSTART(0));
	printf("%d\n",MSGSTART(1));
	printf("%d\n",BUSSTART);
	printf("%d\n",MEMSTART);
	bus=shmat(shmget(BUSSTART,BUSSIZE*sizeof(int),IPC_CREAT|0666),0,0);
	msg0=shmat(shmget(MSGSTART(0),MSGSIZE*sizeof(int),IPC_CREAT|0666),0,0);
	msg1=shmat(shmget(MSGSTART(1),MSGSIZE*sizeof(int),IPC_CREAT|0666),0,0);
	mem=shmat(shmget(MEMSTART,MEMSIZE*sizeof(int),IPC_CREAT|0666),0,0);

	int a=0;
	for (a=0;a<BUSSIZE;a++)
		bus[a]=0;
	for (a=0;a<MEMSIZE;a++)
		mem[a]=0;
	for (a=0;a<MSGSIZE;a++)
		msg0[a]=msg1[a]=0;

	while (1);
	return 0;
}
