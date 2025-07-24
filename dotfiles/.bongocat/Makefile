clean: 
	rm bongocat
	rm xdg-shell-client-protocol.h xdg-shell-protocol.c zwlr-layer-shell-v1-protocol.c zwlr-layer-shell-v1-client-protocol.h


install: 
	wayland-scanner client-header \
	  /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
	  xdg-shell-client-protocol.h
	wayland-scanner private-code \
	  /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
	  xdg-shell-protocol.c
	# wget https://raw.githubusercontent.com/swaywm/wlr-protocols/master/unstable/wlr-layer-shell-unstable-v1.xml
	wayland-scanner private-code wlr-layer-shell-unstable-v1.xml zwlr-layer-shell-v1-protocol.c
	wayland-scanner client-header wlr-layer-shell-unstable-v1.xml zwlr-layer-shell-v1-client-protocol.h
	# wget https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
	gcc bongocat.c -o bongocat zwlr-layer-shell-v1-protocol.c xdg-shell-protocol.c -lwayland-client -lm
