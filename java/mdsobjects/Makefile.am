JAVASOURCE = 6
JAVABOOTCLASSPATH = -bootclasspath $(RT6JAR)
include ../Makefile.inc.am

java_srcdir = $(srcdir)/src/main/java
java_DATA = mdsobjects.jar
DEPENDENCIES = $(JSCOPE_JAR)

if USE_JARS_DIR

$(java_DATA): @JARS_DIR@/java/mdsobjects/mdsobjects.jar
	cp $< $@

else

$(java_DATA): $(DEPENDENCIES) classjava.stamp
	@ echo "Creating JAR: $@ ..."; \
	$(JAR) cf $@ -C $(JAVAROOT) .

endif

SOURCE = MDSplus/*.java
