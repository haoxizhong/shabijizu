#include"cache.h"

int main(int argc,char *argv[])
{
	if (argc!=2)
	{
		printf("Please use the correct ID number\n");
		return 1;
	}
	int myID;
	if (argv[1][0]=='0' && strlen(argv[1])==1) myID=1;
	else
	{
		if (argv[1][0]=='1' && strlen(argv[1])==1) myID=2;
		else
		{
			printf("Please use the correct ID number\n");
			return 1;
		}
	}

	volatile int* cacheadd = shmat(shmget(CACHESTART(myID),0,0),0,0);
	volatile int* busadd = shmat(shmget(BUSSTART,0,0),0,0);
	volatile int* memadd = shmat(shmget(MEMSTART,0,0),0,0);
	volatile int* msgadd = shmat(shmget(MSGSTART(myID),0,0),0,0);

	volatile cache* mycache;

	while (1)
	{
		read_msg(msgadd);
		if (msgadd)
		{
			if (msgadd[0]==1)
			{
				//Y86 ask to read msgadd[1] and store it msgadd[2]
				msgadd[2]=read(cache,msgadd[1]);
				msgadd[0]=0;
			}
			else
			{
				//Y86 ask to write msgadd[2] in msgadd[1]
				write(cache,msgadd[1],msgadd[2]);
				msgadd[0]=0;
			}
		}
		else 
		{
			while (check_quest(c)) 
				clear_bus(c);
		}
	}

	return 0;
}
