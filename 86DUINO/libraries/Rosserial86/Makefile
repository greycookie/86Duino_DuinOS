
CC := gcc
AR := ar
CFLAGS := -c -w -g -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -O1 -DARDUINO=158 -D_86DUINO=210

INCLUDE_CORE := -I..\..\core -I.\\
OBJS := 86DuinoHardware.o duration.o time.o
TARGETS := libRosserial86.a

.PHONY : everything clean all

everything: $(TARGETS)

clean:
	rm $(OBJS) $(TARGETS)

all: clean everything

$(TARGETS): $(OBJS) $(UTILITY_LIB)
	$(AR) rcs $(TARGETS) $?

86DuinoHardware.o: 86DuinoHardware.cpp 86DuinoConfig.h 86DuinoHardware.h vortex86\utilities\dmpcfg.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

duration.o: duration.cpp ros\duration.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)

time.o: time.cpp ros.h ros\node_handle.h std_msgs\Time.h ros\msg.h ros\time.h \
 ros\duration.h rosserial_msgs\TopicInfo.h rosserial_msgs\Log.h rosserial_msgs\RequestParam.h ros\msg.h ros\publisher.h \
 ros\node_handle.h ros\subscriber.h ros\service_server.h ros\service_client.h 86DuinoHardware.h ros\time.h
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_CORE)