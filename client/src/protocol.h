#ifndef PROTOCOL_H_
#define PROTOCOL_H_
#define BUFFERSIZE 512
//message structure
struct msgStruct
{
	char operation;
	char value1[BUFFERSIZE];
	char value2[BUFFERSIZE];
};
#endif /* PROTOCOL_H_ */
