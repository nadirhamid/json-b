# JSON-B C wrapper 
a native C11 based JSON-B parser/writer based on JNI. supports fromJSONB and toJSONB for
single objects and collections. (work in progress)

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

// serialize with json-b
Dog dog = (Dog) reflection.fromJSONB("{\"name\":\"Falco\",\"age\":4,\"bitable\":false}", Dog.class);

```

## serializing an object

```
// Create a dog instance
Dog dog = new Dog();
dog.name = "Falco";
dog.age = 4;
dog.bitable = false;

// serialize with json-b
Reflection reflection = new Reflection();
String result = reflection.toJSONB(dog, Dog.class);
```

### working with collections

```
// more than 1 dog
Reflection reflection = new Reflection();
// create our collection
Dog[2] dogs = (Dog) reflection.fromJSONBArray("[{\"name\":\"Falco\",\"age\":4,\"bitable\":false}, {\"name\":\"Cassidy\",\"age\":4,\"bitable\":false}]", Dog.class);

// serialize into JSON-B string
String result = reflection.toJSONBArray(dogs, Dog.class);
```

more coming soon..
