#include"cache.h"

int main(int argc,char *argv[])
{
	if (argc!=2)
	{
		printf("Please use the correct ID number\n");
		return 1;
	}
	int myID;
	if (argv[1][0]=='0' && strlen(argv[1])==1) myID=0;
	else
	{
		if (argv[1][0]=='1' && strlen(argv[1])==1) myID=1;
		else
		{
			printf("Please use the correct ID number\n");
			return 1;
		}
	}

	cache mycache=new_cache(myID);
	int* msgadd=shmat(shmget(MSGSTART(myID),MSGSIZE,0),0,0);
	//printf("%d\n",MSGSTART(myID));

	while (1)
	{
		//read_msg(msgadd);
		if (msgadd[0])
		{
			if (msgadd[0]==1)
			{
				//Y86 ask to read msgadd[1] and store it msgadd[2]
				msgadd[2]=read_cache(mycache,msgadd[1]);
				msgadd[0]=0;
			}
			else
			{
				//Y86 ask to write msgadd[2] in msgadd[1]
				write_cache(mycache,msgadd[1],msgadd[2]);
				msgadd[0]=0;
			}
		}
		while (check_quest_bit(mycache)) 
			clear_bus(mycache);
	}

	return 0;
}
