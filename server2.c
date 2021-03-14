#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <math.h>

#include "read_save.h"
#include "fun_server.h"

char buffer[SIZE_BUFFER];


int main ()
{
	int server_sockfd, client_sockfd;
	socklen_t server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	server_sockfd = socket (AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl (INADDR_ANY);
	server_address.sin_port = htons (9734);
	server_len = sizeof (server_address);
	bind (server_sockfd, (struct sockaddr *) &server_address, server_len);

	/*  Create a connection queue, ignore child exit details and wait for clients.  */

	listen (server_sockfd, 5);

	//po zakonczeniu procesu dziecka nie staje procesem zombie
	// signal (SIGCHLD, SIG_IGN);

	while (1)
	{

		printf ("server waiting\n");

		/*  Accept connection.  */

		client_len = sizeof (client_address);
		client_sockfd = accept (server_sockfd,
				(struct sockaddr *) &client_address,
				&client_len);

		/*  Fork to create a process for this client and perform a test to see
			whether we're the parent or the child.  */
		//fork tworzy kopie procesu jak wchodzimy to z jednego a wychodzimy w dwuch
		//stan preocesu i pamieci jest kopiowany to procesu potomnego
		// mozemy rozruznic czy jestem w procesie macierzystym
		// fork return 0-> proces potomny
		// fork return -1-> nie udao nam sie utworzyc nowego procesu
		// fork return pid of proces dziecka
		if (fork () == 0)
		{
			
			/*  If we're the child, we can now read/write to the client on client_sockfd.
				The five second delay is just for this demonstration.  */
			safe_read(client_sockfd, buffer, SIZE_BUFFER);
			procesMsg(buffer);
			safe_write(client_sockfd, buffer, SIZE_BUFFER);
			close (client_sockfd);
			exit (0);
		}
		/*  Otherwise, we must be the parent and our work for this client is finished.  */
		else
		{
			close (client_sockfd);
		}
	}
}
