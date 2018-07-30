#include "java.h"

jobject create_jobject_from_jsonb(JNIEnv* env, jclass cls, struct value_array* values)
{
	jfieldID fid;	
	jobject obj = jsonb_create_java_object_from_class(env, cls);
	struct value_dynamic*  ptr = values->head;
	while (ptr != NULL) {
		switch (ptr->type) {
			case JSONB_PARSER_TYPE_INT:
				//set the field value for obj
				printf("JNI casting %s to int\r\n", ptr->key);
				fid = (*env)->GetFieldID(env, cls, ptr->key, "I");
				(*env)->SetIntField(env, obj, fid, ptr->int_value /** int **/);
			break;
			case JSONB_PARSER_TYPE_FLOAT:
				printf("JNI casting %s to float\r\n", ptr->key);
				fid = (*env)->GetFieldID(env, cls, ptr->key, "F");
				(*env)->SetFloatField(env, obj, fid, ptr->float_value /** int **/);

			break;
			case JSONB_PARSER_TYPE_STR:
				printf("JNI casting %s to string value: %s\r\n", ptr->key, ptr->str_value);
				jstring jstr = (*env)->NewStringUTF(env, ptr->str_value);
				fid = (*env)->GetFieldID(env, cls, ptr->key, "Ljava/lang/String;");
				(*env)->SetObjectField(env, obj, fid, jstr);
			break;
            case JSONB_PARSER_TYPE_NULL:
			break;
            case JSONB_PARSER_TYPE_BOOLEAN:
            	printf("JNI casting %s to boolean value: %d\r\n", ptr->key, ptr->jboolean_value);
				fid = (*env)->GetFieldID(env, cls, ptr->key, "Z");
				(*env)->SetBooleanField(env, obj, fid, ptr->jboolean_value);

			break;

			case JSONB_PARSER_TYPE_ARRAY:
			break;
			case JSONB_PARSER_TYPE_OBJECT:
			break;
		}
		ptr = ptr->next;
	}
	return obj;

}
JNIEXPORT jobject JNICALL Java_com_jsonb_ExecJar_Reflection_fromJSONB(JNIEnv* env, jobject obj, jstring jstr, jclass target, jobjectArray fields)
{
	const char* str = (*env)->GetStringUTFChars(env, jstr, 0);
	struct parser* parser;
	jsonb_generic_parse(&parser, str);
	(*env)->ReleaseStringUTFChars(env, jstr, str);
	jobject output = create_jobject_from_jsonb(env, target, parser->values);
	jsonb_cleanup_parser(parser);
	return output;
}

JNIEXPORT jobjectArray JNICALL Java_com_jsonb_ExecJar_Reflection_fromJSONBArray(JNIEnv* env, jobject obj, jstring jstr, jclass target, jobjectArray fields)
{
	const char* str = (*env)->GetStringUTFChars(env, jstr, 0);
	struct parser* parser;
       	jsonb_generic_parse(&parser, str);
	// default value
	jobject default_value = jsonb_create_java_object_from_class( env, target );	
	jobjectArray arr = (*env)->NewObjectArray(env, parser->values->len, target, default_value);
 	struct value_dynamic* ptr = parser->values->current;
	int index;
	while (ptr!=NULL) {
		jobject obj = create_jobject_from_jsonb(env, target,ptr->object_value->values);
		(*env)->SetObjectArrayElement(env, arr, index, obj);
		index++;
		ptr = ptr->next;
	}
	jsonb_cleanup_parser(parser);
	return arr;
}

JNIEXPORT jstring JNICALL Java_com_jsonb_ExecJar_Reflection_toJSONB(JNIEnv* env, jobject obj, jobject target, jclass cls, jobjectArray fields)
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

JNIEXPORT jstring JNICALL Java_com_jsonb_ExecJar_Reflection_toJSONBArray(JNIEnv* env, jobject obj, jobjectArray target, jclass cls, jobjectArray fields)
{
	struct writer writer;
	jsonb_write(env, fields, target, cls, &writer);
	jstring jstr = (*env)->NewStringUTF(env, writer.output);
	return jstr;
}


