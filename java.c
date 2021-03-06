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
#include "java.h"
void process_value_type( struct value_dynamic* ptr, JNIEnv* env, jclass cls, jobject obj)
{
  jfieldID fid;  
  if (ptr->type == JSONB_PARSER_TYPE_INT) {
    //set the field value for obj
    jsonb_debugf("JNI casting %s to int\r\n", ptr->key);
    fid = (*env)->GetFieldID(env, cls, ptr->key, "I");
    (*env)->SetIntField(env, obj, fid, ptr->int_value /** int **/);
  } else if (ptr->type == JSONB_PARSER_TYPE_FLOAT) {
    jsonb_debugf("JNI casting %s to float\r\n", ptr->key);
    fid = (*env)->GetFieldID(env, cls, ptr->key, "F");
    (*env)->SetFloatField(env, obj, fid, ptr->float_value /** int **/);
  } else if (ptr->type == JSONB_PARSER_TYPE_STR) {
    jstring jstr;
    jsonb_debugf("JNI casting %s to string \r\n", ptr->key);
    fid = (*env)->GetFieldID(env, cls, ptr->key, "Ljava/lang/String;");
    jstr = (*env)->NewStringUTF(env, ptr->str_value);
    (*env)->SetObjectField(env, obj, fid, jstr);
    free( ptr->str_value );
  } else if (ptr->type == JSONB_PARSER_TYPE_NULL) {
    jsonb_debugf("JNI casting %s to null\r\n", ptr->key);
  } else if (ptr->type == JSONB_PARSER_TYPE_BOOLEAN) {
    jsonb_debugf("JNI casting %s to boolean value: %d\r\n", ptr->key, ptr->jboolean_value);
    fid = (*env)->GetFieldID(env, cls, ptr->key, "Z");
    (*env)->SetBooleanField(env, obj, fid, ptr->jboolean_value);
  } else if (ptr->type == JSONB_PARSER_TYPE_ARRAY) {
  } else if (ptr->type == JSONB_PARSER_TYPE_OBJECT) {
  }
}

jobject create_jobject_from_jsonb(JNIEnv* env, jclass cls, struct value_array* values)
{
  jobject obj = jsonb_create_java_object_from_class(env, cls);
  struct value_dynamic*  ptr = values->head;
  while (ptr != NULL) {
    process_value_type( ptr, env, cls, obj );
    free( ptr->key );
    ptr = ptr->next;
  }

  return obj;
}

jint throw_jsonb_parser_ex( JNIEnv *env, const char *message )
{
  jclass exClass;
  char *className = "com/infinitet3ch/jsonb/ParserException";

  exClass = (*env)->FindClass( env, className );
  return (*env)->ThrowNew( env, exClass, message );
}


JNIEXPORT jobject JNICALL Java_com_infinitet3ch_jsonb_Reflection_fromJSONB(JNIEnv* env, jobject obj, jstring jstr, jclass target, jobjectArray fields)
{
  const char* str = (*env)->GetStringUTFChars(env, jstr, 0);
  struct parser* parser;
  jobject output;
  jint ex;
  jsonb_parse_from_str(&parser, str);
  (*env)->ReleaseStringUTFChars(env, jstr, str);

  if (parser->code == JSONB_READ_ERROR) {
    jsonb_debugf("JSONB parser resulted in error\r\n");
    ex =  throw_jsonb_parser_ex( env, parser->error );
    jsonb_cleanup_parser(parser);
    return output;
  }

  output = create_jobject_from_jsonb(env, target, parser->values);
  jsonb_cleanup_parser(parser);
  return output;
}

JNIEXPORT jobjectArray JNICALL Java_com_infinitet3ch_jsonb_Reflection_fromJSONBArray(JNIEnv* env, jobject obj, jstring jstr, jclass target, jobjectArray fields)
{
  const char* str = (*env)->GetStringUTFChars(env, jstr, 0);
  struct parser* parser;
  jobjectArray arr;
  jint ex;
  jsonb_parse_from_str(&parser, str);
  (*env)->ReleaseStringUTFChars(env, jstr, str);
  if (parser->code == JSONB_READ_ERROR ) {
    ex =  throw_jsonb_parser_ex( env, parser->error );
    return arr;
  }
  // default value
  jobject default_value = jsonb_create_java_object_from_class( env, target );  
  arr = (*env)->NewObjectArray(env, parser->values->len, target, default_value);
  struct value_dynamic* ptr = parser->values->head;
  int index = 0;
  jobject arrobj;
  jsonb_debugf("fromJSONBArray length of values: %d, parser type: %d\r\n", parser->values->len, parser->type);
  while ( ptr != NULL ) {
    jsonb_debugf("fromJSONBArray adding object at index %d\r\n", index);
    arrobj = create_jobject_from_jsonb(env, target, ptr->object_value->values);
    (*env)->SetObjectArrayElement(env, arr, index, arrobj);
    ptr = ptr->next;
    index ++;
  }
  jsonb_cleanup_parser(parser);
  return arr;
}

JNIEXPORT jstring JNICALL Java_com_infinitet3ch_jsonb_Reflection_toJSONB(JNIEnv* env, jobject obj, jobject target, jclass cls, jobjectArray fields)
{
  struct writer writer;
  jobjectArray objs;
  jsize len = 1;
  objs = (*env)->NewObjectArray(env, len, cls, NULL);
  (*env)->SetObjectArrayElement(env, objs, 0, target);
  jsonb_write_obj(env, fields, objs, cls, &writer);
  jstring jstr = (*env)->NewStringUTF(env, writer.output);

  return jstr;
}

JNIEXPORT jstring JNICALL Java_com_infinitet3ch_jsonb_Reflection_toJSONBArray(JNIEnv* env, jobject obj, jobjectArray target, jclass cls, jobjectArray fields)
{
  struct writer writer;
  jsonb_write(env, fields, target, cls, &writer);
  jstring jstr = (*env)->NewStringUTF(env, writer.output);
  return jstr;
}
