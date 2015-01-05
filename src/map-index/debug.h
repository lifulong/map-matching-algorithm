
#include <stdio.h>
#include <time.h>

//accept c_str as input parameters.
#define debug_2_console(fmt,args...)	\
	do{									\
		time_t t_time;					\
		time(&t_time);					\
		printf("%s", ctime(&t_time));	\
		printf(fmt,##args);				\
	}while(0)

//accept c_str as input parameters.
#define debug_2_file(fmt,args...)	\
	do{								\
		FILE* fp;					\
		fp = fopen("log", "a+");	\
		fprintf(fp, fmt, ##args);	\
		fclose(fp);					\
	}while(0)

#define DEBUG_2_CONSOLE

#ifdef DEBUG_2_FILE
#define debug_msg(fmt,args...) debug_2_file(fmt,##args)
#elif defined DEBUG_2_CONSOLE
#define debug_msg(fmt,args...) debug_2_console(fmt,##args)
#else
#define debug_msg(fmt,args...)	do{} while(0)
#endif


