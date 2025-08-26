#include "net_utility.h"

int main(int argc, char **argv) {

	#if defined(SERVER)
		if ( argc < 4 ) {
			usage(argv);
			return -1;
		}
	#elif defined(client)
		if ( argc < 3 ) {
			usage(argv);
			return -1;
		}
	#endif

	int port = atoi(argv[2]);

	if ( port < 1024 ) {
		fprintf(stderr,"Error: Given Port %s has to be above 1023\n",argv[2]);
		usage(argv);
		return -1;
	}
	else if ( port > 65535 ) {
		fprintf(stderr,"Error: Given Port %s has to be below 65536\n",argv[2]);
		usage(argv);
		return -1;
	}

	#if defined(SERVER)
		start_server(argv);
	#elif defined(CLIENT)
		start_client(argv);
	#else
	#error "Must define either -DSERVER or -DCLIENT"
	#endif

	return 0;
}
