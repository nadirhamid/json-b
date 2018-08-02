dll:
	rm -f ./jsonb.dll
	gcc -Wl,--add-stdcall-alias -m32 -I"${JAVA_HOME}\include" -I"${JAVA_HOME}\include\win32" -c jsonb.c -o jsonb.o
	gcc -Wl,--add-stdcall-alias -m32 -I"${JAVA_HOME}\include" -I"${JAVA_HOME}\include\win32" -c java.c -o java.o
	gcc -Wl,--add-stdcall-alias -m32 -I"${JAVA_HOME}\include" -I"${JAVA_HOME}\include\win32" -shared -o jsonb.dll jsonb.o java.o
	"${JAVA_HOME}/bin/javac.exe" -d "java/classes" -classpath "java/classes" "java/src/com/infinitet3ch/jsonb/TestExample.java" "java/src/com/infinitet3ch/jsonb/ParserException.java" "java/src/com/infinitet3ch/jsonb/Reflection.java"
nodll:
	gcc -Wl,--add-stdcall-alias -m32 -I"${JAVA_HOME}\include" -I"${JAVA_HOME}\include\win32" -o jsonb.exe jsonb.c

all:
	dll
	run
run:
	cd ./java/
	"${JAVA_HOME}/bin/java.exe" -classpath "java/classes" com.infinitet3ch.jsonb.TestExample


