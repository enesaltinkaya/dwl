#ifndef DWL_H
#define DWL_H

#include <stdint.h>
#include <xkbcommon/xkbcommon.h>
#include <wayland-server-core.h>
#include <wlr/types/wlr_output.h> /* for wl_output_transform */
#include <getopt.h>
#include <libinput.h>
#include <linux/input-event-codes.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/backend/libinput.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_alpha_modifier_v1.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_cursor_shape_v1.h>
#include <wlr/types/wlr_data_control_v1.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_drm.h>
#include <wlr/types/wlr_export_dmabuf_v1.h>
#include <wlr/types/wlr_fractional_scale_v1.h>
#include <wlr/types/wlr_gamma_control_v1.h>
#include <wlr/types/wlr_idle_inhibit_v1.h>
#include <wlr/types/wlr_idle_notify_v1.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_keyboard_group.h>
#include <wlr/types/wlr_layer_shell_v1.h>
#include <wlr/types/wlr_linux_dmabuf_v1.h>
#include <wlr/types/wlr_linux_drm_syncobj_v1.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_output_management_v1.h>
#include <wlr/types/wlr_output_power_management_v1.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_pointer_constraints_v1.h>
#include <wlr/types/wlr_presentation_time.h>
#include <wlr/types/wlr_primary_selection.h>
#include <wlr/types/wlr_primary_selection_v1.h>
#include <wlr/types/wlr_relative_pointer_v1.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_screencopy_v1.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_server_decoration.h>
#include <wlr/types/wlr_session_lock_v1.h>
#include <wlr/types/wlr_single_pixel_buffer_v1.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_viewporter.h>
#include <wlr/types/wlr_virtual_keyboard_v1.h>
#include <wlr/types/wlr_virtual_pointer_v1.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_xdg_activation_v1.h>
#include <wlr/types/wlr_xdg_decoration_v1.h>
#include <wlr/types/wlr_xdg_output_v1.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/log.h>
#include <wlr/util/region.h>
#include <xkbcommon/xkbcommon.h>
#ifdef XWAYLAND
#include <wlr/xwayland.h>
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#endif


/* Forward declarations */
struct wl_event_source;
struct wlr_box;
struct wlr_keyboard_group;
struct wlr_layer_surface_v1;
struct wlr_output;
struct wlr_pointer_constraint_v1;
struct wlr_scene_layer_surface_v1;
struct wlr_scene_output;
struct wlr_scene_rect;
struct wlr_scene_tree;
struct wlr_session_lock_surface_v1;
struct wlr_session_lock_v1;
struct wlr_xdg_surface;
struct wlr_xdg_toplevel_decoration_v1;
struct wlr_xwayland_surface;

/* enums */
enum { CurNormal, CurPressed, CurMove, CurResize }; /* cursor */

enum { XDGShell, LayerShell, X11 }; /* client types */

enum { LyrBg, LyrBottom, LyrTile, LyrFloat, LyrTop, LyrFS, LyrOverlay, LyrBlock, NUM_LAYERS }; /* scene layers */

typedef union {
    int i;
    uint32_t ui;
    float f;
    const void* v;
} Arg;

typedef struct {
    unsigned int mod;
    unsigned int button;
    void (*func)(const Arg*);
    const Arg arg;
} Button;

typedef struct Monitor Monitor;

typedef struct {
    /* Must keep this field first */
    unsigned int type; /* XDGShell or X11* */

    Monitor* mon;
    struct wlr_scene_tree* scene;
    struct wlr_scene_rect* border[4]; /* top, bottom, left, right */
    struct wlr_scene_tree* scene_surface;
    struct wl_list link;
    struct wl_list flink;
    struct wlr_box geom;   /* layout-relative, includes border */
    struct wlr_box prev;   /* layout-relative, includes border */
    struct wlr_box bounds; /* only width and height are used */

    union {
        struct wlr_xdg_surface* xdg;
        struct wlr_xwayland_surface* xwayland;
    } surface;
    struct wlr_xdg_toplevel_decoration_v1* decoration;
    struct wl_listener commit;
    struct wl_listener map;
    struct wl_listener maximize;
    struct wl_listener unmap;
    struct wl_listener destroy;
    struct wl_listener set_title;
    struct wl_listener fullscreen;
    struct wl_listener set_decoration_mode;
    struct wl_listener destroy_decoration;
#ifdef XWAYLAND
    struct wl_listener activate;
    struct wl_listener associate;
    struct wl_listener dissociate;
    struct wl_listener configure;
    struct wl_listener set_hints;
#endif
    int bw;
    uint32_t tags;
    int isfloating, isurgent, isfullscreen;
    uint32_t resize; /* configure serial of a pending resize */
} Client;

typedef struct {
    uint32_t mod;
    xkb_keysym_t keysym;
    void (*func)(const Arg*);
    const Arg arg;
} Key;

typedef struct {
    struct wlr_keyboard_group* wlr_group;

    int nsyms;
    const xkb_keysym_t* keysyms; /* invalid if nsyms == 0 */
    uint32_t mods;               /* invalid if nsyms == 0 */
    struct wl_event_source* key_repeat_source;

    struct wl_listener modifiers;
    struct wl_listener key;
    struct wl_listener destroy;
} KeyboardGroup;

typedef struct {
    /* Must keep this field first */
    unsigned int type; /* LayerShell */

    Monitor* mon;
    struct wlr_scene_tree* scene;
    struct wlr_scene_tree* popups;
    struct wlr_scene_layer_surface_v1* scene_layer;
    struct wl_list link;
    int mapped;
    struct wlr_layer_surface_v1* layer_surface;

    struct wl_listener destroy;
    struct wl_listener unmap;
    struct wl_listener surface_commit;
} LayerSurface;

typedef struct {
    const char* symbol;
    void (*arrange)(Monitor*);
} Layout;

struct Monitor {
    struct wl_list link;
    struct wlr_output* wlr_output;
    struct wlr_scene_output* scene_output;
    struct wlr_scene_rect* fullscreen_bg; /* See createmon() for info */
    struct wl_listener frame;
    struct wl_listener destroy;
    struct wl_listener request_state;
    struct wl_listener destroy_lock_surface;
    struct wlr_session_lock_surface_v1* lock_surface;
    struct wlr_box m;         /* monitor area, layout-relative */
    struct wlr_box w;         /* window area, layout-relative */
    struct wl_list layers[4]; /* LayerSurface.link */
    const Layout* lt[2];
    unsigned int seltags;
    unsigned int sellt;
    uint32_t tagset[2];
    float mfact;
    int gamma_lut_changed;
    int nmaster;
    char ltsymbol[16];
    int asleep;
};

typedef struct {
    const char* name;
    float mfact;
    int nmaster;
    float scale;
    const Layout* lt;
    enum wl_output_transform rr;
    int x, y;
} MonitorRule;

typedef struct {
    struct wlr_pointer_constraint_v1* constraint;
    struct wl_listener destroy;
} PointerConstraint;

typedef struct {
    struct wlr_scene_tree* scene;

    struct wlr_session_lock_v1* lock;
    struct wl_listener new_surface;
    struct wl_listener unlock;
    struct wl_listener destroy;
} SessionLock;

/* Rule structure definition */
typedef struct {
    const char* id;
    const char* title;
    uint32_t tags;
    int isfloating;
    int monitor;
} Rule;

/* function declarations */
void applybounds(Client* c, struct wlr_box* bbox);
void applyrules(Client* c);
void arrange(Monitor* m);
void arrangelayer(Monitor* m, struct wl_list* list, struct wlr_box* usable_area, int exclusive);
void arrangelayers(Monitor* m);
void axisnotify(struct wl_listener* listener, void* data);
void buttonpress(struct wl_listener* listener, void* data);
void chvt(const Arg* arg);
void checkidleinhibitor(struct wlr_surface* exclude);
void cleanup(void);
void cleanupmon(struct wl_listener* listener, void* data);
void cleanuplisteners(void);
void closemon(Monitor* m);
void commitlayersurfacenotify(struct wl_listener* listener, void* data);
void commitnotify(struct wl_listener* listener, void* data);
void commitpopup(struct wl_listener* listener, void* data);
void createdecoration(struct wl_listener* listener, void* data);
void createidleinhibitor(struct wl_listener* listener, void* data);
void createkeyboard(struct wlr_keyboard* keyboard);
KeyboardGroup* createkeyboardgroup(void);
void createlayersurface(struct wl_listener* listener, void* data);
void createlocksurface(struct wl_listener* listener, void* data);
void createmon(struct wl_listener* listener, void* data);
void createnotify(struct wl_listener* listener, void* data);
void createpointer(struct wlr_pointer* pointer);
void createpointerconstraint(struct wl_listener* listener, void* data);
void createpopup(struct wl_listener* listener, void* data);
void cursorconstrain(struct wlr_pointer_constraint_v1* constraint);
void cursorframe(struct wl_listener* listener, void* data);
void cursorwarptohint(void);
void destroydecoration(struct wl_listener* listener, void* data);
void destroydragicon(struct wl_listener* listener, void* data);
void destroyidleinhibitor(struct wl_listener* listener, void* data);
void destroylayersurfacenotify(struct wl_listener* listener, void* data);
void destroylock(SessionLock* lock, int unlocked);
void destroylocksurface(struct wl_listener* listener, void* data);
void destroynotify(struct wl_listener* listener, void* data);
void destroypointerconstraint(struct wl_listener* listener, void* data);
void destroysessionlock(struct wl_listener* listener, void* data);
void destroykeyboardgroup(struct wl_listener* listener, void* data);
Monitor* dirtomon(enum wlr_direction dir);
void focusclient(Client* c, int lift);
void focusmon(const Arg* arg);
void focusstack(const Arg* arg);
Client* focustop(Monitor* m);
void fullscreennotify(struct wl_listener* listener, void* data);
void gpureset(struct wl_listener* listener, void* data);
void handlesig(int signo);
void incnmaster(const Arg* arg);
void inputdevice(struct wl_listener* listener, void* data);
int keybinding(uint32_t mods, xkb_keysym_t sym);
void keypress(struct wl_listener* listener, void* data);
void keypressmod(struct wl_listener* listener, void* data);
int keyrepeat(void* data);
void killclient(const Arg* arg);
void locksession(struct wl_listener* listener, void* data);
void mapnotify(struct wl_listener* listener, void* data);
void maximizenotify(struct wl_listener* listener, void* data);
void monocle(Monitor* m);
void motionabsolute(struct wl_listener* listener, void* data);
void motionnotify(uint32_t time, struct wlr_input_device* device, double sx, double sy, double sx_unaccel, double sy_unaccel);
void motionrelative(struct wl_listener* listener, void* data);
void moveresize(const Arg* arg);
void outputmgrapply(struct wl_listener* listener, void* data);
void outputmgrapplyortest(struct wlr_output_configuration_v1* config, int test);
void outputmgrtest(struct wl_listener* listener, void* data);
void pointerfocus(Client* c, struct wlr_surface* surface, double sx, double sy, uint32_t time);
void printstatus(void);
void powermgrsetmode(struct wl_listener* listener, void* data);
void quit(const Arg* arg);
void quitTest(const Arg* arg);
void rendermon(struct wl_listener* listener, void* data);
void requestdecorationmode(struct wl_listener* listener, void* data);
void requeststartdrag(struct wl_listener* listener, void* data);
void requestmonstate(struct wl_listener* listener, void* data);
void resize(Client* c, struct wlr_box geo, int interact);
void run(char* startup_cmd);
void setcursor(struct wl_listener* listener, void* data);
void setcursorshape(struct wl_listener* listener, void* data);
void setfloating(Client* c, int floating);
void setfullscreen(Client* c, int fullscreen);
void setlayout(const Arg* arg);
void setmfact(const Arg* arg);
void setmon(Client* c, Monitor* m, uint32_t newtags);
void setpsel(struct wl_listener* listener, void* data);
void setsel(struct wl_listener* listener, void* data);
void setup(void);
void spawn(const Arg* arg);
void startdrag(struct wl_listener* listener, void* data);
void tag(const Arg* arg);
void tagmon(const Arg* arg);
void tile(Monitor* m);
void togglefloating(const Arg* arg);
void togglefullscreen(const Arg* arg);
void toggletag(const Arg* arg);
void toggleview(const Arg* arg);
void unlocksession(struct wl_listener* listener, void* data);
void unmaplayersurfacenotify(struct wl_listener* listener, void* data);
void unmapnotify(struct wl_listener* listener, void* data);
void updatemons(struct wl_listener* listener, void* data);
void updatetitle(struct wl_listener* listener, void* data);
void urgent(struct wl_listener* listener, void* data);
void view(const Arg* arg);
void virtualkeyboard(struct wl_listener* listener, void* data);
void virtualpointer(struct wl_listener* listener, void* data);
Monitor* xytomon(double x, double y);
void xytonode(double x, double y, struct wlr_surface** psurface, Client** pc, LayerSurface** pl, double* nx, double* ny);
void zoom(const Arg* arg);

#endif  // DWL_H
