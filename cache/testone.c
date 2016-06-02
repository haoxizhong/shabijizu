#include "cache.h"

int mem[100010];

int main(int argc,char *argv[])
{
	//freopen("result.out","w",stdout);
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
	int* msgadd=shmat(shmget(MSGSTART(myID),MSGSIZE,0),0,0);
	//printf("%d\n",MSGSTART(myID));
	int maxtime=10000;
	int a;
	for (a=0;a<maxtime;a++)
	{
		printf("%d\n",a);
		int type=rand()%2,p=rand()%100,v=rand()%100;
		//printf("%d %d %d\n",type,p,v);
		if (type==0)
		{
			//test write
			msg_write(msgadd,p,v);
			mem[p]=v;
		}
		if (type==1)
		{
			//test read
			int v=msg_read(msgadd,p);
			if (mem[p]!=v)
			{
				printf("Are you SB?\n");
				return 1;
			}
		}
	}
	printf("Passed\n");

	return 0;
}
