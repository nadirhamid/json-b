#ifndef JSONB_PARSER_JAVA_H
#define JSONB_PARSER_JAVA_H 1
#include <jni.h>
#include "jsonb.h"

jobject create_jobject_from_jsonb(JNIEnv* env, jclass target, struct value_array* values);

jint throw_jsonb_parser_ex( JNIEnv *env, const char *message );

JNIEXPORT jobject JNICALL Java_com_infinitet3ch_jsonb_Reflection_fromJSONB(JNIEnv *, jobject, jstring, jclass, jobjectArray);

JNIEXPORT jobjectArray JNICALL Java_com_infinitet3ch_jsonb_Reflection_fromJSONBArray(JNIEnv *, jobject, jstring, jclass, jobjectArray);

JNIEXPORT jstring JNICALL Java_com_infinitet3ch_jsonb_Reflection_toJSONB(JNIEnv *, jobject, jobject, jclass, jobjectArray);

JNIEXPORT jstring JNICALL Java_com_infinitet3ch_jsonb_Reflection_toJSONBArray(JNIEnv *, jobject, jobjectArray, jclass, jobjectArray);

#endif

