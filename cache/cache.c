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

	int cnt=0;
	while (1)
	{
		//read_msg(msgadd);
		if (msgadd[0])
		{
			//printf("%d %d\n",myID,msgadd[0]);
			if (msgadd[0]==1)
			{
				//Y86 ask to read msgadd[1] and store it msgadd[2]
				msgadd[2]=read_cache(mycache,msgadd[1]);
				//printf("ID %d: read from %d get value %d\n",myID,msgadd[1],msgadd[2]);
				msgadd[0]=0;
			}
			if (msgadd[0]==2)
			{
				//Y86 ask to write msgadd[2] in msgadd[1]
				write_cache(mycache,msgadd[1],msgadd[2]);
				//printf("ID %d: write into %d with value %d\n",myID,msgadd[1],msgadd[2]);
				msgadd[0]=0;
			}
			if (msgadd[0]==3)
			{
				//Y86 ask to atom
				msgadd[2]=atom_cache(mycache,msgadd[1]);
				msgadd[0]=0;
			}
		}
		while (check_quest_bit(mycache)) 
			clear_bus(mycache);
	}

	return 0;
}
