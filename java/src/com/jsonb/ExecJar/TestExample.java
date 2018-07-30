package com.jsonb.ExecJar;

class TestExample {
	public static void main(String[] args) {
		// Obtain the class object if we know the name of the class
		Class exampleClass = JSONBExample.class;
		try {
			int i;
			JSONBExample obj = new JSONBExample();
			String str = "";
			Reflection reflection = new Reflection();
			String parseStr = "{\"name\":\"test\",\"example\":\"12345\",\"example123\":null,\"example789\":null,\"number\":123, \"on\": true}";
			System.out.println("fromJSONB input "  + parseStr);
			for ( i = 0; i != 10; i ++ ) {
				obj = (JSONBExample) reflection.fromJSONB(parseStr, exampleClass);
				System.out.println("fromJSONB obj.name = " + obj.name + " obj.number = "  + obj.number + " obj.on " + obj.on);
			}
			JSONBExample obj2 = new JSONBExample();
			for ( i = 0; i != 10; i ++ ) {
				str = reflection.toJSONB(obj);
				System.out.println("toJSONB result = " + str);
			}


			JSONBExample obj3 = (JSONBExample) reflection.fromJSONB(str, exampleClass);
			System.out.println("fromJSONB obj.name = " + obj3.name);
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		}
	}

}

