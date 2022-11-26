#include<unistd.h>
#include<stdio.h>
#include<sys/io.h>
int main()
{
	int i=0;
	ioperm(0x3f8,7,1);
	outb(0x83,0x3fb);
	outb(0x0c,0x3f8);
	outb(0x00,0x3f9);
	outb(0x03,0x3fb);

	char data;

	while(1) {
		printf("Enter character: ");
		scanf("%c",&data);
		outb(data,0x3f8);

		while((inb(0x3fd) & 0x60) != 0x60);
		printf("Sent data %c\n",data);
	}
	return 0;
}
