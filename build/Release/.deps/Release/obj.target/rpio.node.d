cmd_Release/obj.target/rpio.node := flock ./Release/linker.lock g++ -shared -pthread -rdynamic -m64  -Wl,-soname=rpio.node -o Release/obj.target/rpio.node -Wl,--start-group Release/obj.target/rpio/src/bcm2835.o Release/obj.target/rpio/src/rpio.o -Wl,--end-group 
