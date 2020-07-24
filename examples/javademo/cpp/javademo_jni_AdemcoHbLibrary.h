/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class javademo_jni_AdemcoHbLibrary */

#ifndef _Included_javademo_jni_AdemcoHbLibrary
#define _Included_javademo_jni_AdemcoHbLibrary
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     javademo_jni_AdemcoHbLibrary
 * Method:    testPrint
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javademo_jni_AdemcoHbLibrary_testPrint
  (JNIEnv *, jobject);

/*
 * Class:     javademo_jni_AdemcoHbLibrary
 * Method:    parse
 * Signature: (Ljava/lang/String;I)Ljavademo/jni/AdemcoHbLibrary/ParseResult;
 */
JNIEXPORT jobject JNICALL Java_javademo_jni_AdemcoHbLibrary_parse
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     javademo_jni_AdemcoHbLibrary
 * Method:    pack
 * Signature: (ILjava/lang/String;IIII)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javademo_jni_AdemcoHbLibrary_pack
  (JNIEnv *, jobject, jint, jstring, jint, jint, jint, jint);

/*
 * Class:     javademo_jni_AdemcoHbLibrary
 * Method:    pack2
 * Signature: (ILjava/lang/String;IIIILjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javademo_jni_AdemcoHbLibrary_pack2
  (JNIEnv *, jobject, jint, jstring, jint, jint, jint, jint, jstring);

/*
 * Class:     javademo_jni_AdemcoHbLibrary
 * Method:    packAck
 * Signature: (ILjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javademo_jni_AdemcoHbLibrary_packAck
  (JNIEnv *, jobject, jint, jstring);

#ifdef __cplusplus
}
#endif
#endif
