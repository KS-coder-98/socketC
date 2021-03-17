/*  Make the necessary includes and set up the variables.  */

// #include <sys/types.h>
// #include <sys/socket.h>
// #include <stdio.h>
// // #include <stdlib.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// // #include <errno.h>
// #include <string.h>

#include "read_save.h"

char buffer[SIZE_BUFFER];

int main ()
{
	int sockfd;
	socklen_t len; // rozmiar struktury sockaddr_in
	//     struct sockaddr_in {
	//        sa_family_t    sin_family; /* address family: AF_INET */
	//        in_port_t      sin_port;   /* port in network byte order */
	//        struct in_addr sin_addr;   /* internet address */
	//    };

	// 	/* Internet address. */
	// struct in_addr {
	// 	uint32_t       s_addr;     /* address in network byte order */
	// };
	struct sockaddr_in address;
	int result;

	/*  Create a socket for the client.  */
	//tworzenie socketu 
	//1arg oznacza ze protokul z rodziny TCP/IP
	//2arg ze to jest protokol TCP
	//3arg nie istotny
	//socket zwraca liczbe calkowita jest to numer dekryptora pliku lub -1 jak blad i errno
	sockfd = socket (AF_INET, SOCK_STREAM, 0);

	/*  Name the socket, as agreed with the server.  */

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr ("127.0.0.1");
	address.sin_port = htons (9734); // zamiana beznakowego shorta int z bigendian na littlendian
	//w protokolach sieciowych jest bigendian
	len = sizeof (address);

	/*  Now connect our socket to the server's socket.  */
	//1arg -> deskryptor pliku socketu
	//2arg -> jest to struktura w kturej okreslamy powyzsze parametry i jest ona bardziej ogolna niz 
	// struktura addr_in poniewaz connect laczy sie nie tylko poprzez TCP?IP
	//3arg -> rozmiar struktury z drugiego parametru
	result = connect (sockfd, (struct sockaddr *) &address, len);

	if (result == -1)
	{
		//wypisze blad skojarzony z bledem errno
		perror ("oops: netclient");
		exit (1);
	}

	/*  We can now read/write via sockfd.  */
	//write
	//1arg -> deskryptor pliku (socket)
	//2arg -> adres buffora z ktorego bedziemy pisac zapisujemy do niego wartosc
	//3arg -> liczba bajtow ktora chcemy zapisac
	

	struct msg msgToSend;
	// struct msg msg_read;
	
	// //to sqrt
	setFlag(&msgToSend, CALCULATE_SQRT_RYQ);
	setReqIdInMsg(&msgToSend, generateReqIdInProcess());
	double numberToSqrt = 16;
	if ( __FLOAT_WORD_ORDER__ != BIG_ENDIAN ){
		numberToSqrt = htobe64(numberToSqrt);
	}
	memcpy(&msgToSend.data, &numberToSqrt, 8);

	// // setFlag(&msgToSend, GET_TIME_REQ);
	// // setReqIdInMsg(&msgToSend, generateReqIdInProcess());

	safe_write(sockfd, &msgToSend, SIZE_BUFFER);

	// //read
	// //1arg -> deskryptor pliku (socket)
	// //2arg -> adres buffora z ktorego bedziemy czytac odczytujemy z niego wartosc
	// //3arg -> liczba bajtow ktora chcemy zapisac
	// safe_read(sockfd, &buffer, SIZE_BUFFER);
	// procesMsg(buffer);

	// sleep(2);

	// setFlag(&msgToSend, GET_TIME_REQ);
	// setReqIdInMsg(&msgToSend, generateReqIdInProcess());
	// safe_write(sockfd, &msgToSend, SIZE_BUFFER);
	safe_read(sockfd, &buffer, SIZE_BUFFER);
	procesMsg(buffer);

	close (sockfd);
	exit (0);
}
