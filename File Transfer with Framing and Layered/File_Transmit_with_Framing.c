#include <stdio.h>
#include <sys/io.h>
#include <unistd.h>
#define LCR 0x3fb
#define THR 0x3f8
#define DLM 0x3f9
#define LSR 0x3fd

FILE *fp;
int generate_packet(char *packet)
{
	// Makes packet by reading file

	for (int i = 0; i < 8; i++)
	{
		packet[i] = getc(fp);

		if (packet[i] == EOF)
			return 0;
	}
	return 1;
}

void generate_frame(char *frame, char *packet)
{
	// Makes frame from packet
	for (int i = 0; i < 8; i++)
	{
		frame[i] = packet[i];
	}
}

void send_frame(char *frame)
{
	// send frame

	printf("Sending Frame = %s\n", frame);
	for (int i = 0; i < 8; i++)
	{
		while (((inb(LSR)) & 0x60) != 0x60)
			;
		outb(frame[i], THR);
	}
}

int main()
{
	fp = fopen("TransmitFile.txt", "r");

	ioperm(THR, 7, 1);
	outb(0x83, LCR);
	outb(0x0C, THR);
	outb(0x00, DLM);
	outb(0x03, LCR);

	char packet[8];
	char frame[8];
	while (generate_packet(packet))
	{
		generate_frame(frame, packet);
		send_frame(frame);
	}

	generate_frame(frame, packet);
	send_frame(frame);

	return 0;
}
