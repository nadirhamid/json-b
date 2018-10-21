package com.infinitet3ch.jsonb;

class Dog {
    public String name;
    public int age;
    public boolean bitable;
    Dog() {
    }
}

class TestExample {
    public static void dump(Dog obj, String heading)
    {
        System.out.println("fromJSONB " + heading + " dog.name = " + obj.name + " dog.age = " + obj.age + " dog.bitable = " + obj.bitable);
    }
    public static void dumpObject(Dog obj)
    {
        TestExample.dump( obj, "" );
    }
    public static void dumpObjectIndex(Dog obj, int index) {
        TestExample.dump( obj, "index(" + index + ")" );
    }
	public static void main(String[] args) {
		// Obtain the class object if we know the name of the class
		Class exampleClass = Dog.class;
		try {
			int i;
            int j;
			String str = "";
      System.out.println("starting JSON-B demonstration..");
			Reflection reflection = new Reflection();
            String parseObjStr = "{\"name\": \"Falco\", \"age\": 4, \"bitable\": false}";
			String parseObjArrStr = "[{\"name\": \"Falco\", \"age\": 4, \"bitable\": false}, {\"name\": \"Cassidy\", \"age\": 5, \"bitable\": false}]";
            String parsedObjStr = "";
            String parsedObjArrStr = "";
            Dog[] objs;
            Dog obj;
            Dog[] reparsedObjs;
            Dog reparsedObj;

			for ( i = 0; i != 10; i ++ ) {
                System.out.println("test trial (" + i + ")");
                obj = (Dog) reflection.fromJSONB(parseObjStr, exampleClass);
                TestExample.dumpObject( obj );
                objs = (Dog[]) reflection.fromJSONBArray(parseObjArrStr, exampleClass);
                for (j = 0; j != objs.length; j ++ ) {
                    obj= objs[ j ];
                    TestExample.dumpObjectIndex( obj, j );
                }
                for ( j= 0; j != 10; j ++ ) {
                    parsedObjStr = reflection.toJSONB(objs[0]);
                    System.out.println("toJSONB result = " + parsedObjStr);
                    parsedObjArrStr = reflection.toJSONBArray(objs, exampleClass);
                    System.out.println("toJSONBArray result = " + parsedObjArrStr);
                }
                reparsedObj = (Dog) reflection.fromJSONB(parsedObjStr, exampleClass);
			    TestExample.dumpObject( reparsedObj );
                reparsedObjs = (Dog[]) reflection.fromJSONBArray(parsedObjArrStr, exampleClass);
			    for ( j = 0; j != reparsedObjs.length; j ++ ) {
                    obj = reparsedObjs[ j ];
                    TestExample.dumpObjectIndex( obj, j );
               }
            }
        } catch (ParserException e) {
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
    }
	}

}

