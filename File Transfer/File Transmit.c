#include<unistd.h>
#include<stdio.h>
#include<sys/io.h>
int main()
{
	FILE *fp;
	char data;
	fp = fopen("a.txt","r");

	ioperm(0x3f8,7,1);
	outb(0x83,0x3fb);
	outb(0x0c,0x3f8);
	outb(0x00,0x3f9);
	outb(0x03,0x3fb);

	do{
		data = fgetc(fp);
		outb(data,0x3f8);
		printf("data sent =%c\n",data);
		while((inb(0x3fd) & 0x60) != 0x60); // 60= 0110 0000

  }while(data != EOF);

	printf("File Transmitted\n");
}
