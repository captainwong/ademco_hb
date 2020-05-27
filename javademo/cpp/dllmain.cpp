// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#define DISABLE_JLIB_LOG2
#include <ademco_packet.h>
#include "javademo_jni_AdemcoHbLibrary.h"

#ifdef _WIN32
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif

JNIEXPORT void JNICALL Java_javademo_jni_AdemcoHbLibrary_nativePrint
(JNIEnv*, jobject)
{
    printf("native print\n");
}

JNIEXPORT jobject JNICALL Java_javademo_jni_AdemcoHbLibrary_nativeParse
(JNIEnv* env, jclass clazz, jstring pack, jint pack_len)
{
    ademco::AdemcoPacket ap;
    size_t commited = 0;
    jboolean iscopy = 0;
    std::string s = env->GetStringUTFChars(pack, &iscopy);
    auto result = ap.parse(s.data(), static_cast<size_t>(pack_len), commited);
    //*cb_commited = static_cast<int>(commited);
    //return static_cast<int>(result);
    // Attempt to find the Version class.
    clazz = env->FindClass("javademo/jni/AdemcoHbLibrary$ParseResult");
    // If this class does not exist then return null.
    if (clazz == 0)
        return 0;
    // Allocate memory for a new Version class object.  Do not bother calling
    // the constructor (the default constructor does nothing).
    jobject obj = env->AllocObject(clazz);
    // Attempt to find the major field.
    jfieldID fid = env->GetFieldID(clazz, "result", "I");
    // If this field does not exist then return null.
    if (fid == 0)
        return 0;
    // Set the major field to the operating system's major version.
    env->SetIntField(obj, fid, static_cast<int>(result));
    // Attempt to find the minor field.
    fid = env->GetFieldID(clazz, "cb_commited", "I");
    // If this field does not exist then return null.
    if (fid == 0)
        return 0;
    // Set the minor field to the operating system's minor version.
    env->SetIntField(obj, fid, static_cast<int>(commited));
    return obj;
}

