all:
	gcc mucignore.c $(CFLAGS) -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/loudmouth-1.0/ -std=c99 -lglib-2.0 -shared -DMODULES_ENABLE -o libmucignore.so -fPIC

install: all
	install -D libmucignore.so "$(DESTDIR)"/libmucignore.so

.PHONY: install
