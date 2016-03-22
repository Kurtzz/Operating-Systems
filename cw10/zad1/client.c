
#include "commons.h"
//------------------------------------------------------------------------------
typedef struct thread_params {
   int socket;
   struct sockaddr* server;
   socklen_t server_size;
   Message msg;
} thread_params;
//------------------------------------------------------------------------------
char* CLIENT_ID;
struct sockaddr* server;
socklen_t server_size;
struct sockaddr_un server_un;
struct sockaddr_un sock_un;
int type = 0;	// 0 - unix, 1 - internet
//------------------------------------------------------------------------------
void clean(int);
void* run(void*);
int getInternetSocket(char*);
int getUnixSocket(void);
//------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
  if(argc != 3) {
		printf("usage: %s [CLIENT_ID] [-u | -i]\n", argv[0]);
		return 0;
	}

  signal(SIGINT, clean);

	CLIENT_ID = argv[1];
	CLIENT_ID[strlen(argv[1])] = '\0';

	char buffer[MAX_NAME_LENGTH];
	int sock;

  if(strcmp(argv[2], "-u") == 0) {
    //c = 'u';
    type = 0;
    sock = getUnixSocket();

  } else if (strcmp(argv[2], "-i") == 0) {
    type = 1;
    printf("\nINET> Enter server address: ");
    fgets(buffer, 99, stdin);
    buffer[strlen(buffer)] = '\0';

    // get internet socket
    sock = getInternetSocket(buffer);
  } else {
    printf("Wrong socket type\n"
            "usage: [-u | -i]\n");
    exit(-1);
  }

	Message msg;
	msg.size = server_size;
	strcpy(msg.name, CLIENT_ID);

	// create thread for receiving message
	pthread_t thread;

	thread_params* params = (thread_params*)malloc(sizeof(thread_params));
	params->socket = sock;
	params->msg = msg;
	params->server = server;
	params->server_size = server_size;

	if(pthread_create(&thread, NULL, &run, (void*)params) < 0) {
    printf("pthread_create(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  strcpy(msg.content, "Registration message");
  if(sendto(sock, &msg, sizeof(msg) , 0 , server, server_size)==-1) {
    printf("sendto(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }
	while(1) {
    fgets(msg.content, msg.size, stdin);
    //send the message
    if(sendto(sock, &msg, sizeof(msg) , 0 , server, server_size)==-1) {
      printf("sendto(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
  }

  if(pthread_join(thread, NULL)) {
    printf("pthread_join(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

	close(sock);

	return 0;
}
//------------------------------------------------------------------------------
void clean(int arg) {

  char path[256] = {0};
	sprintf(path, "/tmp/s%d", getpid());
	unlink(path);

  printf("[client] clean\n");

	exit(0);
}
//------------------------------------------------------------------------------
void* run(void* args) {
	thread_params* params = (thread_params*) args;

	while(1) {
		//try to receive some data, this is a blocking call
		if(recvfrom(params->socket, &params->msg, sizeof(params->msg), 0, params->server, &params->server_size) == -1) {
      printf("recvfrom(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
		printf("<%s>: %s\n", params->msg.name, params->msg.content);
	}

	return NULL;
}
//------------------------------------------------------------------------------
int getInternetSocket(char* address) {
	int sock;

	struct sockaddr_in* sock_in = malloc(sizeof(struct sockaddr_in));
	sock_in->sin_family = AF_INET;
  sock_in->sin_port = htons(PORT_NO);

	if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    printf("socket(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }
  if(inet_aton(address, &sock_in->sin_addr) < 0) {
    printf("socket(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  server = (struct sockaddr*)sock_in;
  server_size = sizeof(*sock_in);

  return sock;
}
//------------------------------------------------------------------------------
int getUnixSocket() {
  char path[256] = {0};
	int sock;
	sprintf(path, "/tmp/s%d", getpid());

	path[strlen(path)] = '\0';
	unlink(path);

	if((sock = socket(PF_UNIX, SOCK_DGRAM, 0)) == -1) {
    printf("socket(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  sock_un.sun_family = AF_UNIX;
  strcpy(sock_un.sun_path, path);

  if(bind(sock, (struct sockaddr*)&sock_un, SUN_LEN(&sock_un)) < 0) {
    printf("pthread_create(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  server_un.sun_family = AF_UNIX;
  strcpy(server_un.sun_path, SERVER_PATH);
  printf("path: %s\n", server_un.sun_path);

  server = (struct sockaddr*)&server_un;
  server_size = SUN_LEN(&sock_un);

  return sock;
}
