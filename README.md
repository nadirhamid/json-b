# JSON-B C wrapper

a native C11 based JSON-B parser/writer. supports fromJSONB and toJSONB for
single objects and collections. 

## pre-requisites

please make sure your JAVA_HOME is set and that you have a GCC compiler.
the following version of java and GCC were used to test:

```
java==1.8.0_151
gcc==6.3.0
```


## installation

```
make
```

## testing with java
 
```
cd java
"${JAVA_HOME}/bin/java.exe" -classpath "java/classes" -Djava.library.path="{PATH_TO_LIBRARY}" com.jsonb.ExecJar.TestExample
```

## overview / integration

below are samples of code based on [http://json-b.net/users-guide.html].
All examples are shown with equivalent code using this C based JSON-B wrapper.

### mapping an object

```
public class Dog {
    public String name;
    public int age;
    public boolean bitable;
}

// Create a dog instance
Dog dog = new Dog();
dog.name = "Falco";
dog.age = 4;
dog.bitable = false;

// serialize with json-b
JSONBReflection reflection = new JSONBReflection();
JSONBGenericCls<?> dogCls = new JSONBGenericCls<Dog>(Dog.Class);
Dog dog1 = (Dog) reflection.fromJSONB("{name:\"Falco\",age:4,bitable:false}", dogCls);
```

### mapping a collection

```
// more than 1 dog
Dog[2] dogs = new Dog[2];
dogs[0] = falco;
dogs[1] = cassidy;

JSONBReflection reflection = new JSONBReflection();
JSONBGenericCls<?> dogCls = new JSONBGenericCls<Dog>(Dog.Class);
String result = reflection.toJSONBArray(dogs, dogCls);

// deserialize our json-b string
dogs = reflection.fromJSONBArray(result, dogCls);
```
```
more coming soon..


