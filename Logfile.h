#ifndef LOGFILE_H_INCLUDED
#define LOGFILE_H_INCLUDED

#include <stdio.h>

extern FILE *LOG_FILE;

void close_log();

void open_log(const char *filename, const char *time);

#endif // LOGFILE_H_INCLUDED
