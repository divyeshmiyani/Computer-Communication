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
int ackFrameSize = 5;

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
        printf("%c", frame[i]);
        if (frame[i] == EOF)
            return 0;
    }
    return 1;
}

void makePacketFromFrame(char *frame, char *packet)
{
    int j = 0;
    printf("\nData received = ");
    for (int i = 0; i < 21; i++)
    {
        if ((frame[i] == DLE) && (frame[i + 1] == DLE))
            continue;

        if (((frame[i] == DLE) && (frame[i + 1] == STX)) || ((frame[i] == DLE) && (frame[i + 1] == ETX)))
        {
            i++;
            continue;
        }
        packet[j++] = frame[i];
        printf("%c", frame[i]);
        fprintf(fp, "%c", frame[i]);
    }
}

void sendAck(char *frame)
{ // Sends Ack

    printf("Sending Ack = %s\n", frame);
    for (int i = 0; i < 5; i++)
    {
        while (((inb(LSR)) & 0x60) != 0x60)
            ;
        outb(frame[i], THR);
    }
}

void makeAckFrame(char *frame, char *packet)
{
    // Makes frame from packet

    for (int i = 0; i < 5; i++)
        frame[i] = '\0';

    // Start Of Frame
    frame[0] = DLE;
    frame[1] = STX;
    frame[2] = packet[0];

    // End of Frame
    frame[3] = DLE;
    frame[4] = ETX;
}

int main()
{
    char frame[20] = {'\0'};
    char packet[8];

    char ackPacket = "0";
    char ackFrame[5];
    // char ackFrame = ?
    fp = fopen("ReceivedFile.txt", "w");

    ioperm(THR, 7, 1);
    outb(0x83, LCR);
    outb(0x0c, THR);
    outb(0x00, DLM);
    outb(0x03, LCR);

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
