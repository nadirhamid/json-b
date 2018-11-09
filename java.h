/**
 * Copyright 2018 infinitet3ch
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/
#ifndef JSONB_PARSER_JAVA_H
#define JSONB_PARSER_JAVA_H 1
#include <jni.h>
#include "jsonb.h"

jobject create_jobject_from_jsonb(JNIEnv* env, jclass target, struct value_array* values);

jint throw_jsonb_parser_ex( JNIEnv *env, const char *message );

void process_value_type(struct value_dynamic* ptr, JNIEnv* env, jclass cls, jobject obj);

JNIEXPORT jobject JNICALL Java_com_infinitet3ch_jsonb_Reflection_fromJSONB(JNIEnv *, jobject, jstring, jclass, jobjectArray);

JNIEXPORT jobjectArray JNICALL Java_com_infinitet3ch_jsonb_Reflection_fromJSONBArray(JNIEnv *, jobject, jstring, jclass, jobjectArray);

JNIEXPORT jstring JNICALL Java_com_infinitet3ch_jsonb_Reflection_toJSONB(JNIEnv *, jobject, jobject, jclass, jobjectArray);

JNIEXPORT jstring JNICALL Java_com_infinitet3ch_jsonb_Reflection_toJSONBArray(JNIEnv *, jobject, jobjectArray, jclass, jobjectArray);

#endif

