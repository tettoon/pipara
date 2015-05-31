#include "pipara.h"
#include "tettoon_pipara_PiParaJni.h"

int initialized = 0;

JNIEXPORT void JNICALL Java_tettoon_pipara_PiParaJni_setup
  (JNIEnv *env, jobject me)
{
    int rc = pipara_setup();
    if (rc != 0)
    {
        jclass clsj = env->FindClass("Ljava/io/IOException;");
        if (clsj == NULL) return;
        char msg[40];
        sprintf(msg, "pipara_setup() returned %d.", rc);
        env->ThrowNew(clsj, msg);
        env->DeleteLocalRef(clsj);
        return;
    }

    initialized = -1;
}

JNIEXPORT void JNICALL Java_tettoon_pipara_PiParaJni_write
  (JNIEnv *env, jobject me, jbyte dataj)
{
    if (initialized == 0)
    {
        jclass clsj = env->FindClass("Ljava/lang/IllegalStateException;");
        if (clsj == NULL) return;
        env->ThrowNew(clsj, "pipara_setup() has not been called.");
        env->DeleteLocalRef(clsj);
        return;
    }

    pipara_write(dataj);
}

