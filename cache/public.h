#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define CACHESIZE 32
#define BUSSIZE 32
#define MEMSIZE 3000
#define MSGSIZE 32

#define CACHESTART(x) (ftok("Makefile",x))
#define BUSSTART (ftok("Makefile",12))
#define MEMSTART (ftok("Makefile",18))
#define MSGSTART(x) (ftok("Makefile",x+6))
