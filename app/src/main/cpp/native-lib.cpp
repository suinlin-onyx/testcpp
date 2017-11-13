#include <jni.h>
#include <string>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include "SortCompare.h"
#include "IOFile.h"

#define TAG    "read test"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

using namespace std;
typedef struct stat Stat_t;
vector<StatFile> listOfFileData;
vector<StatFile> listOfDirData;
vector<string> filePaths;
vector<string> filePathsFromJava;

static std::string const enum_sort_names[] = {"None", "Name", "Size", "CreationTime", "ModifyTime"};
static std::string const enum_ado_names[] = {"Asc", "Desc"};

EnumSort charToSortEnum(char *ch) {
    int size = sizeof(enum_sort_names) / sizeof(enum_sort_names[0]);
    for (int i = 0; i < size; i++) {
        if (strcmp(ch, enum_sort_names[i].c_str()) == 0) {
            return (EnumSort) i;
        }
    }
    return None;
}

EnumAscDescOrder charToAdoEnum(char *ch) {
    for (int i = 0; i < sizeof(enum_ado_names) / sizeof(enum_ado_names[0]); i++) {
        if (strcmp(ch, enum_ado_names[i].c_str()) == 0) {
            return (EnumAscDescOrder) i;
        }
    }
    return Asc;
}

void LoadFiles(const char *rootPath) {
    DIR *dir;
    dir = opendir(rootPath);

    if (dir != NULL) {
        dirent *currentDir;
        while ((currentDir = readdir(dir)) != NULL) {


            if (DT_DIR & currentDir->d_type) {
                if (strcmp(currentDir->d_name, ".") == 0 || strcmp(currentDir->d_name, "..") == 0) {
                    continue;
                }
                StatFile stfile;
                stfile.path.append(rootPath);
                stfile.path.append("/");
                stfile.path.append(currentDir->d_name);
                //LoadFiles(strpath.c_str());
                stfile.strName = currentDir->d_name;
                listOfDirData.push_back(stfile);
            } else {
                StatFile st_file;
                st_file.path = rootPath;
                st_file.path.append("/");
                st_file.path.append(currentDir->d_name);

                Stat_t buf;
                memset(&buf, 0, sizeof(buf));
                if (stat(st_file.path.c_str(), &buf) >= 0 && !S_ISDIR(buf.st_mode)) {
                    st_file.strName = currentDir->d_name;
                    st_file.nSize = buf.st_size;
                    st_file.nCTime = buf.st_ctime;
                    st_file.nMTime = buf.st_mtime;
                    listOfFileData.push_back(st_file);
                }
            }

        }
        closedir(dir);
    }
}

void initFiles() {
    listOfFileData.clear();
    listOfDirData.clear();

    int size = filePathsFromJava.size();
    int i = 0;
    for (i = 0; i < size; i++) {
        StatFile st_file;
        string &str = filePathsFromJava[i];
        string strSub = str.substr(str.find_last_of("/") + 1);
        st_file.path = str;
        st_file.strName = strSub;

        const char *show = strstr(strSub.c_str(), ".");
        if (show != NULL) {
            listOfFileData.push_back(st_file);
        } else {
            listOfDirData.push_back(st_file);
        }
    }
}

jobjectArray getFileArray(JNIEnv *env, jint startOffset, jint endOffset) {
    LOGD("%s", "getFileArray");
    int fileSize = listOfFileData.size();
    int dirSize = listOfDirData.size();
    int start = 0, end = 0;
    if (startOffset < 0 || endOffset < 0) {
        end = fileSize;
    } else {
        if (startOffset > fileSize) {
            start = fileSize;
        } else {
            start = startOffset;
        }
        if (endOffset > fileSize) {
            end = fileSize;
        } else {
            end = endOffset;
        }
    }
    int nSize = dirSize + end - start;


    jclass objArray = env->FindClass("[Ljava/lang/String");
    jobjectArray fileDatas = env->NewObjectArray(nSize, objArray, NULL);

    jclass stringClass = env->FindClass("Ljava/lang/String");
    for (int i = 0; i < dirSize; i++) {
        jobjectArray fileData = env->NewObjectArray((jsize) 2, stringClass, 0);

        jstring strPath = env->NewStringUTF(listOfDirData[i].path.c_str());
        jstring strName = env->NewStringUTF(listOfDirData[i].strName.c_str());

        env->SetObjectArrayElement(fileData, 0, strPath);
        env->SetObjectArrayElement(fileData, 1, strName);

        LOGD("dir", "%d", i);
        //delete object
        // strPath
        // strName
        env->DeleteLocalRef(strPath);
        env->DeleteLocalRef(strName);

        env->SetObjectArrayElement(fileDatas, i, fileData);
        env->DeleteLocalRef(fileData);
    }

    for (int i = start; i < end; i++) {
        jobjectArray fileData = env->NewObjectArray((jsize) 2, stringClass, 0);
        jstring strPath = env->NewStringUTF(listOfFileData[i].path.c_str());
        jstring strName = env->NewStringUTF(listOfFileData[i].strName.c_str());

        env->SetObjectArrayElement(fileData, 0, strPath);
        env->SetObjectArrayElement(fileData, 1, strName);
        LOGD("%d", i);
        env->DeleteLocalRef(strPath);
        env->DeleteLocalRef(strName);

        LOGD("%d", i - start + dirSize);
        env->SetObjectArrayElement(fileDatas, i - start + dirSize, fileData);
        LOGD("%s", "SetObjectArrayElement");

        env->DeleteLocalRef(fileData);

//        jstring newStr = env->NewStringUTF(listOfFileData[i].path.c_str());
//        env->SetObjectArrayElement(fileDatas, i - start + dirSize, newStr);
//        env->DeleteLocalRef(newStr);
    }


    return fileDatas;
}


extern "C"
JNIEXPORT jobjectArray
JNICALL
Java_com_qq_myapplication_MainActivity_initFilePaths(
        JNIEnv *env,
        jobject object,
        jobjectArray strPaths) {
    jsize size = env->GetArrayLength(strPaths);
    filePathsFromJava.clear();

    for (int i = 0; i < size; i++) {
        jstring job = (jstring) env->GetObjectArrayElement(strPaths, i);
        char *pData = (char *) env->GetStringUTFChars(job, 0);
        filePathsFromJava.push_back(pData);
        env->ReleaseStringUTFChars(job, pData);
        env->DeleteLocalRef(job);
    }

    initFiles();

    sort(listOfFileData.begin(), listOfFileData.end(), SortCompare::CompareName);
    sort(listOfDirData.begin(), listOfDirData.end(), SortCompare::CompareDirName);

    return getFileArray(env, -1, -1);
}

extern "C"
JNIEXPORT jobjectArray
JNICALL
Java_com_qq_myapplication_MainActivity_getFilePaths(
        JNIEnv *env,
        jobject object,
        jstring strPath,
        jstring sortBy,
        jstring orderBy,
        jint startOffset,
        jint endOffset) {

    listOfFileData.clear();
    listOfDirData.clear();
    filePaths.clear();


    const char *chars = env->GetStringUTFChars(strPath, NULL);
    LoadFiles(chars);
    env->ReleaseStringUTFChars(strPath, chars);

    EnumSort enumSort;
    {
        const char *sorts = env->GetStringUTFChars(sortBy, 0);
        enumSort = charToSortEnum((char *) sorts);
        env->ReleaseStringUTFChars(sortBy, sorts);
    }
    EnumAscDescOrder order;
    {
        const char *sorts = env->GetStringUTFChars(orderBy, 0);
        order = charToAdoEnum((char *) sorts);
        env->ReleaseStringUTFChars(orderBy, sorts);
    }

    SortCompare::enumAscDescOrder = order;
    switch (enumSort) {
        case None:
        case Name:
            sort(listOfFileData.begin(), listOfFileData.end(), SortCompare::CompareName);
            break;
        case Size:
            sort(listOfFileData.begin(), listOfFileData.end(), SortCompare::CompareSize);
            break;
        case CreationTime:
            sort(listOfFileData.begin(), listOfFileData.end(), SortCompare::CompareCTime);
            break;
        case ModifyTime:
            sort(listOfFileData.begin(), listOfFileData.end(), SortCompare::CompareMTime);
            break;
        default:
            sort(listOfFileData.begin(), listOfFileData.end(), SortCompare::CompareName);
            break;
    }

    sort(listOfDirData.begin(), listOfDirData.end(), SortCompare::CompareDirName);

    return getFileArray(env, startOffset, endOffset);
}