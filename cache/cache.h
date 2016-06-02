#ifndef CACHE_H
#define CACHE_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "public.h"

typedef struct
{
	int size,id,maxsize;
	int *pos;
	int *val;
	int *bus;
	int *mem;
	int *msg;
	int *type;
	int *hit;
}*cache,no_pointer_cache;

// Definition

#define MESI_M -1
#define MESI_E -2
#define MESI_S -3
#define MESI_I -4

// Operations on memeory

void write_mem(cache c,int p,int v)
{
	c->mem[p]=v;
}

int read_mem(cache c,int p)
{
	return c->mem[p];
}

// Operations on queue

int find_in_queue(cache c,int p)
{
	int a=0;
	for (a=0;a<c->size;a++)
		if (c->pos[a]==p && c->type[a]!=MESI_I) 
		{
			c->hit[a]++;
			return a;
		}
	return MESI_I;
}

int new_ele_in_queue(cache c,int p,int v)
{
	int a=0;
	if (c->size<c->maxsize)
	{
		c->pos[c->size]=p;
		c->val[c->size]=v;
		c->hit[c->size]=1;
		c->size++;
		return c->size-1;
	}
	for (a=0;a<c->size;a++)
		if (c->type[a]==MESI_I)
		{
			c->pos[a]=p;
			c->val[a]=v;
			c->hit[a]=1;
			return a;
		}
	int res=0;
	for (a=1;a<c->size;a++)
		if (c->hit[a]<c->hit[res]) res=a;
	write_mem(c,c->pos[res],c->val[res]);
	c->pos[res]=p;
	c->val[res]=v;
	c->hit[res]=1;
	return res;
}

// Query on bus

//0 1 quest
//2 3 quest type
//4 5 pos information

int check_quest_bit(cache c)
{
	return c->bus[0+c->id];
}

int check_type_bit(cache c)
{
	return c->bus[2+c->id];
}

int check_pos_bit(cache c)
{
	return c->bus[4+c->id];
}

void done_quest(cache c)
{
	c->bus[4+c->id]=-1;
	c->bus[0+c->id]=0;
}

void start_quest(cache c)
{
	c->bus[1-c->id]=1;
}

int check_op_quest_bit(cache c)
{
	return c->bus[1-c->id];
}

void write_type(cache c,int v)
{
	c->bus[3-c->id]=v;
}

void write_pos(cache c,int v)
{
	c->bus[5-c->id]=v;
}

// Operations on bus

void clear_bus(cache c)
{
	int type=check_type_bit(c);
	//printf("%d\n",type);
	switch (type)
	{
		case 1:// The other one wants to read
			{
				int p=-1;
				while (p==-1)
					p=check_pos_bit(c);
				int pos=find_in_queue(c,p);
				if (pos!=MESI_I)
				{
					int v=c->val[pos];
					c->type[pos]=MESI_S;
					write_mem(c,p,v);
				}
				/*else
				{
					int v=read_mem(c,p);
					int pos=new_ele_in_queue(c,p,v);
					c->type[pos]=MESI_S;
				}*/
				done_quest(c);
			}
			break;
		case 2:// The other one wants to write
			{
				int p=-1;
				while (p==-1)
					p=check_pos_bit(c);
				//printf("%d process 1\n",c->id);
				int pos=find_in_queue(c,p);
				//printf("%d process 2\n",c->id);
				if (pos!=MESI_I)
				{
					int v=c->val[pos];
					write_mem(c,p,v);
					c->type[pos]=MESI_I;
				}
				//printf("%d process 3\n",c->id);
				done_quest(c);
			}
			break;
		default:
			printf("Are you SB?\n");
	}
}

void write_ele(cache c,int p)
{
	while (check_quest_bit(c))
		clear_bus(c);
	write_type(c,2);
	write_pos(c,p);
	start_quest(c);
	while (check_op_quest_bit(c))
	{
		if (check_quest_bit(c)) clear_bus(c);
	}
}

void read_ele(cache c,int p)
{
	while (check_quest_bit(c))
		clear_bus(c);
	write_type(c,1);
	write_pos(c,p);
	start_quest(c);
	while (check_op_quest_bit(c))
	{
		if (check_quest_bit(c)) clear_bus(c);
	}
}

// Operations on cache

void write_cache(cache c,int p,int v)
{
	int pos=find_in_queue(c,p);
	if (pos==MESI_I) 
	{
		write_ele(c,p);
		write_mem(c,p,v);
		pos=new_ele_in_queue(c,p,v);
		c->type[pos]=MESI_E;
	}
	else
	{
		if (c->type[pos]==MESI_S)
		{
			write_ele(c,p);
			write_mem(c,p,v);
			c->type[pos]=MESI_E;
			c->val[pos]=v;
		}
		else
		{
			if (c->type[pos]==MESI_E)
			{
				c->val[pos]=v;
				c->type[pos]=MESI_M;
			}
			else c->val[pos]=v;
		}
	}
	//printf("%d process 4\n",c->id);
}

int read_cache(cache c,int p)
{
	int pos=find_in_queue(c,p);
	int res;
	if (pos!=MESI_I) res=c->val[pos];
	else
	{
		read_ele(c,p);
		res=read_mem(c,p);
		pos=new_ele_in_queue(c,p,res);
		c->type[pos]=MESI_S;
	}
	return res;
}

int atom_cache(cache c,int p)
{
	int v=read_cache(c,p);
	write_cache(c,p,v+1);
	return v;
}

cache new_cache(int id)
{
	cache res=(cache)malloc(sizeof(no_pointer_cache));
	res->size=0;
	res->maxsize=CACHESIZE;
	res->pos=(int*)calloc(res->maxsize,sizeof(int));
	res->val=(int*)calloc(res->maxsize,sizeof(int));
	res->type=(int*)calloc(res->maxsize,sizeof(int));
	res->hit=(int*)calloc(res->maxsize,sizeof(int));
	res->bus=shmat(shmget(BUSSTART,0,0),0,0);
	res->mem=shmat(shmget(MEMSTART,0,0),0,0);
	res->id=id;
	return res;
}

// Test on cache

void msg_write(int *msg,int p,int v)
{
	//if (p>=1600) printf("I want to write into %d with value %d\n",p,v);
	msg[1]=p;
	msg[2]=v;
	msg[0]=2;
	while (msg[0])
	{;
		//freopen("out","w",stderr);
		//fprintf(stderr,"%d\n",msg[0]);
	}
	//if (p>=1600) printf("msg_write is done\n");
}

int msg_read(int *msg,int p)
{
	//if (p>=1600) printf("I want to read from %d\n",p);
	msg[1]=p;
	msg[0]=1;
	while (msg[0])
	{;
		//freopen("out","w",stderr);
		//fprintf(stderr,"%d\n",msg[0]);
	}
	//if (p>=1600) printf("msg_read is done %d\n",msg[2]);
	return msg[2];
}

int msg_atom(int *msg,int p)
{
	msg[1]=p;
	msg[0]=3;
	while (msg[0])
	{;
		//freopen("out","w",stderr);
		//fprintf(stderr,"%d\n",msg[0]);
	}
	int base=1600;
	p-=base;
	//printf("I want to atom from %d\n",p);
	if (p==4) printf("Send");
	if (p==5) printf("Receive");
	if (p==6) printf("\n");
	if (p==0)
	{
		int ans[4];
		ans[0]=msg[2];
		int a=1;
		for (a=1;a<=3;a++)
		{
			msg[1]=p+a+base;
			msg[0]=1;
			while (msg[0])
			{;
				//freopen("out","w",stderr);
				//fprintf(stderr,"%d\n",msg[0]);
			}
			ans[a]=msg[2];
		}
		int res=0;
		for (a=3;a>=0;a--)
			res=res*256+ans[a];
		printf(" %d",res);
	}
	//printf("msg_atom is done %d\n",msg[2]);
	return msg[2];
}

#endif
