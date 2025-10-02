#define _POSIX_C_SOURCE 200809L
#include <wayland-client.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/input-event-codes.h>

#include "xdg-shell-client-protocol.h"
#include "zwlr-layer-shell-v1-client-protocol.h"

// ------------------- Globals (Wayland) -------------------
struct wl_display* display;
struct wl_compositor* compositor;
struct wl_shm* shm;
struct zwlr_layer_shell_v1* layer_shell;
struct wl_surface* surface;
struct wl_buffer* buffer;
struct zwlr_layer_surface_v1* layer_surface;
uint8_t* pixels;

// Input
struct wl_seat* seat;
struct wl_pointer* pointer;
int mouse_x = -1, mouse_y = -1;

// Screen
int screen_width = 1920;
int screen_height = 1080;

// Fire region
int fire_x = 500;
int fire_y = 500;
int fire_width = 200;
int fire_height = 500;


// --------------- Fire simulation -----------------
int* firePixels;
int palette[256][3];

void make_palette() {
    for (int i = 0; i < 256; i++) {
        int r = i < 128 ? i*2 : 255;
        int g = i < 128 ? 0 : (i-128)*2;
        int b = 0;
        palette[i][0] = r;
        palette[i][1] = g;
        palette[i][2] = b;
    }
}

void init_fire_buffer() {
    firePixels = calloc(fire_width * fire_height, sizeof(int));
}

void ignite_bottom() {
    for (int x = 0; x < fire_width; x++) {
        int intensity = rand() % 100 + 155; // 155-255
        firePixels[(fire_height-1)*fire_width + x] = intensity;
    }
}

void ignite_at(int mx, int my) {
    if (mx < fire_x || mx >= fire_x + fire_width) return;
    if (my < fire_y || my >= fire_y + fire_height) return;
    int fx = mx - fire_x;
    int fy = my - fire_y;
    // make mouse ignition stronger near bottom
    for (int y = fy; y < fire_height; y++) {
        int idx = y * fire_width + fx;
        firePixels[idx] = 255;
    }
}

void do_fire_step() {
    for (int y = 1; y < fire_height; y++) {
        for (int x = 0; x < fire_width; x++) {
            int src = y * fire_width + x;
            int below = src + fire_width;
            if (below >= fire_width * fire_height) continue;

            int decay = rand() % 2 + 1;  // smaller decay to allow taller flames
            int val = firePixels[below] - decay;

            // Fade based on height (less aggressive)
            val = val * (fire_height - y + 3) / fire_height;
            if (val < 0) val = 0;

            // Smooth swirl: consider neighbors
            int left = x > 0 ? firePixels[below - 1] : 0;
            int right = x < fire_width - 1 ? firePixels[below + 1] : 0;
            val = (val + left + right) / 3;

            // Slight horizontal jitter
            int swirl = rand() % 3 - 1; // -1,0,1
            int dst_x = x + swirl;
            if (dst_x < 0) dst_x = 0;
            if (dst_x >= fire_width) dst_x = fire_width - 1;

            int dst = (y - 1) * fire_width + dst_x;
            firePixels[dst] = val;
        }
    }

    // Bottom row ignition: stronger & wider
    for (int x = 0; x < fire_width; x++) {
        if (rand() % 3 == 0) { // 1/3 chance for extra fire
            firePixels[(fire_height - 1) * fire_width + x] = rand() % 100 + 155;
        }
    }
}


void render_fire() {
    memset(pixels, 0, screen_width * screen_height * 4);

    for (int y = 0; y < fire_height; y++) {
        for (int x = 0; x < fire_width; x++) {
            int colorIndex = firePixels[y * fire_width + x];
            if (colorIndex == 0) continue;
            int r = palette[colorIndex][0];
            int g = palette[colorIndex][1];
            int b = palette[colorIndex][2];
            int px = (fire_y + y) * screen_width + (fire_x + x);
            int idx = px * 4;
            pixels[idx + 0] = b;
            pixels[idx + 1] = g;
            pixels[idx + 2] = r;
            pixels[idx + 3] = 255;
        }
    }
}

// ------------------- Wayland SHM -------------------
int create_shm(int size){
    char name[]="/bar-shm-XXXXXX";
    int fd;
    for(int i=0;i<100;i++){
        for(int j=0;j<6;j++) name[9+j]='A'+(rand()%26);
        fd = shm_open(name,O_RDWR|O_CREAT|O_EXCL,0600);
        if(fd>=0){shm_unlink(name); break;}
    }
    if(fd<0||ftruncate(fd,size)<0){
        perror("shm"); exit(1);
    }
    return fd;
}

// ------------------- Wayland Listeners -------------------
void layer_surface_configure(void* data, struct zwlr_layer_surface_v1* ls,
                             uint32_t serial, uint32_t w, uint32_t h){
    (void)data;(void)w;(void)h;
    zwlr_layer_surface_v1_ack_configure(ls,serial);
}
struct zwlr_layer_surface_v1_listener layer_listener = {
    .configure = layer_surface_configure,
    .closed = NULL
};

void registry_global(void* _, struct wl_registry* reg, uint32_t name,
                     const char* iface, uint32_t ver){
    (void)_;
    if(strcmp(iface,wl_compositor_interface.name)==0)
        compositor=wl_registry_bind(reg,name,&wl_compositor_interface,4);
    else if(strcmp(iface,wl_shm_interface.name)==0)
        shm=wl_registry_bind(reg,name,&wl_shm_interface,1);
    else if(strcmp(iface,zwlr_layer_shell_v1_interface.name)==0)
        layer_shell=wl_registry_bind(reg,name,&zwlr_layer_shell_v1_interface,4);
    else if(strcmp(iface,wl_seat_interface.name)==0)
        seat=wl_registry_bind(reg,name,&wl_seat_interface,5);
}
void registry_remove(void* _, struct wl_registry* r, uint32_t name){(void)_;(void)r;(void)name;}
struct wl_registry_listener reg_listener={
    .global=registry_global,
    .global_remove=registry_remove
};

// ------------------- Pointer -------------------
void pointer_enter(void* d, struct wl_pointer* p, uint32_t s,
                   struct wl_surface* surf, wl_fixed_t sx, wl_fixed_t sy){
    (void)d;(void)p;(void)s;(void)surf;
    mouse_x=wl_fixed_to_int(sx);
    mouse_y=wl_fixed_to_int(sy);
    ignite_at(mouse_x,mouse_y);
}
void pointer_leave(void* d, struct wl_pointer* p, uint32_t s, struct wl_surface* surf){
    (void)d;(void)p;(void)s;(void)surf;
    mouse_x=mouse_y=-1;
}
void pointer_motion(void* d, struct wl_pointer* p, uint32_t t, wl_fixed_t sx, wl_fixed_t sy){
    (void)d;(void)p;(void)t;
    mouse_x=wl_fixed_to_int(sx);
    mouse_y=wl_fixed_to_int(sy);
    ignite_at(mouse_x,mouse_y);
}
void pointer_button(void* d, struct wl_pointer* p, uint32_t s, uint32_t t, uint32_t b, uint32_t st){(void)d;(void)p;(void)s;(void)t;(void)b;(void)st;}
void pointer_axis(void* d, struct wl_pointer* p, uint32_t t, uint32_t axis, wl_fixed_t v){(void)d;(void)p;(void)t;(void)axis;(void)v;}
struct wl_pointer_listener pointer_listener={
    .enter=pointer_enter,
    .leave=pointer_leave,
    .motion=pointer_motion,
    .button=pointer_button,
    .axis=pointer_axis
};

void seat_handle_capabilities(void* d, struct wl_seat* s, uint32_t caps){
    if(caps & WL_SEAT_CAPABILITY_POINTER){
        pointer = wl_seat_get_pointer(s);
        wl_pointer_add_listener(pointer,&pointer_listener,NULL);
    }
}
void seat_handle_name(void* d, struct wl_seat* s, const char* name){(void)d;(void)s;(void)name;}
struct wl_seat_listener seat_listener={
    .capabilities=seat_handle_capabilities,
    .name=seat_handle_name
};

// ------------------- Update loop -------------------
void* update(void* _){
    (void)_;
    struct timespec ts={0,33*1000000};
    while(1){
        do_fire_step();
        render_fire();
        wl_surface_attach(surface,buffer,0,0);
        wl_surface_damage_buffer(surface,0,0,screen_width,screen_height);
        wl_surface_commit(surface);
        wl_display_flush(display);
        nanosleep(&ts,NULL);
    }
    return NULL;
}

// ------------------- Main -------------------
int main(){
    srand(time(NULL));

    display=wl_display_connect(NULL);
    struct wl_registry* registry=wl_display_get_registry(display);
    wl_registry_add_listener(registry,&reg_listener,NULL);
    wl_display_roundtrip(display);
    if(seat) wl_seat_add_listener(seat,&seat_listener,NULL);

    surface=wl_compositor_create_surface(compositor);
    layer_surface=zwlr_layer_shell_v1_get_layer_surface(layer_shell,surface,
        NULL,ZWLR_LAYER_SHELL_V1_LAYER_TOP,"fire");
    zwlr_layer_surface_v1_set_anchor(layer_surface,
        ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP|
        ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT|
        ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT);
    zwlr_layer_surface_v1_set_size(layer_surface,0,screen_height);
    zwlr_layer_surface_v1_set_exclusive_zone(layer_surface,-1);
    zwlr_layer_surface_v1_add_listener(layer_surface,&layer_listener,NULL);

    struct wl_region* input_region=wl_compositor_create_region(compositor);
    wl_surface_set_input_region(surface,input_region);
    wl_region_destroy(input_region);
    wl_surface_commit(surface);

    int size=screen_width*screen_height*4;
    int fd=create_shm(size);
    pixels=mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    struct wl_shm_pool* pool=wl_shm_create_pool(shm,fd,size);
    buffer=wl_shm_pool_create_buffer(pool,0,screen_width,screen_height,
                                     screen_width*4,WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);

    make_palette();
    init_fire_buffer();

    pthread_t t1;
    pthread_create(&t1,NULL,update,NULL);

    while(wl_display_dispatch(display)!=-1){}

    return 0;
}
