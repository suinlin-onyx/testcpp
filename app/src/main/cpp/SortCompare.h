
#include <string>

using namespace std;

#ifndef TESTCPP_FUNCOMPARE_H
#define TESTCPP_FUNCOMPARE_H
#define TAG    "read test"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

typedef struct _stat_file {
    string path;
    string strName;
    long long nCTime;
    long long nMTime;
    long nSize;
} StatFile, *pStatFile;

typedef enum  {
    None, Name, Size, CreationTime, ModifyTime
}EnumSort;

typedef enum  {
    Asc, Desc
}EnumAscDescOrder;

class SortCompare {

public:
    SortCompare();
public:
    static EnumAscDescOrder enumAscDescOrder;
public:
    static bool CompareDirName(StatFile &sf1, StatFile &sf2);
    static bool CompareName(StatFile &sf1, StatFile &sf2);
    static bool CompareSize(StatFile &sf1, StatFile &sf2);
    static bool CompareCTime(StatFile &sf1, StatFile &sf2);
    static bool CompareMTime(StatFile &sf1, StatFile &sf2);
    static void getTime(unsigned  long time, char *pTime, int leng);
};
#endif //TESTCPP_FUNCOMPARE_H
