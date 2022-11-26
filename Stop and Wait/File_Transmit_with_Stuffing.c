/* Sender                    Receiver
 * |                              |
 * |->                            |
 * |    frame01->                 |
 * |           frame01->          |
 * |                    frame01-> |
 * |                              |
 * |                            <-|
 * |                    <-ACK00   |
 * |          <-ACK00             |
 * |<-ACK00                       |
 * 
 */


/*  Sending Frame Structure Total size = 22 Byte
 *  frame     = | DLE | STX | SEQNo | ACKNo |   DATA   |  DLE  |  ETX  |
 *  ByteIndex = |  0  |  1  |   2   |   3   |  4...19  |   20  |   21  |
 * 
 *  Receive Frame Structure
 *  frame      = | DLE | STX | ACKNo |  DLE  |  ETX  |
 *  ByteIndex  = |  0  |  1  |   2   |   3   |   4   |
 */
//

#include <stdio.h>
#include <sys/io.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define LCR 0x3fb
#define THR 0x3f8
#define DLM 0x3f9
#define LSR 0x3fd

#define DLE '$'
#define STX '*'
#define ETX '@'

FILE *fp;
int seqNo = 0;
int expected_number = 0;
clock_t start_time;
int delayInSecond = 10;

void startTimer() 
{   // Starts Timer

    startTime = clock();
    printf("\nStart Time = %f\n", (double)startTime);
}

char receiveByte()
{   // Waits for next Byte

    // Timeout Algo
    int delayInMillisecond = 1000 * delayInSecond;
    while(clock() < startTime + delayInMillisecond);
    {
        if((inb(0x3fd) & 0x01) != 0x01)
            return inb(0x3f8);
    }
    return 0;

    // OLD
    // while((inb(0x3fd) & 0x01) != 0x01);
    
    // return inb(0x3f8);        
}

int receiveFrame(char *frame)
{   //Receives Frame

    printf("\nReceived Frame = ");
    for (int i = 0; i < 5; i++)
    {
        char temp;
        if(temp = receiveByte())
        {
            frame[i] = temp;

            // frame[i] = receiveByte();
            printf("%c",frame[i]);
            if (frame[i] == EOF)
                return 0;
        }
        else
            return -1;
    }
    return 1;
}


// void makePacketFromFrame(char *frame, char *packet)
// {// Makes packet from frame 
//     int j = 0;
//     printf("\nData received = ");
//     for (int i = 0; i < 20; i++)
//     {
//         if ((frame[i] == DLE) && (frame[i+1] == DLE))
// 	        continue;
        
//         if (((frame[i] == DLE) && (frame[i+1] == STX)) || ((frame[i] == DLE)&&(frame[i+1] == ETX)))
//         {
//         	i++;
// 	        continue;
//         }
//         packet[j++] = frame[i];
//         printf("%c",frame[i]);
//     }
// }

int isCorrectAckNumber(char *frame)
{
    // For now : 0 - DLE      
    //         : 1 - STX   
    //         : 2 - ACKNo.
    //         : 3 - DLE   
    //         : 4 - ETX   
    expected_number = 1 - expected_number
    return frame[2] == 1 - expected_number; // 0th Byte of frame is ACK No.
}

void sendFrame(char *frame)
{   // Sends frame

    printf("Sending Frame = %s\n", frame);
    for (int i = 0; i < 22; i++)
    {
        while (((inb(LSR)) & 0x60) != 0x60)
            ;
        outb(frame[i], THR);
    }
}

void makeFrameFromPacket(char *frame, char *packet)
{   // Makes frame from packet

	for(int i=0;i<20;i++)
	    frame[i] = '\0';

    // Start Of Frame
    frame[0] = DLE;
    frame[1] = STX;
    frame[2] = seqNo;
    frame[3] = expected_number;

    int j = 4; // Index of frame
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

int generateNewPacket(char *packet)
{   // Makes packet by reading file

    for (int i = 0; i < 8; i++)
    {
        packet[i] = getc(fp);

        if (packet[i] == EOF)
            return 0;
    }
    return 1;
}

int main()
{
    char packet[8];
    char frame[22] = {'\0'};
    char received_frame[5] = {'\0'};    // TODO: size = 20

    fp = fopen("TransmitFile.txt", "r");

    ioperm(THR, 7, 1);
    outb(0x83, LCR);
    outb(0x0C, THR);
    outb(0x00, DLM);
    outb(0x03, LCR);

    generateNewPacket(packet);
    while (1)
    {
        // Makes Frame from Generated Packet
        makeFrameFromPacket(frame, packet);

        // Sends Frame
        sendFrame(frame);

        // Starting Timer 
        startTimer();

        // Waits for ACK
        while(receiveFrame(received_frame) == -1)
        {
            // Sends Frame
            sendFrame(frame);

            // Starting Timer 
            startTimer();
        }
        
        // Validates ACK
        if(isCorrectAckNumber(received_frame))
        {
            printf("\nTransmitted Successfully");

            seqNo = 1 - seqNo;
            if(!generateNewPacket(packet))
                break;  // If packet not generated then break
        }
    }

    makeFrameFromPacket(frame, packet);
    sendFrame(frame);

    return 0;
}
