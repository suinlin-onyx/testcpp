//
// Created by arvin on 2017/11/10 0010.
//

#include <string>
#include <vector>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>

#include "ScopedUtfChars.h"

#include<android/log.h>

#define TAG    "read test"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)


#ifndef TESTCPP_IOFILE_H
#define TESTCPP_IOFILE_H

using namespace std;

class IOFile {

public:
    IOFile();
public:
    static bool readDirectory(JNIEnv *env, jstring javaPath, std::vector<std::string> &entries);

};


#endif //TESTCPP_IOFILE_H
