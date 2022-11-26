#include <stdio.h>
#include <sys/io.h>
#include <unistd.h>
#define LCR 0x3fb
#define THR 0x3f8
#define DLM 0x3f9
#define LSR 0x3fd

#define DLE '$'
#define STX '*'
#define ETX '@'

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

	for (int i = 0; i < 20; i++)
		frame[i] = '\0';
	// Makes frame from packet

	// Start Of Frame
	frame[0] = DLE;
	frame[1] = STX;

	int j = 2; // Index of frame
	for (int i = 0; i < 8; i++)
	{
		frame[j++] = packet[i];

		if (packet[i] == DLE)
			frame[j++] = DLE;
	}

	// End of Frame
	frame[j++] = DLE;
	frame[j++] = ETX;
}

void send_frame(char *frame)
{
	// send frame

	printf("Sending Frame = %s\n", frame);
	for (int i = 0; i < 20; i++)
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
	char frame[20] = {'\0'};
	while (generate_packet(packet))
	{
		generate_frame(frame, packet);
		send_frame(frame);
	}

	generate_frame(frame, packet);
	send_frame(frame);

	return 0;
}
