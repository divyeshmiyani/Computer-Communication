#include <unistd.h>
#include <stdio.h>
#include <sys/io.h>

#define LCR 0x3fb
#define THR 0x3f8
#define DLM 0x3f9
#define LSR 0x3fd   

#define DLE '$'
#define STX '*'
#define ETX '@'

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
    printf("\nReceived Frame = ");
    for (int i = 0; i < 20; i++)
    {
        frame[i] = receiveByte();
        printf("%c",frame[i]);
        if (frame[i] == EOF)
            return 0;
    }
    return 1;

}

void makePacketFromFrame(char *frame, char *packet)
{
    int j = 0;
    printf("\nData received = ");
    for (int i = 0; i < 20; i++)
    {
        if ((frame[i] == DLE) && (frame[i+1] == DLE))
	        continue;
        
        if (((frame[i] == DLE) && (frame[i+1] == STX)) || ((frame[i] == DLE)&&(frame[i+1] == ETX)))
        {
        	i++;
	        continue;
        }
        packet[j++] = frame[i];
        printf("%c",frame[i]);
        fprintf(fp, "%c", frame[i]);
    }
}

void sendAck(char *frame)
{   // Sends Ack

    printf("Sending Ack = %s\n", frame);
    for (int i = 0; i < 5; i++)
    {
        while (((inb(LSR)) & 0x60) != 0x60)
            ;
        outb(frame[i], THR);
    }
}

void makeFrameFromPacket(char *frame, char *packet)
{
    // Makes frame from packet

	for(int i=0;i<20;i++)
	    frame[i] = '\0';

    // Start Of Frame
    frame[0] = DLE;
    frame[1] = STX;
    frame[2] = expected_number;

    int j = 3; // Index of frame
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

int main()
{
    char frame[20] = {'\0'};
    char packet[8];

    char ackPacket;
    char ackFrame[5];
    // char ackFrame = ?
    fp = fopen("ReceivedFile.txt", "w");

    ioperm(0x3f8, 7, 1);
    outb(0x83, 0x3fb);
    outb(0x0c, 0x3f8);
    outb(0x00, 0x3f9);
    outb(0x03, 0x3fb);

    while (receiveFrame(frame))
    {
        makePacketFromFrame(frame, packet);
        makeAckFrame(ackFrame, ackPacket);
        sendAck(ackFrame);
    }
    makePacketFromFrame(frame, packet);
    makeAckFrame(ackFrame, ackPacket);
    sendAck(ackFrame);

    printf("File Received\n");
}
