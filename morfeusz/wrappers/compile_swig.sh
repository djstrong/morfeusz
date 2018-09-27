swig -python -c++ morfeusz.i

x86_64-linux-gnu-gcc -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/home/jszejko/morfeusz/trunk/morfeusz/wrappers/python/../.. -I/home/jszejko/morfeusz/trunk/buildall/Linux-amd64-true -I/usr/include/python2.7 -c morfeusz_wrap.cxx -o swigPYTHON.o

c++ -pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -Wl,-z,relro -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -D_FORTIFY_SOURCE=2 -g -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security swigPYTHON.o -L/home/jszejko/morfeusz/trunk/buildall/Linux-amd64-true/morfeusz/wrappers/python/../.. -lmorfeusz2 -o _morfeusz2.so
