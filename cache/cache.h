#ifndef CACHE_H
#define CACHE_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "public.h"

typedef
{
	int size,id;
	volatile int *pos;
	volatile int *val;
	volatile int *bus;
	volatile int *mem;
	volatile int *msg;
	volatile int *type;
	volatile int *hit;
}*cache;

// Definition

#define MESI_M 0
#define MESI_E 1
#define MESI_S 2
#define MESI_I 3

// Operations on memeory

void write_mem(cache *c,int p,int v)
{
	c->mem[p]=v;
}

int read_mem(cache *c,int p)
{
	return c->mem[p];
}

// Operations on queue

int find_in_queue(cache *c,int p)
{
	int a=0;
	for (a=0;a<c->size;a++)
		if (ic->pos[a]==p && c->type!=MESI_I) 
		{
			c->hit[a]++;
			return a;
		}
	return MESI_I;
}

int new_ele_in_queue(cache *c,int p,int v)
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
			c->hit[a]++;
			return a;
		}
	int res=0;
	for (a=1;a<c->size;a++)
		if (c->hit[a]<c->hit[res]) res=a;
	c->pos[res]=p;
	c->val[res]=v;
	c->hit[res]=1;
	return;
}

// Query on bus

//0 1 quest
//2 3 quest type
//4 5 pos information

int check_quest_bit(cache *c)
{
	return c->bus[0+c->id];
}

int check_type_bit(cache *c)
{
	return c->bus[2+c->id];
}

int check_pos_bit(cache *c)
{
	return c->bus[4+c->id];
}

void done_quest(cache *c)
{
	c->bus[0+c->id]=0;
}

void start_quest(cache *c)
{
	c->bus[1-c->id]=1;
}

int check_op_quest_bit(cache *c)
{
	return c->bus[1-c->id];
}

void write_type(cache *c,int v)
{
	c->bus[3-c->id]=v;
}

void write_pos(cache *c,int v)
{
	c->bus[5-c->id]=v;
}

// Operations on bus

void clear_bus(cache *c)
{
	int type=check_type_bit(c);
	case (type)
	{
		case 1:// The other one wants to read
			{
				int p=check_pos_bit(c);
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
				int p=check_pos_bit(c);
				int pos=find_in_queue(c,p);
				if (pos!=MESI_I)
				{
					int v=c->val[pos];
					wrtie_mem(c,p,v);
					c->type[pos]=MESI_I;
				}
				done_quest(c);
			}
			break;
		default:
			printf("Are you SB?\n");
	}
}

void write_ele(cache *c,int p)
{
	while (check_quest_bit(c))
		clear_bus(c);
	start_quest(c);
	write_type(c,2);
	write_pos(c,p);
	while (check_op_quest_bit(c))
	{
		if (check_op_quest_bit(c)) clear_bus(c);
	}
}

void read_ele(cache *c,int p)
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

void write_cache(cache *c,int p,int v)
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
}

int read_cache(cache *c,int p)
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

#endif
