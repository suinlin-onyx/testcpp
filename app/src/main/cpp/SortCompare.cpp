
#include "SortCompare.h"

EnumAscDescOrder SortCompare::enumAscDescOrder = Asc;

SortCompare::SortCompare() {
}

bool SortCompare::CompareDirName(StatFile &sf1, StatFile &sf2) {
    int len = sf1.strName.length() < sf2.strName.length() ? sf1.strName.length()
                                                          : sf2.strName.length();
    for (int i = 0; i <= len; i++) {
        if (tolower(sf1.strName[i]) != tolower(sf2.strName[i])) {
            switch (enumAscDescOrder) {
                case Asc:
                    return tolower(sf1.strName[i]) < tolower(sf2.strName[i]);
                case Desc:
                    return tolower(sf1.strName[i]) > tolower(sf2.strName[i]);
            }
        }
    }
}

bool SortCompare::CompareName(StatFile &sf1, StatFile &sf2) {
    int len = sf1.strName.length() < sf2.strName.length() ? sf1.strName.length()
                                                          : sf2.strName.length();
    for (int i = 0; i <= len; i++) {
        if (tolower(sf1.strName[i]) != tolower(sf2.strName[i])) {
            switch (enumAscDescOrder) {
                case Asc:
                    return tolower(sf1.strName[i]) < tolower(sf2.strName[i]);
                case Desc:
                    return tolower(sf1.strName[i]) > tolower(sf2.strName[i]);
            }
        }
    }
}

bool SortCompare::CompareSize(StatFile &sf1, StatFile &sf2) {
    switch (enumAscDescOrder) {
        case Asc:
            return sf1.nSize < sf2.nSize;
        case Desc:
            return sf1.nSize > sf2.nSize;
    }
    return false;
}

bool SortCompare::CompareCTime(StatFile &sf1, StatFile &sf2) {
    switch (enumAscDescOrder) {
        case Asc:
            return sf1.nCTime < sf2.nCTime;
        case Desc:
            return sf1.nCTime > sf2.nCTime;
    }
    return false;
}

bool SortCompare::CompareMTime(StatFile &sf1, StatFile &sf2) {
    switch (enumAscDescOrder) {
        case Asc:
            return sf1.nMTime < sf2.nMTime;
        case Desc:
            return sf1.nMTime > sf2.nMTime;
    }
    return false;
}



