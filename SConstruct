import os
env = Environment(
    CCFLAGS = '-I. -Isrc/c -Isrc/c/dns -Isrc/c/dns/libdns -I/usr/include/lua5.2 -std=gnu99 -fPIC -DPIC',
    LUA_VER = 5.2
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
shlib_without_prefix = env.SharedLibrary('ratchet.so', objs, LIBS=['event', 'ssl', 'crypto', 'zmq'], SHLIBPREFIX='') # without `lib' prefix verison
shlib = env.SharedLibrary('ratchet.so', objs, LIBS=['event', 'ssl', 'crypto', 'zmq'])

env.Default([shlib_without_prefix, shlib])

shlib_install = env.Install('/usr/local/lib', shlib)
shlib_without_prefix_install = env.Install('/usr/local/lib/lua/$LUA_VER', shlib_without_prefix)
luasrc_install = env.Install('/usr/local/lib/lua/$LUA_VER/ratchet', Glob("./src/lua/*"))

env.Alias('install', [shlib_install, shlib_without_prefix_install, luasrc_install])
if GetOption("clean"):
    clean_luasrc_install = env.Clean('install', '/usr/local/lib/lua/$LUA_VER/ratchet')
