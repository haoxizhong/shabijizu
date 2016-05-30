all: cache testone prework

cache: cache.h public.h cache.c
	gcc cache.c -o cache -Wall -g

testone: cache.h public.h testone.c
	gcc testone.c -o testone -Wall -g

prework: cache.h public.h prework.c
	gcc prework.c -o prework -Wall -g

clean:
	rm -f cache 
	rm -f testone
	rm -f prework
