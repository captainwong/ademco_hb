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
(JNIEnv* env, jobject, jint seq, jstring acct, jint ademco_id, jint ademco_event, jint zone, jint gg)
{
    ademco::AdemcoPacket ap;
    char buff[1024];
    jboolean iscopy = 0;
    std::string sacct = env->GetStringUTFChars(acct, &iscopy);
    auto res = ap.make_hb(buff, sizeof(buff), static_cast<uint16_t>(seq),
                          sacct, static_cast<size_t>(ademco_id), static_cast<unsigned char>(gg),
                          static_cast<ademco::ADEMCO_EVENT>(ademco_event), static_cast<size_t>(zone));
    if (res > 0) {
        buff[res] = 0;
        return env->NewStringUTF(buff);
    }
    return 0;
}

JNIEXPORT jstring JNICALL Java_javademo_jni_AdemcoHbLibrary_pack2
(JNIEnv* env, jobject, jint seq, jstring acct, jint ademco_id, jint ademco_event, jint zone, jint gg, jstring xdata)
{
    jboolean iscopy = 0;
    std::string sacct = env->GetStringUTFChars(acct, &iscopy);
    std::string s = env->GetStringUTFChars(xdata, &iscopy);
    auto xdata_ = ademco::makeXData(s.data(), s.size());
    ademco::AdemcoPacket ap;
    char buff[1024];
    auto res = ap.make_hb(buff, sizeof(buff), static_cast<uint16_t>(seq),
                          sacct, static_cast<size_t>(ademco_id), static_cast<unsigned char>(gg),
                          static_cast<ademco::ADEMCO_EVENT>(ademco_event), static_cast<size_t>(zone), xdata_);
    if (res > 0) {
        //buff[res] = 0;
        auto ascii = ademco::detail::toString(buff, res, ademco::detail::ToStringOption::ALL_CHAR_AS_HEX, false, false);
        memcpy(buff, ascii.data(), ascii.size());
        return env->NewStringUTF(buff);
    }
    return 0;
}

JNIEXPORT jcharArray JNICALL Java_javademo_jni_AdemcoHbLibrary_pack3
(JNIEnv* env, jobject, jint seq, jstring acct, jint ademco_id, jint ademco_event, jint zone, jint gg, jcharArray xdata, jint xdata_len)
{
    jboolean iscopy = 0;
    std::string sacct = env->GetStringUTFChars(acct, &iscopy);

    jchar *array = env->GetCharArrayElements(xdata, nullptr); //复制数组元素到array内存空间  
    if(array == nullptr){  
        return 0;  
    }

    jboolean *buf = (jboolean *)calloc(xdata_len , sizeof(jboolean)); //开辟jboolean类型的内存空间，jboolean对应的c++类型为unsigned char  
    if(buf == nullptr){  
        return 0;  
    }
    for(int i=0; i < xdata_len; i++){ //把jcharArray的数据元素复制到buf所指的内存空间  
        *(buf + i) = (jboolean)(*(array + i));  
    }  
  
    env->ReleaseCharArrayElements(xdata, array, 0);//释放资源  
    auto xdata_ = ademco::makeXData(buf, xdata_len);
    
    free(buf);//释放内存空间  
    buf = nullptr;  

    ademco::AdemcoPacket ap;
    char buff[1024] = {0};
    auto res = ap.make_hb(buff, sizeof(buff), static_cast<uint16_t>(seq),
                          sacct, static_cast<size_t>(ademco_id), static_cast<unsigned char>(gg),
                          static_cast<ademco::ADEMCO_EVENT>(ademco_event), static_cast<size_t>(zone), xdata_);
    if (res > 0) {
        //buff[res] = 0;
        auto ascii = ademco::detail::toString(buff, res, ademco::detail::ToStringOption::ALL_CHAR_AS_HEX, false, false);
        memcpy(buff, ascii.data(), ascii.size());
        //return env->NewStringUTF(buff);

        jcharArray rarray = env->NewCharArray(ascii.size());//定义数据变量  
        if(rarray == nullptr){
            return 0;
        }
        
        jchar *pArray = (jchar*)calloc(ascii.size(), sizeof(jchar));//开辟jchar类型内存空间
        if(pArray == nullptr){
           return 0;
        }
        //copy buffer to jchar array
        for(int i = 0; i < ascii.size(); i++) {
            *(pArray + i) = buff[i]; //复制bufTemp数据元素到pArray内存空间	
        }
        env->SetCharArrayRegion(rarray, 0, ascii.size(), pArray);//复制pArray的jchar数据元素到jcharArray 
        return rarray;
    }
    return 0;
}

JNIEXPORT jstring JNICALL Java_javademo_jni_AdemcoHbLibrary_packAck
(JNIEnv* env, jobject, jint seq, jstring acct)
{
    ademco::AdemcoPacket ap; 
    char buff[1024];
    jboolean iscopy = 0;
    std::string s = env->GetStringUTFChars(acct, &iscopy);
    auto res = ap.make_ack(buff, sizeof(buff), static_cast<uint16_t>(seq),
                           s, 0);
    if (res > 0) {
        buff[res] = 0;
        return env->NewStringUTF(buff);
    }
    return 0;
}

