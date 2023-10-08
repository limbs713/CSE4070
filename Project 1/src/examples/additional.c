#include<stdio.h>
#include<syscall.h>


int atoi(char* str)
{
    int i=0;
    int res =0;
    while(str[i]<= '9' && str[i] >= '0')
    {
        res = res * 10 + (str[i] - '0');
        i++;
    }

    return res;
}

int main(int argc, char* argv[])
{

  if (argc != 5) 
    {
      printf ("usage: additional A B C D\n");
      exit(-1);
    }
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    int c = atoi(argv[3]);
    int d = atoi(argv[4]);
    printf("%d %d\n", fibonacci(a), max_of_four_int(a,b,c,d));

    return 0;
}
