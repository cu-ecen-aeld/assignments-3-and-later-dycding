#include <syslog.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    openlog("writer", LOG_PID, LOG_USER);
    if (argc != 3) {
	syslog(LOG_ERR, "Incorrect number of input arguments");
	return 1;
    }

    const char *writefile = argv[1];
    const char *writestr = argv[2];
    int fd;
    ssize_t nr;
    fd = open(writefile, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    
    if (fd == -1) {
	syslog(LOG_ERR, "Cannot open %s\n", writefile);
	return 1;
    }
    
    nr = write(fd, writestr, strlen(writestr));
    if (nr == -1) {
	syslog(LOG_ERR, "Cannot write to %s\n", writefile);
	return 1;
    } else {
    	syslog(LOG_DEBUG, "Writing '%s' to '%s'", writestr, writefile);
    	return 0;
    }
}
