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
				int pos=find_in_queue(c,p);
				if (pos!=MESI_I)
				{
					int v=c->val[pos];
					write_mem(c,p,v);
					c->type[pos]=MESI_I;
				}
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
	start_quest(c);
	write_type(c,2);
	write_pos(c,p);
	while (check_op_quest_bit(c))
	{
		if (check_quest_bit(c)) clear_bus(c);
	}
}

void read_ele(cache c,int p)
{
	while (check_quest_bit(c))
		clear_bus(c);
	start_quest(c);
	write_type(c,1);
	write_pos(c,p);
	while (check_op_quest_bit(c))
	{
		if (check_quest_bit(c)) clear_bus(c);
	}
}

// Operations on cache

void write_cache(cache c,int p,int v)
{
	int pos=find_in_queue(c,p);
	/*if (p==87)
	{
		printf("Write\n");
		printf("%d %d %d\n",p,v,pos);
		if (pos!=MESI_I)
		{
			printf("%d %d %d %d\n",c->pos[pos],c->val[pos],c->type[pos],c->hit[pos]);
		}
	}*/
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
}

int read_cache(cache c,int p)
{
	int pos=find_in_queue(c,p);
	/*if (p==87)
	{
		printf("Read\n");
		printf("%d %d\n",p,pos);
		if (pos!=MESI_I)
		{
			printf("%d %d %d %d\n",c->pos[pos],c->val[pos],c->type[pos],c->hit[pos]);
		}
	}*/
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
	msg[0]=2;
	msg[1]=p;
	msg[2]=v;
	while (msg[0]);
}

int msg_read(int *msg,int p)
{
	msg[0]=1;
	msg[1]=p;
	while (msg[0]);
	return msg[2];
}

#endif
