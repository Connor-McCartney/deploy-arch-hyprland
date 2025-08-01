#include <stdio.h>
#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>



int *any_key_pressed;
void capture(char* filename)
{
    int fd = open(filename, O_RDONLY);
    struct input_event ev[64];
    int i, rd;
    while (1) {
        rd = read(fd, ev, sizeof(ev));
        for (i = 0; i < rd / sizeof(struct input_event); i++) {
            if (ev[i].value == 1) {
                *any_key_pressed = 1;
            }
        }
        usleep(10000);
    }
}

#define _POSIX_C_SOURCE 200809L
#include <wayland-client.h>
#include <stdbool.h>
#include "zwlr-layer-shell-v1-client-protocol.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Globals
bool configured = false;
struct wl_display* display;
struct wl_compositor* compositor;
struct wl_shm* shm;
struct zwlr_layer_shell_v1* layer_shell;
struct wl_surface* surface;
struct wl_buffer* buffer;
struct zwlr_layer_surface_v1* layer_surface;
uint8_t* pixels;

int screen_width = 1920;
int bar_height = 60;

#define NUM_FRAMES1 3
#define NUM_FRAMES2 8

// Animation 1
const char* anim1_paths[NUM_FRAMES1] = {
    "/home/connor/.bongocat/cat_rest.png",
    "/home/connor/.bongocat/cat_left.png",
    "/home/connor/.bongocat/cat_right.png",
};
unsigned char* anim1_imgs[NUM_FRAMES1];
int anim1_width[NUM_FRAMES1], anim1_height[NUM_FRAMES1];
int anim1_index = 0;
pthread_mutex_t anim1_lock = PTHREAD_MUTEX_INITIALIZER;

// Animation 2
const char* anim2_paths[NUM_FRAMES2] = {
    "/home/connor/.bongocat/k1_shrunk.png",
    "/home/connor/.bongocat/k2_shrunk.png",
    "/home/connor/.bongocat/k3_shrunk.png",
    "/home/connor/.bongocat/k4_shrunk.png",
    "/home/connor/.bongocat/k5_shrunk.png",
    "/home/connor/.bongocat/k6_shrunk.png",
    "/home/connor/.bongocat/k7_shrunk.png",
    "/home/connor/.bongocat/k8_shrunk.png",
};
unsigned char* anim2_imgs[NUM_FRAMES2];
int anim2_width[NUM_FRAMES2], anim2_height[NUM_FRAMES2];
int anim2_index = 3;
pthread_mutex_t anim2_lock = PTHREAD_MUTEX_INITIALIZER;

//bool background_drawn = false;

int create_shm(int size) {
    char name[] = "/bar-shm-XXXXXX";
    int fd;
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 6; j++) name[9 + j] = 'A' + (rand() % 26);
        fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (fd >= 0) {
            shm_unlink(name);
            break;
        }
    }
    if (fd < 0 || ftruncate(fd, size) < 0) {
        perror("shm");
        exit(1);
    }
    return fd;
}

void blit_image(uint8_t* dest, int dest_w, int dest_h,
                unsigned char* src, int src_w, int src_h,
                int offset_x, int offset_y) {
    for (int y = 0; y < src_h; y++) {
        int dy = y + offset_y;
        if (dy < 0 || dy >= dest_h) continue;
        for (int x = 0; x < src_w; x++) {
            int dx = x + offset_x;
            if (dx < 0 || dx >= dest_w) continue;
            int di = (dy * dest_w + dx) * 4;
            int si = (y * src_w + x) * 4;
            dest[di+0] = (uint8_t)(src[si+2]);
            dest[di+1] = (uint8_t)(src[si+1]);
            dest[di+2] = (uint8_t)(src[si+0]);
            dest[di+3] = (uint8_t)(src[si+3]);
            /*
            float a = src[si+3] / 255.0f;
            if (dy < 46) {
                dest[di+0] = (uint8_t)(38 * (1-a) + src[si+2] * a);
                dest[di+1] = (uint8_t)(27 * (1-a) + src[si+1] * a);
                dest[di+2] = (uint8_t)(26 * (1-a) + src[si+0] * a);
                dest[di+3] = 255;
            } else {
                memcpy(&dest[di], &src[si], 4);
            }
            */
        }
    }
}

/*
void draw_rect(uint8_t* dest, int width, int height, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    for (int j = y; j < y + h; j++)
        for (int i = x; i < x + w; i++) {
            if (i < 0 || j < 0 || i >= width || j >= height) continue;
            int idx = (j * width + i) * 4;
            dest[idx+0] = b;
            dest[idx+1] = g;
            dest[idx+2] = r;
            dest[idx+3] = a;
        }
}
*/

void draw_bar() {
    if (!configured) return;
    /*
    if (!background_drawn) {
        draw_rect(pixels, screen_width, bar_height, 0, 0, screen_width, 46, 26, 27, 38, 255);
        background_drawn = true;
    }
    */
    pthread_mutex_lock(&anim1_lock);
    blit_image(pixels, screen_width, bar_height, anim1_imgs[anim1_index], anim1_width[anim1_index], anim1_height[anim1_index], screen_width/2+150, -4);
    pthread_mutex_unlock(&anim1_lock);
    pthread_mutex_lock(&anim2_lock);
    blit_image(pixels, screen_width, bar_height, anim2_imgs[anim2_index], anim2_width[anim2_index], anim2_height[anim2_index], screen_width/2-210, -8);
    pthread_mutex_unlock(&anim2_lock);

    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_damage_buffer(surface, 0, 0, screen_width, bar_height);
    wl_surface_commit(surface);
    wl_display_flush(display);
}

void layer_surface_configure(void* data, struct zwlr_layer_surface_v1* ls, uint32_t serial, uint32_t w, uint32_t h) {
    zwlr_layer_surface_v1_ack_configure(ls, serial);
    configured = true;
    draw_bar();
}

struct zwlr_layer_surface_v1_listener layer_listener = {
    .configure = layer_surface_configure,
    .closed = NULL,
};

void* animate1(void* _) {
    struct timespec ts = {0, 16 * 1000000};
    struct timeval last, now;
    long hold_until = 0;
    gettimeofday(&last, NULL);
    while (1) {
        gettimeofday(&now, NULL);
        long now_us = now.tv_sec * 1000000 + now.tv_usec;
        pthread_mutex_lock(&anim1_lock);
        if (*any_key_pressed) {
            anim1_index = (rand()%2)+1;
            hold_until = now_us + 100000;
        } else if (now_us > hold_until) anim1_index = 0;
        pthread_mutex_unlock(&anim1_lock);
        *any_key_pressed = 0;
        draw_bar();
        nanosleep(&ts, NULL);
    }
}

void* animate2(void* _) {
    struct timespec ts = {0, 16 * 1000000};
    struct timeval last, now;
    gettimeofday(&last, NULL);
    while (1) {
        gettimeofday(&now, NULL);
        long diff = (now.tv_sec - last.tv_sec) * 1000000 + (now.tv_usec - last.tv_usec);
        pthread_mutex_lock(&anim2_lock);
        if (diff < 1000000) anim2_index = 3;
        else if (diff < 1900000) anim2_index = 4;
        else if (diff < 2200000) anim2_index = 5;
        else if (diff < 2500000) anim2_index = 6;
        else if (diff < 2800000) anim2_index = 7;
        else if (diff < 4000000) anim2_index = 7;
        else if (diff < 5000000) anim2_index = 0;
        else if (diff < 5100000) anim2_index = 1;
        else if (diff < 6100000) anim2_index = 2;
        else if (diff < 6200000) anim2_index = 1;
        else last = now;
        pthread_mutex_unlock(&anim2_lock);
        draw_bar();
        nanosleep(&ts, NULL);
    }
}

int get_screen_width() {
    FILE *fp = popen("/usr/bin/hyprctl monitors active", "r");
    char buf[1000];
    fgets(buf, sizeof(buf), fp); // skip
    fgets(buf, sizeof(buf), fp);
    pclose(fp);
    strtok(buf, "x");
    return atoi(buf);
}

void registry_global(void* _, struct wl_registry* reg, uint32_t name, const char* iface, uint32_t ver) {
    if (strcmp(iface, wl_compositor_interface.name)==0)
        compositor = wl_registry_bind(reg, name, &wl_compositor_interface, 4);
    else if (strcmp(iface, wl_shm_interface.name)==0)
        shm = wl_registry_bind(reg, name, &wl_shm_interface, 1);
    else if (strcmp(iface, zwlr_layer_shell_v1_interface.name)==0)
        layer_shell = wl_registry_bind(reg, name, &zwlr_layer_shell_v1_interface, 1);
}

void registry_remove(void* _, struct wl_registry* r, uint32_t name) {}
struct wl_registry_listener reg_listener = {.global=registry_global, .global_remove=registry_remove};

int main() {
    usleep(2000000); // let waybar spawn first
    screen_width = get_screen_width();
    display = wl_display_connect(NULL);
    struct wl_registry* registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &reg_listener, NULL);
    wl_display_roundtrip(display);

    surface = wl_compositor_create_surface(compositor);
    layer_surface = zwlr_layer_shell_v1_get_layer_surface(layer_shell, surface, NULL,
        ZWLR_LAYER_SHELL_V1_LAYER_TOP, "my-bar");
    zwlr_layer_surface_v1_set_anchor(layer_surface, ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP |
        ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT | ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT);
    zwlr_layer_surface_v1_set_size(layer_surface, 0, bar_height);
    zwlr_layer_surface_v1_set_exclusive_zone(layer_surface, -1);
    zwlr_layer_surface_v1_add_listener(layer_surface, &layer_listener, NULL);


    //zwlr_layer_surface_v1_set_keyboard_interactivity(layer_surface, ZWLR_LAYER_SURFACE_V1_KEYBOARD_INTERACTIVITY_NONE);
    // Create an empty input region to make the surface click-through
    struct wl_region *input_region = wl_compositor_create_region(compositor);
    // Don't add any rectangles to the region, keeping it empty
    wl_surface_set_input_region(surface, input_region);
    wl_region_destroy(input_region);

    wl_surface_commit(surface);

    int size = screen_width * bar_height * 4;
    int fd = create_shm(size);
    pixels = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    struct wl_shm_pool* pool = wl_shm_create_pool(shm, fd, size);
    buffer = wl_shm_pool_create_buffer(pool, 0, screen_width, bar_height, screen_width * 4, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);

    for (int i = 0; i < NUM_FRAMES1; i++) anim1_imgs[i] = stbi_load(anim1_paths[i], &anim1_width[i], &anim1_height[i], NULL, 4);
    for (int i = 0; i < NUM_FRAMES2; i++) anim2_imgs[i] = stbi_load(anim2_paths[i], &anim2_width[i], &anim2_height[i], NULL, 4);

    any_key_pressed = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, animate1, NULL);
    pthread_create(&t2, NULL, animate2, NULL);

    if (fork()) {
        while (wl_display_dispatch(display) != -1) {}
    } else {
        FILE *fp;
        fp = popen("/bin/echo /dev/input/by-path/*event-kbd", "r");
        char devices[1000];
        fgets(devices, sizeof(devices), fp);
        devices[strlen(devices)-1] = '\0'; // just remove newline
        pclose(fp);

        char *device, *str;
        str = strdup(devices);  
        while ((device = strsep(&str, " "))) {
            if (!fork()) {
                capture(device);
            }
        }
    }
    return 0;
}
