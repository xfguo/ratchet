import os
env = Environment(
       CCFLAGS = '-I. -Isrc/c -Isrc/c/dns -Isrc/c/dns/libdns -I/usr/include/lua5.2 -std=gnu99 -fPIC -DPIC'
)
src = [
    './src/c/ratchet.c',
    './src/c/misc.c',
    './src/c/error.c',
    './src/c/exec.c',
    './src/c/sockopt.c',
]

# zmq
src += './src/c/zmq.c',

# ssl
src += './src/c/ssl.c',

# timerfd
src += './src/c/timerfd.c',

# socket
src += './src/c/socket.c',

# dns
src += Glob('./src/c/dns/*.c') + Glob('./src/c/dns/libdns/*.c')

objs = env.SharedObject(src)
env.SharedLibrary('ratchet.so', objs, LIBS=['event', 'ssl', 'crypto', 'zmq'], SHLIBPREFIX='') # without `lib' prefix verison
env.SharedLibrary('ratchet.so', objs, LIBS=['event', 'ssl', 'crypto', 'zmq'])
