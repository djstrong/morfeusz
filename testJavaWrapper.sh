#!/bin/bash
echo "testing java wrapper"
PROJECT_SRC_DIR=$1
PROJECT_BINARY_DIR=$2
TEST_DIR="$PROJECT_BINARY_DIR/morfeusz/wrappers/java"
JUNIT_JAR="$PROJECT_SRC_DIR/morfeusz/wrappers/java/test_lib/junit.jar"
JMORFEUSZ_JAR="$TEST_DIR/jmorfeusz.jar"
JAVA_TEST_FILE="$PROJECT_SRC_DIR/morfeusz/wrappers/java/JMorfeuszTest.java"

LD_LIBRARY_PATH="$PROJECT_BINARY_DIR/morfeusz:$TEST_DIR" javac -cp "$JUNIT_JAR:$JMORFEUSZ_JAR" -d "$TEST_DIR" "$JAVA_TEST_FILE"
LD_LIBRARY_PATH="$PROJECT_BINARY_DIR/morfeusz:$TEST_DIR" java -cp "$JUNIT_JAR:$JMORFEUSZ_JAR:$TEST_DIR" org.junit.runner.JUnitCore JMorfeuszTest
