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

JNIEXPORT void JNICALL Java_javademo_jni_AdemcoHbLibrary_testPrint
  (JNIEnv *, jobject)
{
    printf("native print\n");
}

JNIEXPORT jobject JNICALL Java_javademo_jni_AdemcoHbLibrary_parse
(JNIEnv* env, jobject , jstring pack, jint pack_len)
{
    ademco::AdemcoPacket ap;
    size_t commited = 0;
    jboolean iscopy = 0;
    std::string s = env->GetStringUTFChars(pack, &iscopy);
    auto result = ap.parse(s.data(), static_cast<size_t>(pack_len), commited);    
    // Attempt to find the Version class.
    jclass clazz = env->FindClass("javademo/jni/AdemcoHbLibrary$ParseResult");
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

JNIEXPORT jstring JNICALL Java_javademo_jni_AdemcoHbLibrary_pack
(JNIEnv* env, jobject, jint seq, jint ademco_id, jint ademco_event, jint zone, jint gg)
{
    ademco::AdemcoPacket ap;
    char buff[1024];
    auto res = ap.make_hb(buff, sizeof(buff), static_cast<uint16_t>(seq),
                          nullptr, static_cast<size_t>(ademco_id), static_cast<unsigned char>(gg),
                          static_cast<ademco::ADEMCO_EVENT>(ademco_event), static_cast<size_t>(zone));
    if (res > 0) {
        buff[res] = 0;
        return env->NewStringUTF(buff);
    }
    return 0;
}

JNIEXPORT jstring JNICALL Java_javademo_jni_AdemcoHbLibrary_packAck
(JNIEnv* env, jobject, jint seq, jint ademco_id)
{
    ademco::AdemcoPacket ap; 
    char buff[1024];
    auto res = ap.make_ack(buff, sizeof(buff), static_cast<uint16_t>(seq),
                           nullptr, static_cast<size_t>(ademco_id));
    if (res > 0) {
        buff[res] = 0;
        return env->NewStringUTF(buff);
    }
    return 0;
}

