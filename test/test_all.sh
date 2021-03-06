ln -sf `pwd`/src/lua `pwd`/src/ratchet
export LUA_PATH="`pwd`/src/?.lua;`pwd`/src/?/init.lua"
lua5.2 test/test_5sec_timer.lua
lua5.2 test/test_callable_object.lua
lua5.2 test/test_event_timeout.lua
lua5.2 test/test_exec.lua
lua5.2 test/test_http_get.lua
lua5.2 test/test_listen_connect.lua
lua5.2 test/test_message_bus_local.lua
lua5.2 test/test_message_bus_sockets.lua
lua5.2 test/test_multi_protocol.lua
lua5.2 test/test_pause_unpause.lua
lua5.2 test/test_pcall_kernel_loop.lua
lua5.2 test/test_send_recv.lua
lua5.2 test/test_shutdown.lua
lua5.2 test/test_smtp_bigmessage.lua
lua5.2 test/test_smtp.lua
#   - lua5.2 test/test_smtp_starttls.lua
#   - lua5.2 test/test_smtp_tls.lua
lua5.2 test/test_socket_byteorder.lua
lua5.2 test/test_socket_multi_recv.lua
lua5.2 test/test_socketpad.lua
lua5.2 test/test_socketpair.lua
lua5.2 test/test_sockopt.lua
#   - lua5.2 test/test_ssl_send_recv.lua
lua5.2 test/test_thread_alarm.lua
lua5.2 test/test_thread_kill.lua
lua5.2 test/test_thread_space.lua
lua5.2 test/test_timerfd.lua
lua5.2 test/test_unix_sockets.lua
lua5.2 test/test_wait_all.lua
lua5.2 test/test_zmq_send_recv.lua
