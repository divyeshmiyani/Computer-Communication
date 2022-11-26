#include <unistd.h>
#include <stdio.h>
#include <sys/io.h>

#define LCR 0x3fb
#define THR 0x3f8
#define DLM 0x3f9
#define LSR 0x3fd

FILE *fp;
char receiveByte()
{
	// Wait for next
	while ((inb(0x3fd) & 0x01) != 0x01)
		;

	return inb(0x3f8);
}

int receiveFrame(char *frame)
{
	for (int i = 0; i < 8; i++)
		frame[i] = '\0';
	for (int i = 0; i < 8; i++)
	{
		frame[i] = receiveByte();
		if (frame[i] == EOF)
			return 0;
	}
	printf("\nReceived Frame = ");
	return 1;
}

void generatePacket(char *frame, char *packet)
{
	for (int i = 0; i < 8; i++)
	{
		packet[i] = frame[i];
		printf("%c", frame[i]);
		fprintf(fp, "%c", frame[i]);
	}
}

int main()
{
	char frame[8];
	char packet[8];
	fp = fopen("ReceivedFile.txt", "w");

	ioperm(0x3f8, 7, 1);
	outb(0x83, 0x3fb);
	outb(0x0c, 0x3f8);
	outb(0x00, 0x3f9);
	outb(0x03, 0x3fb);

	while (receiveFrame(frame))
		generatePacket(frame, packet);
	generatePacket(frame, packet);

	printf("File Received\n");
}
