// // C function showing how to do time delay
// #include <stdio.h>
// // To use time library of C
// #include <time.h>

// void delay(int number_of_seconds)
// {
// 	// Converting time into milli_seconds
// 	int milli_seconds = 1000 * number_of_seconds;

// 	// Storing start time
// 	clock_t start_time = clock();

// 	// looping till required time is not achieved
// 	while (clock() < start_time + milli_seconds)
// 		;
// }

#include<stdio.h>
// void passfun(int (*f)())
// {
//   int a = f();
//   printf("%d",a);
// }

// int fun()
// {
//   return 4;
// }

// int main()
// {
//   char fu[] = "fun";
//   passfun(fun);
// }

char fun()
{
  return '1';
}

int main() 
{
  int i = 5;
  char a = '0';
  printf("%c = %d\n",i+'0', i+'0');
  printf("%c = %d",a +i,a + i);
  // char temp;
  // if(temp = fun())
  // {
  //   printf("%c", (char)95);
  // }
  // printf("out");
}