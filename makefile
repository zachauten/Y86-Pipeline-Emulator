CC = gcc -g

yess: decodeStage.o dump.o executeStage.o fetchStage.o \
loader.o memory.o memoryStage.o registers.o tools.o writebackStage.o main.o
	$(CC) -o yess main.o decodeStage.o dump.o executeStage.o fetchStage.o \
    loader.o memory.o memoryStage.o registers.o tools.o writebackStage.o

decodeStage.o: bool.h tools.h instructions.h status.h forwarding.h bubbling.h decodeStage.h executeStage.h registers.h

dump.o: bool.h dump.h fetchStage.h forwarding.h bubbling.h decodeStage.h executeStage.h \
memoryStage.h writebackStage.h registers.h memory.h status.h

executeStage.o: bool.h tools.h instructions.h forwarding.h status.h bubbling.h executeStage.h memoryStage.h registers.h

fetchStage.o: bool.h tools.h memory.h instructions.h bubbling.h fetchStage.h registers.h

loader.o: bool.h loader.h

memory.o: memory.h tools.h

memoryStage.o: bool.h tools.h status.h instructions.h bubbling.h forwarding.h memoryStage.h writebackStage.h \
registers.h memory.h

registers.o: bool.h registers.h

tools.o: bool.h tools.h

writebackStage.o: bool.h tools.h instructions.h forwarding.h status.h writebackStage.h

main.o: bool.h tools.h memory.h dump.h loader.h instructions.h fetchStage.h forwarding.h\
decodeStage.h executeStage.h memoryStage.h writebackStage.h

clean:
	rm -f *.o

