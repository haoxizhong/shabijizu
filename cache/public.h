#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define CACHESIZE 32
#define BUSSIZE 32
#define MEMSIZE 1024
#define MSGSIZE 32

#define CACHESTART(x) (ftok("Makefile",x))
#define BUSSTART (ftok("Makefile",45))
#define MEMSTART (ftok("Makefile",50))
#define MSGSTART(x) (ftok("Makefile",x+10))
