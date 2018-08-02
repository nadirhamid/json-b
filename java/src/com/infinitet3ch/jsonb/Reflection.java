package com.infinitet3ch.jsonb;
import java.lang.reflect.*;
import java.util.Arrays;

public class Reflection {
	public int myVar;
	Reflection() {
		System.loadLibrary("jsonb");
	}
	public native Object fromJSONB(String parseStr, Class cls, String[] fields) throws ParserException;
	public native Object[] fromJSONBArray(String parseStr, Class cls, String[] fields) throws ParserException;
	public native String toJSONB(Object obj, Class cls, String[][] fields);
	public native String toJSONBArray(Object[] objs, Class cls, String[][] fields);

	private String[] getFields(Class cls)
	{
		Field[] reflectFields = cls.getFields();	
		String[] fields = new String[ reflectFields.length ];
		for ( int i = 0; i != reflectFields.length; i ++ ) {
			fields[ i ] = reflectFields[ i ].getName();
		}
		return fields;
	}
	private String[][] makeWriterFields(Class cls)
	{
		Field[] reflectFields = cls.getFields();
		String[][] fields = new String[ reflectFields.length ][ 2 ];
		for ( int i = 0; i != reflectFields.length; i ++ ) {
			Type type = reflectFields[i].getGenericType();
			String sig = this.getJNISignature(type);
			fields[i][0] = reflectFields[ i ].getName();
			fields[i][1] = sig;
		}
		return fields;
	}
	private String getJNISignature(Type type)
	{
		String name = type.getTypeName();
		if ( name == "int" ) {
			return "I";
		} else if ( name == "float" ) {
			return "F";
		} else if ( name == "boolean" ) {
			return "Z";
		} else if ( name == "java.lang.String" ) {
			return "Ljava/lang/String;";
		}
		return null;
	}
	public Object fromJSONB(String parseStr, Class cls) throws ParserException
	{
		String[] fields = this.getFields(cls);
		return this.fromJSONB(parseStr, cls, fields);
	}
	public Object[] fromJSONBArray(String parseStr, Class cls) throws ParserException
	{
		String[] fields = this.getFields(cls);
		return this.fromJSONBArray(parseStr, cls, fields);
	}
	public String toJSONB(Object obj)		
	{
		return this.toJSONB(obj, obj.getClass());
	}
	public String toJSONB(Object obj, Class cls)
	{
		String[][] fields = this.makeWriterFields(cls);
		return this.toJSONB(obj, cls, fields);
	}
	public String toJSONBArray(Object[] objs, Class cls)
	{
		String[][] fields = this.makeWriterFields(cls);
		return this.toJSONBArray(objs, cls, fields);
	}
}
