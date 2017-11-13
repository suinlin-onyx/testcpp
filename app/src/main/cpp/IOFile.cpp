//
// Created by arvin on 2017/11/10 0010.
//

#include "IOFile.h"

IOFile::IOFile(){
};

class ScopedReaddir {
public:
    ScopedReaddir(const char* path) {
        mDirStream = opendir(path);
        mIsBad = (mDirStream == NULL);
    }

    ~ScopedReaddir() {
        if (mDirStream != NULL) {
            closedir(mDirStream);
        }
    }

    // Returns the next filename, or NULL.
    const char* next() {
        if (mIsBad) {
            return NULL;
        }
        dirent* result = NULL;
        int rc = readdir_r(mDirStream, &mEntry, &result);
        if (rc != 0) {
            mIsBad = true;
            return NULL;
        }
        return (result != NULL) ? result->d_name : NULL;
    }

    // Has an error occurred on this stream?
    bool isBad() const {
        return mIsBad;
    }

private:
    DIR* mDirStream;
    dirent mEntry;
    bool mIsBad;

    // Disallow copy and assignment.
    ScopedReaddir(const ScopedReaddir&);
    void operator=(const ScopedReaddir&);
};


bool IOFile::readDirectory(JNIEnv *env, jstring javaPath, std::vector<std::string> &entries) {
    ScopedUtfChars path(env, javaPath);
    if (path.c_str() == NULL) {
        return false;
    }

    ScopedReaddir dir(path.c_str());
    const char *filename;
    while ((filename = dir.next()) != NULL) {
        if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0) {
            // TODO: this hides allocation failures from us. Push directory iteration up into Java?
            entries.push_back(filename);
        }
    }
    return !dir.isBad();
}