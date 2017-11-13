

#ifndef SCOPED_UTF_CHARS_H_included
#define SCOPED_UTF_CHARS_H_included

#include <string.h>
#include <jni.h>

class ScopedUtfChars {
public:
    ScopedUtfChars(JNIEnv* env, jstring s)
    : mEnv(env), mString(s)
    {
        if (s == NULL) {
            mUtfChars = NULL;
        } else {
            mUtfChars = env->GetStringUTFChars(s, NULL);
        }
    }

    ~ScopedUtfChars() {
        if (mUtfChars) {
            mEnv->ReleaseStringUTFChars(mString, mUtfChars);
        }
    }

    const char* c_str() const {
        return mUtfChars;
    }

    size_t size() const {
        return strlen(mUtfChars);
    }

    const char& operator[](size_t n) const {
        return mUtfChars[n];
    }

private:
    JNIEnv* mEnv;
    jstring mString;
    const char* mUtfChars;

    // Disallow copy and assignment.
    ScopedUtfChars(const ScopedUtfChars&);
    void operator=(const ScopedUtfChars&);
};

#endif  // SCOPED_UTF_CHARS_H_included
