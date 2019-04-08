shared:
	cc -O3 -Iextern -Iextern/lua/api/lua5.3 aist.c -llua -fPIC -shared -o aist.so
