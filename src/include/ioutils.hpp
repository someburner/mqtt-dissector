#ifndef IOUTILS__hpp
#define IOUTILS__hpp

#include <sys/types.h>
#include <sys/stat.h>

static inline bool dirExists(const char * path) {
    struct stat info;
    if( stat( path, &info ) != 0 ) {
        printf( "cannot access %s\n", path );
    }
    else if( info.st_mode & S_IFDIR ) {
        return true;
    } else {
        printf( "%s is not a directory\n", path );
    }
    return false;
}

#endif /* IOUTILS__hpp */
