#ifndef USLEEP_H
#define USLEEP_H

#ifdef _WIN32

#define sleep(x) Sleep(1000*(x))

void usleep(unsigned int usec);
#endif

#endif