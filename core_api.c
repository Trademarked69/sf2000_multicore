#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <reent.h>
#include <ctype.h>

#include "libretro.h"
#include "file/file_path.h"
#include "file/config_file.h"

#include "core_api.h"
#include "core.h"
#include "debug.h"
#include "stockfw.h"
#include "video_sf2000.h"

#define MIPS_J(pfunc)    (2 << 26) | (uint32_t)pfunc >> 2 & ((1 << 26) - 1)
#define MIPS_JAL(pfunc)  (3 << 26) | (uint32_t)pfunc >> 2 & ((1 << 26) - 1)

#define PATCH_J(target, hook)    *(uint32_t*)(target) = MIPS_J(hook)
#define PATCH_JAL(target, hook)  *(uint32_t*)(target) = MIPS_JAL(hook)

// Hotkeys
// Up = 10, Down = 40, Left = 80, Right = 20
// Select = 1, Start = 8
// A = 2000, B = 4000. X = 400, Y = 800, L = 1000, R = 8000
#define HOTKEYSCREENSHOT 0x9008 // press L + R + Start
#define HOTKEYSAVESTATE 0x9400 // press L + R + X
#define HOTKEYLOADSTATE 0x9800 // press L + R + Y
#define HOTKEYINCREASESTATE 0x9020 // press L + R + Right
#define HOTKEYDECREASESTATE 0x9080 // press L + R + LEFT
#define HOTKEYINCREASEDARKEN 0x9010 // press L + R + UP
#define HOTKEYDECREASEDARKEN 0x9040 // press L + R + DOWN
#define HOTKEYSWAPPLAYER 45056 // press L + R + A

#define MAXPATH 	255
#define SYSTEM_DIRECTORY	"/mnt/sda1/bios"
#define SAVE_DIRECTORY		"/mnt/sda1/saves"
#define CONFIG_DIRECTORY	"/mnt/sda1/configs"

static config_file_t *s_core_config = NULL;
static void config_load();
static void config_free();
static bool config_get_var(struct retro_variable *var);

static retro_audio_buffer_status_callback_t	audio_buff_status_cb;

static void wrap_retro_set_environment(retro_environment_t cb);
static bool wrap_environ_cb(unsigned cmd, void *data);

static size_t mono_mix_audio_batch_cb(const int16_t *data, size_t frames);
static void mono_mix_audio_sample_cb(int16_t left, int16_t right);

static bool wrap_retro_load_game(const struct retro_game_info* info);
static void wrap_retro_init(void);
static void wrap_retro_deinit(void);
static void wrap_retro_unload_game(void);
static void wrap_retro_run(void);

static void log_cb(enum retro_log_level level, const char *fmt, ...);

static const char *s_game_filepath = NULL;
static int state_load(const char *frontend_state_filepath);
static int state_save(const char *frontend_state_filepath);

static uint16_t* s_rgb565_convert_buffer = NULL;
static void enable_xrgb8888_support();
static void convert_xrgb8888_to_rgb565(void* buffer, unsigned width, unsigned height, size_t stride);

static void wrap_video_refresh_cb(const void *data, unsigned width, unsigned height, size_t pitch);
static void xrgb8888_video_refresh_cb(const void *data, unsigned width, unsigned height, size_t pitch);
static bool g_xrgb888 = false;
static int16_t wrap_input_state_cb(unsigned port, unsigned device, unsigned index, unsigned id);

static bool g_show_fps = false;
static bool g_fps_counter = false;
static void frameskip_cb(BOOL flag);
static bool g_per_state_srm = false;
static bool g_per_core_srm = false;
static bool g_enable_savestate_hotkeys = true;
static bool g_enable_screenshot_hotkey = true;
static bool g_enable_osd = true;
static bool g_osd_small_messages = false;

static void dummy_retro_run(void);

// FPS Counter
#if defined(SF2000)
int *fw_fps_counter_enable = (int *)0x80c0b5e0;	// displayfps
static int *fw_fps_counter = (int *)0x80c0b5dc;
static char *fw_fps_counter_format = (char *)0x8099bdf0;	// "%2d/%2d"
#elif defined(GB300V2)
int *fw_fps_counter_enable = (int *)0x80c5abcc;	// displayfps
static int *fw_fps_counter = (int *)0x80c5abc8;
static char *fw_fps_counter_format = (char *)0x809fb9ec;	// "%2d/%2d"
#endif

#define KEYMAP_SIZE 12

// Forward declarations to fix implicit declaration warnings
void build_game_config_filepath(char *filepath, size_t size, const char *game_filepath, const char *library_name);
void config_add_file(const char *filepath);
void save_srm(const char slot);
void load_srm(const char slot);
void shutdown_game(void);
extern char jal_run_emulator_menu;

static bool gb_temporary_osd = false;

static uint16_t* rgb565_darken_buffer = NULL;
static bool g_enable_darken_filter = true;
static bool g_enable_darken_hotkey = true;
static int buffer_prev_width = 0, buffer_prev_height = 0, g_darken_percentage = 0;

static bool g_swap_controllers = false;
static bool g_swap_gameboy = true;
static bool g_enable_ctrlswp_hotkey = true;
static bool variable_update_flag = false;
static bool g_enable_frogui_patch = true;

#define MAX_CONTENT_INFO_OVERRIDES 3

static const struct retro_system_content_info_override* content_info_overrides[MAX_CONTENT_INFO_OVERRIDES];
static size_t content_info_override_count = 0;

// BMP Header Structures
#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BMPHeader;

typedef struct {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BMPInfoHeader;
#pragma pack(pop)

static int screenshot_counter = 1; // Counter for screenshot filenames
static bool capture_screenshot = false;  // Capture screenshot flag

struct retro_core_t core_exports = {
   .retro_init = wrap_retro_init,
   .retro_deinit = wrap_retro_deinit,
   .retro_api_version = retro_api_version,
   .retro_get_system_info = retro_get_system_info,
   .retro_get_system_av_info = retro_get_system_av_info,
   .retro_set_environment = wrap_retro_set_environment,
   .retro_set_video_refresh = retro_set_video_refresh,
   .retro_set_audio_sample = retro_set_audio_sample,
   .retro_set_audio_sample_batch = retro_set_audio_sample_batch,
   .retro_set_input_poll = retro_set_input_poll,
   .retro_set_input_state = retro_set_input_state,
   .retro_set_controller_port_device = retro_set_controller_port_device,
   .retro_reset = retro_reset,
   .retro_run = wrap_retro_run,
   .retro_serialize_size = retro_serialize_size,
   .retro_serialize = retro_serialize,
   .retro_unserialize = retro_unserialize,
   .retro_cheat_reset = retro_cheat_reset,
   .retro_cheat_set = retro_cheat_set,
   .retro_load_game = wrap_retro_load_game,
   .retro_load_game_special = retro_load_game_special,
   .retro_unload_game = wrap_retro_unload_game,
   .retro_get_region = retro_get_region,
   .retro_get_memory_data = retro_get_memory_data,
   .retro_get_memory_size = retro_get_memory_size,
};

struct retro_game_info saved_game_info;
struct retro_system_info sysinfo;
struct retro_game_info_ext game_info_ext;

// Make a directory if it doesn't exist
int create_dir(const char *path) {
	if (fs_access(path, 0) != 0) {
		xlog("filepath: creating %s\n", path);
        fs_mkdir(path, 0755);
    }
}

// Loads the keymap configuration for the game.
void load_keymap(const char *s_game_filepath)
{
	char kmp_filepath[MAXPATH];

	char basename[MAXPATH];
	fill_pathname_base(basename, s_game_filepath, sizeof(basename));
	path_remove_extension(basename);

	// Rom keymap
	snprintf(kmp_filepath, sizeof(kmp_filepath), "%s/%s/keymaps/%s.kmp", CONFIG_DIRECTORY, sysinfo.library_name, basename);
	xlog("Checking keymap in %s...\n", kmp_filepath);

	// if ROM keymap doesn't exist load Core keymap
	if (fs_access(kmp_filepath, 0) != 0) {
		snprintf(kmp_filepath, sizeof(kmp_filepath), "%s/%s/%s.kmp", CONFIG_DIRECTORY, sysinfo.library_name, sysinfo.library_name);
		xlog("Checking keymap in %s...\n", kmp_filepath);
	}

	// if Core keymap doesn't exist load Multicore keymap
	if (fs_access(kmp_filepath, 0) != 0) {
		snprintf(kmp_filepath, sizeof(kmp_filepath), "%s/multicore.kmp", CONFIG_DIRECTORY);
		xlog("Checking keymap in %s...\n", kmp_filepath);
	}

	// if no keymap, just return
	if (fs_access(kmp_filepath, 0) != 0) return;

	uint32_t keymap[KEYMAP_SIZE];
	FILE *h_file = NULL;
	h_file = fopen(kmp_filepath, "rb");

	size_t elements_read = fread(keymap, sizeof(uint32_t), KEYMAP_SIZE, h_file);
	fclose(h_file);

    set_keymap(keymap, 8);
	xlog("Keymap file %s loaded\n", kmp_filepath);
}

void build_srm_filepath(char *filepath, size_t size, const char *game_filepath, const char *extension, size_t extension_size) {
	char basename[MAXPATH];
	char directory[MAXPATH] = SAVE_DIRECTORY;
	fill_pathname_base(basename, game_filepath, sizeof(basename));
	path_remove_extension(basename);
	create_dir(directory); // Make sure SAVE_DIRECTORY exists 

	if(g_per_core_srm){
		snprintf(directory, size, "%s/%s", SAVE_DIRECTORY, sysinfo.library_name);
		create_dir(directory);	// Make sure SAVE_DIRECTORY/sysinfo.library_name exists 
	}

	snprintf(filepath, size, "%s/%s.%s", directory, basename, extension);
}

#ifdef DUAL_SAVE
void load_srm_id(const char slot, int position);
void save_srm_id(const char slot, int position);
void save_srm(const char slot){
	int count = retro_dual_emulated_count();
	for(int i = 0; i < count; i++){
		save_srm_id(slot, i);
	}
}
void save_srm_id(const char slot, int position){
	char ram_filepath[MAXPATH];
	char ext[9];
	if(position == 0){
		snprintf(ext, 5, "srm%c", slot);
	}else{
		if(slot == 0){
			snprintf(ext, 9, "srm_%d", position + 1);
		}else{
			snprintf(ext, 9, "srm_%d_%c", position + 1, slot);
		}
	}
	build_srm_filepath(ram_filepath, sizeof(ram_filepath), s_game_filepath, ext, 8);
	xlog("save_srm: file=%s\n", ram_filepath);
	size_t save_size = retro_dual_get_sram_size(position);
	if(save_size == 0)
		return;
	FILE *ram_file = fopen(ram_filepath, "wb");
	if (!ram_file)
		return;
	fwrite(retro_dual_get_sram_ptr(position), save_size, 1, ram_file);
	fclose(ram_file);
	fs_sync(ram_filepath);
}
void load_srm(const char slot){
	int count = retro_dual_emulated_count();
	for(int i = 0; i < count; i++){
		load_srm_id(slot, i);
	}
}
void load_srm_id(const char slot, int position){
	size_t save_size = retro_dual_get_sram_size(position);
	char ram_filepath[MAXPATH];
	char ext[9];
	if(position == 0){
		snprintf(ext, 5, "srm%c", slot);
	}else{
		if(slot == 0){
			snprintf(ext, 9, "srm_%d", position + 1);
		}else{
			snprintf(ext, 9, "srm_%d_%c", position + 1, slot);
		}
	}
	build_srm_filepath(ram_filepath, sizeof(ram_filepath), s_game_filepath, ext, 8);
	xlog("load_srm: file=%s\n", ram_filepath);
	FILE *ram_file = fopen(ram_filepath, "rb");
	if (!ram_file)
		return;
	fseeko(ram_file, 0, SEEK_END);
	size_t ram_file_size = ftell(ram_file);
	fseeko(ram_file, 0, SEEK_SET);
	if(ram_file_size < save_size){
		save_size = ram_file_size;
	}
	if(save_size == 0){
		fclose(ram_file);
		return;
	}
	fread(retro_dual_get_sram_ptr(position), 1, save_size, ram_file);
	fclose(ram_file);
}
#endif

#ifndef DUAL_SAVE
void save_srm(const char slot){
	char ram_filepath[MAXPATH];
	char ext[5];
	snprintf(ext, 5, "srm%c", slot);
	build_srm_filepath(ram_filepath, sizeof(ram_filepath), s_game_filepath, ext, 4);
	xlog("save_srm: file=%s\n", ram_filepath);
	size_t save_size = retro_get_memory_size(RETRO_MEMORY_SAVE_RAM);
	if(save_size == 0)
		return;
	FILE *ram_file = fopen(ram_filepath, "wb");
	if (!ram_file)
		return;
	fwrite(retro_get_memory_data(RETRO_MEMORY_SAVE_RAM), save_size, 1, ram_file);
	fclose(ram_file);
	fs_sync(ram_filepath);
}
void load_srm(const char slot){
	size_t save_size = retro_get_memory_size(RETRO_MEMORY_SAVE_RAM);
	char ram_filepath[MAXPATH];
	char ext[5];
	snprintf(ext, 5, "srm%c", slot);
	build_srm_filepath(ram_filepath, sizeof(ram_filepath), s_game_filepath, ext, 4);
	xlog("load_srm: file=%s\n", ram_filepath);
	FILE *ram_file = fopen(ram_filepath, "rb");
	if (!ram_file)
		return;
	fseeko(ram_file, 0, SEEK_END);
	size_t ram_file_size = ftell(ram_file);
	fseeko(ram_file, 0, SEEK_SET);
	if(ram_file_size < save_size){
		save_size = ram_file_size;
	}
	if(save_size == 0){
		fclose(ram_file);
		return;
	}
	fread(retro_get_memory_data(RETRO_MEMORY_SAVE_RAM), 1, save_size, ram_file);
	fclose(ram_file);
}
#endif

static uint32_t g_osd_time = 0;
static uint32_t slot_delay_time = 0;
static int slot_state = 0;
char osd_message[MAXPATH];

// Show osd message
int show_osd_message(const char *message) {
	if (g_enable_osd) {
		gb_temporary_osd = true;
		if (!g_fps_counter) *fw_fps_counter_enable = 1; // Don't change fps if fps is enabled
		sprintf(fw_fps_counter_format, message);
		g_osd_time = os_get_tick_count();
	}
}

bool config_entry_exists(const config_file_t *conf, const char *key)
{
	struct config_entry_list *entry;
    for (entry = conf->entries; entry; entry = entry->next)
    {
        if (strcmp(entry->key, key) == 0)
            return true;
    }

    return false;
}

void wrap_retro_run(void) {
	// Disable the osd message after 2 seconds
	if (gb_temporary_osd) {
		if (os_get_tick_count() - g_osd_time > 1000) {
			if (!g_fps_counter) *fw_fps_counter_enable = 0; // Don't change fps if fps is enabled
			gb_temporary_osd = false;
		}
	}

	// Do not use retro_input_state_cb to avoid key remapping issues
    // Use osd delay to prevent spamming hotkeys
	// Reset g_joy_state to avoid button presses
    switch (g_joy_task_state) {
        case HOTKEYSCREENSHOT:
			// Screenshot
			g_joy_state = 0; 
			if (os_get_tick_count() - g_osd_time > 1000 && g_enable_screenshot_hotkey) {
                capture_screenshot = true;
            }
            break;
                
        case HOTKEYSAVESTATE:
			// Save current state
			g_joy_state = 0; 
			if (os_get_tick_count() - g_osd_time > 1000 && g_enable_savestate_hotkeys) {
                state_save("");
                g_osd_small_messages ? sprintf(osd_message, "S:%d", slot_state) : sprintf(osd_message, "Save: %d", slot_state);
                show_osd_message(osd_message);
            }
            break;
                
        case HOTKEYLOADSTATE:
			// Load current state
			g_joy_state = 0; 
			if (os_get_tick_count() - g_osd_time > 1000 && g_enable_savestate_hotkeys) {
                state_load("");
                g_osd_small_messages ? sprintf(osd_message, "L:%d", slot_state) : sprintf(osd_message, "Load: %d", slot_state);
                show_osd_message(osd_message);
            }
            break;
                
        case HOTKEYINCREASESTATE:
            // Increase state
			g_joy_state = 0; 
            if (os_get_tick_count() - slot_delay_time > 100 && g_enable_savestate_hotkeys) {
                if (slot_state < 9) {
                    slot_state += 1;
                } else {
                    slot_state = 0;
                }
                    	
                g_osd_small_messages ? sprintf(osd_message, "SLT:%d", slot_state) : sprintf(osd_message, "Slot: %d", slot_state);
                show_osd_message(osd_message);
                slot_delay_time = os_get_tick_count();
            }
            break;

        case HOTKEYDECREASESTATE:
            // Decrease state
			g_joy_state = 0; 
            if (os_get_tick_count() - slot_delay_time > 100 && g_enable_savestate_hotkeys) {
                if (slot_state > 0) {
                    slot_state -= 1;
                } else {
                    slot_state = 9;
                }
                    
                g_osd_small_messages ? sprintf(osd_message, "SLT:%d", slot_state) : sprintf(osd_message, "Slot: %d", slot_state);
                show_osd_message(osd_message);
                slot_delay_time = os_get_tick_count();
            }
            break;

		case HOTKEYINCREASEDARKEN:
            // Increase state
			g_joy_state = 0; 
            if (os_get_tick_count() - slot_delay_time > 100 && g_enable_darken_filter && g_enable_darken_hotkey) {
                if (g_darken_percentage < 100) {
                    g_darken_percentage += 1;
                } 
				
                g_osd_small_messages ? sprintf(osd_message, "DF:%d", g_darken_percentage) : sprintf(osd_message, "Darken: %d", g_darken_percentage);
                show_osd_message(osd_message);
                slot_delay_time = os_get_tick_count();
            }
            break;

        case HOTKEYDECREASEDARKEN:
            // Decrease state
			g_joy_state = 0; 
            if (os_get_tick_count() - slot_delay_time > 100 && g_enable_darken_filter && g_enable_darken_hotkey) {
                if (g_darken_percentage > 0) {
                    g_darken_percentage -= 1;
                }
				
                g_osd_small_messages ? sprintf(osd_message, "DF:%d", g_darken_percentage) : sprintf(osd_message, "Darken: %d", g_darken_percentage);
                show_osd_message(osd_message);
                slot_delay_time = os_get_tick_count();
            }
            break;
                
		case HOTKEYSWAPPLAYER:
            // Swap Controller
			g_joy_state = 0;
			if (os_get_tick_count() - g_osd_time > 1000 && g_enable_ctrlswp_hotkey) {
				char single_screen_mp[32];
				char audio_output[32];
				strcpy(single_screen_mp, "tgbdual_single_screen_mp");
				strcpy(audio_output, "tgbdual_audio_output");
				if (strcmp(sysinfo.library_name, "DoubleCherryGB") == 0) strcpy(single_screen_mp, "dcgb_single_screen_mp");
				if (strcmp(sysinfo.library_name, "DoubleCherryGB") == 0) strcpy(audio_output, "dcgb_audio_output");

				if (g_swap_controllers) {
					if (config_entry_exists(s_core_config, single_screen_mp) 
    					&& (strcmp(sysinfo.library_name, "DoubleCherryGB") == 0 || strcmp(sysinfo.library_name, "TGB Dual") == 0) && g_swap_gameboy) {
						config_set_string(s_core_config, single_screen_mp, "player 1 only");
						config_set_string(s_core_config, audio_output, "Game Boy #1");
						variable_update_flag = true;
					}
					g_swap_controllers = false;
					g_osd_small_messages ? sprintf(osd_message, "P1") : sprintf(osd_message, "Player 1");
				} else {
					if (config_entry_exists(s_core_config, single_screen_mp) 
    					&& (strcmp(sysinfo.library_name, "DoubleCherryGB") == 0 || strcmp(sysinfo.library_name, "TGB Dual") == 0) && g_swap_gameboy) {
						config_set_string(s_core_config, single_screen_mp, "player 2 only");
						config_set_string(s_core_config, audio_output, "Game Boy #2");
						variable_update_flag = true;
					}
					g_swap_controllers = true;
					g_osd_small_messages ? sprintf(osd_message, "P2") : sprintf(osd_message, "Player 2");
				}
				
                show_osd_message(osd_message);
                slot_delay_time = os_get_tick_count();
            }
            break;

        default:
            break;
	}

	retro_run(); 
}

void wrap_retro_unload_game(void){
	save_srm(0);
	retro_unload_game();
}

static void clear_bss()
{
	extern void *__bss_start;
	extern void *_end;

    void *start = &__bss_start;
    void *end = &_end;

	memset(start, 0, end - start);

	// xlog("clear_bss: start=%p end=%p\n", &__bss_start, &_end);
}

// call_ctors currently is not being used since __libc_init_array will handle that instead.
// but leave it here for now if there would be a need to debug a crash during the static init phase.
static void call_ctors()
{
	typedef void (*func_ptr) (void);
	extern func_ptr __init_array_start;
	extern func_ptr __init_array_end;

	xlog("call_ctors: start=%p end=%p\n", &__init_array_start, &__init_array_end);

	// call ctors from last to first
	for (func_ptr *pfunc = &__init_array_end - 1; pfunc >= &__init_array_start; --pfunc) {
		xlog("pfunc=%p func=%p\n", pfunc, *pfunc);
		(*pfunc)();
	}
}

// TODO: need a place to call dtors as well. maybe when retro_deinit is called?
static void call_dtors()
{
	typedef void (*func_ptr) (void);
	extern func_ptr __fini_array_start;
	extern func_ptr __fini_array_end;

	// dtors are called in reverse order of ctors
	for (func_ptr *pfunc = &__fini_array_start; pfunc < &__fini_array_end; ++pfunc) {
		(*pfunc)();
	}
}

// Function to load a raw RGB565 image into the framebuffer
int load_rgb565_image(const char* filename, uint16_t* framebuffer, int width, int height) {
    // Open the raw image file
    FILE* file = fopen(filename, "rb");
    if (file == NULL)
    {
        xlog("Error opening file: %s\n", filename);
        return -1;
    }

    // Calculate the number of bytes to read (width * height * 2 bytes per pixel)
    size_t image_size = width * height * sizeof(uint16_t);

    // Read the raw RGB565 image data into the framebuffer
    size_t bytes_read = fread(framebuffer, 1, image_size, file);
    if (bytes_read != image_size)
    {
        xlog("Error reading the image file\n");
        fclose(file);
        return -1;
    }

    // Close the file after reading the image
    fclose(file);

    return 0;
}

// Wrap run_game for FrogUI or other menu cores so we can unload game and deinit
void wrap_run_game(const char *filename, int load_state) {
	wrap_retro_unload_game();
	wrap_retro_deinit();
	run_game(filename, load_state);
}

// Run the Menu core as a method of exiting
void shutdown_game(void) {
	// Calculate the size of the framebuffer and allocate memory for the framebuffer
    size_t framebuffer_size = 640 * 480 * sizeof(uint16_t);
    uint16_t* framebuffer = (uint16_t*)malloc(framebuffer_size);

    // Initialize the framebuffer to black so we black out the screen before loading a new game
    memset(framebuffer, 0x0000, framebuffer_size);

	// Calculate pixel pitch
	uint16_t pixel_pitch = 640 * sizeof(uint16_t);

	// Write framebuffer to screen then free it
	run_screen_write(framebuffer, 640, 480, pixel_pitch); // Is this the proper way to do this?
	free(framebuffer);

	// Prepare the loader to load the menu
	strcpy(ptr_gs_run_game_file, "menu;p.gba");
	strcpy(ptr_gs_run_game_name, "FrogUI");

	// Unload the game and deinit before running the menu
	wrap_retro_unload_game();
	wrap_retro_deinit();
	run_game("/mnt/sda1/ROMS/menu;p.gba", 0);
}

// Patch pause menu
int dummy_run_emulator_menu(void) {
	save_srm(0); // Save before loading the pause menu (incase of crashes + replace savesrm hotkey)
	unsigned int run_emulator_menu_response = run_emulator_menu();
	if (g_enable_frogui_patch) {
		if (run_emulator_menu_response == 1) { // Replace quit button
			shutdown_game();
		}
		return 0;
	} else return run_emulator_menu_response;
}

// __core_entry__ must be placed at a known location in the binary (at the beginning)
// so that when the loader actually loads the binary into mem address 0x87000000,
// then __core_entry__ will be the first function there for the loader to call.
struct retro_core_t *__core_entry__(void) __attribute__((section(".init.core_entry")));

struct retro_core_t *__core_entry__(void)
{
	os_disable_interrupt();
	// https://gitlab.com/kobily/sf2000_multicore/-/commit/328bce4173316a6ea0afe4c360ee4f3d5b951c19 condensed to core's side with $gp value fetched from _start
	// Values are the same between gb300 V2 and SF2000
	*(unsigned *)0x80049744 = *(unsigned *)0x80001270; // lui	$gp
	*(unsigned *)0x80049748 = *(unsigned *)0x80001274; // addiu	$gp
	__builtin___clear_cache((void *)0x80049744, (void *)0x8004974c);

	// Patch Pause Menu
	PATCH_JAL((uintptr_t)&jal_run_emulator_menu, dummy_run_emulator_menu);
	__builtin___clear_cache(&jal_run_emulator_menu, &jal_run_emulator_menu+4);
	os_enable_interrupt();
	clear_bss();

	extern void __sinit (struct _reent *);
	extern void __libc_init_array (void);

	_REENT_INIT_PTR(_REENT);
	__sinit(_REENT);
	__libc_init_array();

	xlog("libc initialized\n");

	return &core_exports;
}

static bool str_eq_ci(const char *a, const char *b, size_t len) {
    for (size_t i = 0; i < len; i++)
    {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i]))
            return false;
    }
    return true;
}

static bool extension_in_list_ci(const char *ext, const char *list) {
    if (!ext || !list) return false;

    const char *start = list;
    const char *end;

    while (*start)
    {
        end = strchr(start, '|');
        if (!end) end = start + strlen(start);

        size_t len = end - start;

        if (strlen(ext) == len && str_eq_ci(ext, start, len))
            return true;

        if (*end == '\0')
            break;
        start = end + 1;
    }

    return false;
}

bool extension_supports_no_fullpath(const char *ext) {
    for (size_t i = 0; i < content_info_override_count; i++) {
        const struct retro_system_content_info_override *ovr = content_info_overrides[i];

        if (!ovr->need_fullpath && extension_in_list_ci(ext, ovr->extensions))
            return true;
    }
    return false;
}

// Unzip a file into a buffer
void unzip_file(const char* path, void* buffer, bool is_wqw, uint32_t preview_size) {
	uint32_t unwqw_init_response = unwqw_init(path,preview_size,2);
    unwqw_decompress(unwqw_init_response,0,buffer,0,3);
	unwqw_free(unwqw_init_response);
}

// Check if a file is .zip or WQW (.zfc, .zsf, .zmd, .zgb, .zpc)
bool is_zip_wqw_file(const char* path, bool* is_wqw) {
	*is_wqw = false;

    size_t len = strlen(path);
    if (len <= 4) return false;

    const char* ext = path + len - 4;

    if (str_eq_ci(ext, ".zip", 4)) {
        return true;
    }

    // WQW extensions
    const char* wqw_exts[] = { ".zfc", ".zsf", ".zmd", ".zgb", ".zpc" };
    for (size_t i = 0; i < sizeof(wqw_exts)/sizeof(wqw_exts[0]); i++) {
        if (str_eq_ci(ext, wqw_exts[i], 4)) {
            *is_wqw = true;
            return true;
        }
    }

    return false;
}

void dump_zip_header(const char *zip_path, bool is_wqw, uint32_t preview_size, zip_local_file_header *zip_header) {
    FILE *zip_file = fopen(zip_path, "rb");
    if (!zip_file) {
        xlog("Failed to open ZIP file\n");
        return;
    }

	// If this is a WQW file, skip preview bytes
    if (is_wqw && preview_size > 0) {
        if (fseek(zip_file, preview_size, SEEK_SET) != 0) {
            xlog("Failed to skip preview bytes: %u\n", preview_size);
            fclose(zip_file);
            return;
        }
        xlog("Skipped %u preview bytes for WQW file\n", preview_size);
    }

    // Read fields from the header
	fread(&zip_header->signature, sizeof(uint32_t), 1, zip_file);
    fread(&zip_header->version_needed, sizeof(uint16_t), 1, zip_file);
    fread(&zip_header->general_flag, sizeof(uint16_t), 1, zip_file);
    fread(&zip_header->compression_method, sizeof(uint16_t), 1, zip_file);
    fread(&zip_header->last_mod_time, sizeof(uint16_t), 1, zip_file);
    fread(&zip_header->last_mod_date, sizeof(uint16_t), 1, zip_file);
    fread(&zip_header->crc32, sizeof(uint32_t), 1, zip_file);
    fread(&zip_header->compressed_size, sizeof(uint32_t), 1, zip_file);
    fread(&zip_header->uncompressed_size, sizeof(uint32_t), 1, zip_file);
    fread(&zip_header->filename_length, sizeof(uint16_t), 1, zip_file);
	fread(&zip_header->extra_field_length, sizeof(uint16_t), 1, zip_file);

    // Read the filename
    zip_header->filename = (char *)malloc(zip_header->filename_length + 1);  // +1 for null terminator
    fread(zip_header->filename, sizeof(char), zip_header->filename_length, zip_file);
    zip_header->filename[zip_header->filename_length] = '\0';  // Null-terminate the string

	// If the signature is 0x57515703 (0x03575157 little endian) (obfuscated WQW), undo XOR 0xE5 on the filename
    if (zip_header->signature == 0x03575157) {
        for (int i = 0; i < zip_header->filename_length; i++) {
            zip_header->filename[i] ^= 0xE5;
        }
    }

    fclose(zip_file);
}

void extract_extension(const char *filename, char **extension) {
    char *dot = strrchr(filename, '.');
	if (dot == NULL) {
        *extension = NULL;
    } else {
        *extension = strdup(dot + 1);
    }
}

void extract_path_components(const char *filepath, char **dir, char **filename, char **extension) {
    // Copy filepath to avoid modifying the original string
    char *path_copy = strdup(filepath);

    // Extract directory part by finding the last slash
    char *last_slash = strrchr(path_copy, '/');
    *last_slash = '\0';
    *dir = strdup(path_copy);
    *filename = strdup(last_slash + 1);
    extract_extension(*filename, extension);

	if (*extension != NULL) {
        char *dot = strrchr(*filename, '.');
        if (dot != NULL) {
            *dot = '\0';
        }
    }

    free(path_copy);
}

bool wrap_retro_load_game(const struct retro_game_info* info) {
	memcpy(&saved_game_info, info, sizeof(struct retro_game_info)); // Save the retro_game_info incase we want to reset

	bool ret;
	xlog("core=%s-%s need_fullpath=%d\n", sysinfo.library_name, sysinfo.library_version, sysinfo.need_fullpath);

	// no need to strcpy, because info->path string should be valid during the execution of the game
	s_game_filepath = info->path;

	char config_game_filepath[MAXPATH];
	build_game_config_filepath(config_game_filepath, sizeof(config_game_filepath), s_game_filepath, sysinfo.library_name);

	// load per game options
	config_add_file(config_game_filepath);

	// setup load/save state handlers
	gfn_state_load = state_load;
	gfn_state_save = state_save;

	gfn_frameskip = NULL;

	// install custom input handler to filter out all requests for non-joypad devices
	retro_set_input_state(wrap_input_state_cb);

	// intercept audio output to mix stereo into mono
	retro_set_audio_sample(mono_mix_audio_sample_cb);
	retro_set_audio_sample_batch(mono_mix_audio_batch_cb);

	FILE *hfile = fopen(info->path, "rb");
	if (!hfile) {
		xlog("[core] Error opening rom file=%s\n", info->path);
		return false;
	}

	void *buffer;
	long size;
	char *dir = NULL;
    char *filename = NULL;
    char *extension = NULL;
	bool is_wqw;
	bool core_supports_rom_in_buffer = false;
	zip_local_file_header zip_header;

	extract_path_components(info->path, &dir, &filename, &extension);
	core_supports_rom_in_buffer = extension_supports_no_fullpath(extension);

	if (is_zip_wqw_file(info->path, &is_wqw)) {
		g_preview_size = 0;
		if (is_wqw) g_preview_size = g_preview_height * g_preview_width * 2;
		dump_zip_header(info->path, is_wqw, g_preview_size, &zip_header);
		size = zip_header.uncompressed_size;
		// Extended game info
		game_info_ext.file_in_archive = true;
		game_info_ext.archive_path = info->path;
		game_info_ext.archive_file = zip_header.filename;
		extract_extension(zip_header.filename, &extension);
		core_supports_rom_in_buffer = extension_supports_no_fullpath(extension);
		if (!sysinfo.need_fullpath || core_supports_rom_in_buffer) {
			buffer = malloc(size);
			unzip_file(info->path, buffer, is_wqw, g_preview_size);
		}
	} else {
		fseeko(hfile, 0, SEEK_END);
		size = ftell(hfile);
		fseeko(hfile, 0, SEEK_SET);
		if (!sysinfo.need_fullpath || core_supports_rom_in_buffer) {
			buffer = malloc(size);
			fread(buffer, 1, size, hfile);
		}
		fclose(hfile);
		// Extended game info
		game_info_ext.file_in_archive = false;
	}

	if (sysinfo.need_fullpath && !core_supports_rom_in_buffer) xlog("core loads content directly from file\n");

	struct retro_game_info gameinfo;
	gameinfo.path = info->path;
	if (!sysinfo.need_fullpath || core_supports_rom_in_buffer) {
		gameinfo.data = buffer;
		xlog("game loaded into temp buffer. size=%u\n", size);
	} else gameinfo.data = info->data;
	gameinfo.size = size;

	// Extended game info
	game_info_ext.persistent_data = false;
	game_info_ext.full_path = info->path;
	if (!sysinfo.need_fullpath || core_supports_rom_in_buffer) game_info_ext.data = buffer;
	else game_info_ext.data = info->data;
	game_info_ext.size = size;
	game_info_ext.dir = dir;
	game_info_ext.name = filename;
	game_info_ext.ext = extension;

	ret = retro_load_game(&gameinfo);

	if (!sysinfo.need_fullpath || core_supports_rom_in_buffer) free(buffer);
	free(dir);
    free(filename);
    free(extension);

	if (!ret)
	{
		xlog("retro_load_game failed\n");
		gfn_retro_run = dummy_retro_run;
	}
	else
	{
		xlog("retro_load_game ok\n");
		
		// load keymap
		load_keymap(s_game_filepath);

		video_options(s_core_config);

		// Frogui pause menu patch
		config_get_bool(s_core_config, "sf2000_enable_frogui_patch", &g_enable_frogui_patch);

		// show FPS
		config_get_bool(s_core_config, "sf2000_show_fps", &g_show_fps);

		// per state srm
		config_get_bool(s_core_config, "sf2000_per_state_srm", &g_per_state_srm);
		// per core srm
		config_get_bool(s_core_config, "sf2000_per_core_srm", &g_per_core_srm);

		// Hotkey settings
		config_get_bool(s_core_config, "sf2000_enable_savestate_hotkeys", &g_enable_savestate_hotkeys);
		config_get_bool(s_core_config, "sf2000_enable_screenshot_hotkey", &g_enable_screenshot_hotkey);
		config_get_bool(s_core_config, "sf2000_enable_osd", &g_enable_osd);
		config_get_bool(s_core_config, "sf2000_osd_small_messages", &g_osd_small_messages);
		
		// Darkening filter
		config_get_bool(s_core_config, "sf2000_enable_darken_filter", &g_enable_darken_filter);
		config_get_bool(s_core_config, "sf2000_enable_darken_hotkey", &g_enable_darken_hotkey);
		config_get_uint(s_core_config, "sf2000_darken_percentage", &g_darken_percentage);

		// Controller swap
		config_get_bool(s_core_config, "sf2000_swap_controllers", &g_swap_controllers);
		config_get_bool(s_core_config, "sf2000_swap_gameboy", &g_swap_gameboy);
		config_get_bool(s_core_config, "sf2000_enable_ctrlswp_hotkey", &g_enable_ctrlswp_hotkey);

		// Make sure the first two controllers are configured as gamepads
		retro_set_controller_port_device(0, RETRO_DEVICE_JOYPAD);
		retro_set_controller_port_device(1, RETRO_DEVICE_JOYPAD);

		load_srm(0);
	}

	return ret;
}

void wrap_retro_set_environment(retro_environment_t cb)
{
	retro_set_environment(wrap_environ_cb);
}

bool wrap_environ_cb(unsigned cmd, void *data)
{
	switch (cmd)
	{
		case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
		{
			struct retro_log_callback *cb = (struct retro_log_callback*)data;
			cb->log = log_cb;
			return true;
		}

		case RETRO_ENVIRONMENT_SET_MESSAGE:
		case RETRO_ENVIRONMENT_SET_MESSAGE_EXT:
		{
			const struct retro_message *msg = (const struct retro_message*)data;
			log_cb(RETRO_LOG_INFO, "[Environ]: SET_MESSAGE: %s\n", msg->msg);
			return true;
		}

		case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
		{
			const char *dir = SYSTEM_DIRECTORY;
			*(const char**)data = dir;
			log_cb(RETRO_LOG_INFO, "[Environ]: SYSTEM_DIRECTORY: \"%s\"\n", dir);
			return true;
		}

		case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
		{
			const char *dir = SAVE_DIRECTORY;
			*(const char**)data = dir;
			log_cb(RETRO_LOG_INFO, "[Environ]: SAVE_DIRECTORY: \"%s\"\n", dir);
			return true;
		}

		case RETRO_ENVIRONMENT_GET_CAN_DUPE:
			*(bool*)data = true;
			log_cb(RETRO_LOG_INFO, "[Environ]: GET_CAN_DUPE: true\n");
			return true;

		case RETRO_ENVIRONMENT_GET_VARIABLE:
		{
			struct retro_variable *var = (struct retro_variable*)data;
			bool ret = config_get_var(var);
			log_cb(RETRO_LOG_INFO, "[Environ]: GET_VARIABLE: %s=%s\n", var->key, ret ? var->value : "");
			return ret;
		}

		case RETRO_ENVIRONMENT_SET_MEMORY_MAPS:
		{
			log_cb(RETRO_LOG_INFO, "[Environ]: SET_MEMORY_MAPS\n");
			break;
		}

		case RETRO_ENVIRONMENT_SET_GEOMETRY:
		{
			const struct retro_game_geometry *geom = (const struct retro_game_geometry*)data;
            log_cb(RETRO_LOG_INFO, "[Environ]: SET_GEOMETRY: %ux%u, Aspect: %.3f.\n",
				geom->base_width, geom->base_height, geom->aspect_ratio);
			break;
		}

		case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
		{
			enum retro_pixel_format fmt = *(enum retro_pixel_format*)data;
			if (fmt == RETRO_PIXEL_FORMAT_XRGB8888)
			{
				enable_xrgb8888_support();
				return true;
			}
			break;
		}

		case RETRO_ENVIRONMENT_SET_AUDIO_BUFFER_STATUS_CALLBACK:
		{
			struct retro_audio_buffer_status_callback *buff_status_cb = (struct retro_audio_buffer_status_callback*)data;
			if (buff_status_cb)
			{
				audio_buff_status_cb = buff_status_cb->callback;
				gfn_frameskip = frameskip_cb;
			}
			else
				gfn_frameskip = NULL;

			xlog("support for auto frameskipping %s\n", buff_status_cb ? "enabled" : "disabled" );
			return true;
		}

		case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
		{
            *(bool*)data = variable_update_flag;
            variable_update_flag = false;
            return true;
		}

		case RETRO_ENVIRONMENT_SHUTDOWN:
        {
            log_cb(RETRO_LOG_INFO, "[Environ]: RETRO_ENVIRONMENT_SHUTDOWN\n");
			shutdown_game();
            return true;
        }

		case RETRO_ENVIRONMENT_GET_GAME_INFO_EXT:
    	{
			log_cb(RETRO_LOG_INFO, "[Environ]: GET_GAME_INFO_EXT\n");
        	*(struct retro_game_info_ext**)data = &game_info_ext;
        	log_cb(RETRO_LOG_INFO, "Full Path: %s\n", game_info_ext.full_path ? game_info_ext.full_path : "N/A");
        	log_cb(RETRO_LOG_INFO, "Archive Path: %s\n", game_info_ext.archive_path ? game_info_ext.archive_path : "N/A");
        	log_cb(RETRO_LOG_INFO, "Archive File: %s\n", game_info_ext.archive_file ? game_info_ext.archive_file : "N/A");
        	log_cb(RETRO_LOG_INFO, "Directory: %s\n", game_info_ext.dir ? game_info_ext.dir : "N/A");
        	log_cb(RETRO_LOG_INFO, "Name: %s\n", game_info_ext.name ? game_info_ext.name : "N/A");
        	log_cb(RETRO_LOG_INFO, "Extension: %s\n", game_info_ext.ext ? game_info_ext.ext : "N/A");
        	log_cb(RETRO_LOG_INFO, "Data: %p\n", game_info_ext.data);
        	log_cb(RETRO_LOG_INFO, "Size: %d bytes\n", game_info_ext.size);
        	log_cb(RETRO_LOG_INFO, "File in Archive: %s\n", game_info_ext.file_in_archive ? "Yes" : "No");
        	log_cb(RETRO_LOG_INFO, "Persistent Data: %s\n", game_info_ext.persistent_data ? "Yes" : "No");
        	return true;
    	}

		case RETRO_ENVIRONMENT_SET_CONTENT_INFO_OVERRIDE:
		{
    		const struct retro_system_content_info_override *overrides = (const struct retro_system_content_info_override *)data;
    		content_info_override_count = 0;

    		for (size_t i = 0; overrides[i].extensions; i++) {
        		if (content_info_override_count >= MAX_CONTENT_INFO_OVERRIDES) {
            		log_cb(RETRO_LOG_WARN, "[Environ]: SET_CONTENT_INFO_OVERRIDE: too many overrides, truncating at %zu\n", (size_t)MAX_CONTENT_INFO_OVERRIDES);
            		break;
        		}
        		content_info_overrides[content_info_override_count++] = &overrides[i];
    		}

    		return true;
		}
		
	}
	return retro_environment_cb(cmd, data);
}

void log_cb(enum retro_log_level level, const char *fmt, ...)
{
#if DEBUG_XLOG == 1
	char buffer[500];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	switch (level)
	{
		case RETRO_LOG_DEBUG:
			xlog("[core][DEBUG] %s", buffer);
			break;

		case RETRO_LOG_INFO:
			xlog("[core][INFO] %s", buffer);
			break;

		case RETRO_LOG_WARN:
			xlog("[core][WARN] %s", buffer);
			break;

		case RETRO_LOG_ERROR:
			xlog("[core][ERROR] %s", buffer);
			break;

		default:
			break;
	}
#endif
}


char build_state_filepath(char *state_filepath, size_t size, const char *game_filepath, const char *frontend_state_filepath)
{
//	"/mnt/sda1/ROMS/pce/Alien Crush.pce"	->
//	"/mnt/sda1/saves/savestates/[core]/Alien Crush.state[slot]"

	// last char is the save slot number
	char save_slot = frontend_state_filepath[strlen(frontend_state_filepath) - 1];

	if (strlen(frontend_state_filepath) == 0) save_slot = slot_state + '0'; //To convert integer to char only 0 to 9 will be converted. 

	if (strlen(frontend_state_filepath) != 0) { 
		char frontend_state_directory[MAXPATH];
		strcpy(frontend_state_directory, frontend_state_filepath);
		char *last_slash = strrchr(frontend_state_directory, '/');
		*last_slash = '\0';
		create_dir(frontend_state_directory); // Make sure frontend_state_filepath directory exists
	}

	char basename[MAXPATH];
	char directory[MAXPATH];
	fill_pathname_base(basename, game_filepath, sizeof(basename));
	path_remove_extension(basename);
	create_dir(SAVE_DIRECTORY); // Make sure SAVE_DIRECTORY exists
	if(g_per_core_srm){
		snprintf(directory, size, "%s/%s", SAVE_DIRECTORY, sysinfo.library_name);
		create_dir(directory); // Make sure SAVE_DIRECTORY/sysinfo.library_name exists
	} else {
		snprintf(directory, size, "%s/savestates", SAVE_DIRECTORY);
		create_dir(directory); // Make sure SAVE_DIRECTORY/savestates exists
		strcat(directory, "/");
		strcat(directory, sysinfo.library_name);
		create_dir(directory); // Make sure SAVE_DIRECTORY/savestates/sysinfo.library_name exists
	}
	snprintf(state_filepath, size, "%s/%s.state%c", directory, basename, save_slot);
	return save_slot;
}

int state_load(const char *frontend_state_filepath)
{
	char state_filepath[MAXPATH];
	char slot = build_state_filepath(state_filepath, sizeof(state_filepath), s_game_filepath, frontend_state_filepath);
	xlog("state_load: file=%s\n", state_filepath);

	FILE *file = fopen(state_filepath, "rb");
	if (!file)
		return 0;

	fseeko(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseeko(file, 0, SEEK_SET);

	void *data = malloc(size);

	fread(data, 1, size, file);
	fclose(file);

	retro_unserialize(data, size);

	free(data);

	if(g_per_state_srm){
		load_srm(slot);
	}
	return 1;
}

int state_save(const char *frontend_state_filepath)
{
	char state_filepath[MAXPATH];
	char slot = build_state_filepath(state_filepath, sizeof(state_filepath), s_game_filepath, frontend_state_filepath);
	xlog("state_save: file=%s\n", state_filepath);

	FILE *file = fopen(state_filepath, "wb");
	if (!file)
		return 0;

	size_t size = retro_serialize_size();
	void *data = calloc(size, 1);

	retro_serialize(data, size);

	fwrite(data, size, 1, file);
	fclose(file);

	free(data);

	fs_sync(state_filepath);

	if(g_per_state_srm){
		save_srm(slot);
	}
	return 1;
}

void build_game_config_filepath(char *filepath, size_t size, const char *game_filepath, const char *library_name)
{
	char basename[MAXPATH];
	fill_pathname_base(basename, game_filepath, sizeof(basename));
	path_remove_extension(basename);

	snprintf(filepath, size, "%s/%s/options/%s.opt", CONFIG_DIRECTORY, library_name, basename);
}

void config_add_file(const char *filepath)
{
	bool ret = config_append_file(s_core_config, filepath);
	xlog("config_load: %s %s\n", filepath, ret ? "loaded" : "not found");
}

void config_load()
{
	s_core_config = config_file_new_alloc();

	// load global multicore options
	config_add_file(CONFIG_DIRECTORY "/multicore.opt");

	char config_filepath[MAXPATH];
	snprintf(config_filepath, sizeof(config_filepath), "%s/%s/%s.opt", CONFIG_DIRECTORY, sysinfo.library_name, sysinfo.library_name);

	// load per core options
	config_add_file(config_filepath);
}

void config_free()
{
	config_file_free(s_core_config);
}

bool config_get_var(struct retro_variable *var)
{
	if (!s_core_config)
		return false;

	const struct config_entry_list *entry = config_get_entry(s_core_config, var->key);
	if (!entry)
		return false;

	var->value = entry->value;
	return true;
}

void wrap_retro_init(void)
{	
	retro_get_system_info(&sysinfo);
	config_load();
	retro_set_video_refresh(wrap_video_refresh_cb);
	retro_init();
}

void wrap_retro_deinit(void)
{
	if (g_fps_counter) *fw_fps_counter_enable = 0;
	video_cleanup();
	retro_deinit();
	config_free();

	if (rgb565_darken_buffer) free(rgb565_darken_buffer);
	if (s_rgb565_convert_buffer) free(s_rgb565_convert_buffer);
}

size_t mono_mix_audio_batch_cb(const int16_t *data, size_t frames)
{
	// TODO: is data always assumed to be s16bit dual channel buffer?
	for (size_t i=0; i < frames*2; i+=2)
	{
		// for single speaker output, mix to mono both channels into the first channel
		((int16_t*)data)[i] = (data[i] >> 1) + (data[i+1] >> 1);
		// leave the second channel as is because it is not heard anyway
		//((int16_t*)data)[i+1] = 0;
	}

	// NOTE: stock frontend audio_batch_cb always return 0!
	retro_audio_sample_batch_cb(data, frames);
	// return `frames` as if all data was consumed
	return frames;
}

void mono_mix_audio_sample_cb(int16_t left, int16_t right)
{
	int16_t mixed = (left >> 1) + (right >> 1);
	int16_t data[2] = {mixed, right};
	retro_audio_sample_batch_cb(data, 1);
}

void convert_xrgb8888_to_rgb565(void* buffer, unsigned width, unsigned height, size_t stride)
{
	uint32_t* xrgb8888_buffer = (uint32_t*)buffer;
	uint16_t* rgb565_buffer = s_rgb565_convert_buffer;

    for (int y = 0; y < height; y++)
	{
        for (int x = 0; x < width; x++)
		{
            int index = y * stride / sizeof(uint32_t) + x;

            uint32_t xrgbPixel = xrgb8888_buffer[index];

            uint16_t rgb565Pixel = ((xrgbPixel >> 8) & 0xF800) | ((xrgbPixel >> 5) & 0x07E0) | ((xrgbPixel >> 3) & 0x001F);

            rgb565_buffer[index] = rgb565Pixel;
        }
    }
}

void xrgb8888_video_refresh_cb(const void *data, unsigned width, unsigned height, size_t pitch)
{
	convert_xrgb8888_to_rgb565((void*)data, width, height, pitch);

	retro_video_refresh_cb(s_rgb565_convert_buffer, width, height, width * 2);		// each pixel is now 16bit, so pass the pitch as width*2
}

static void enable_xrgb8888_support()
{
	xlog("support for XRGB8888 enabled\n");

	struct retro_system_av_info av_info;
	retro_get_system_av_info(&av_info);

	s_rgb565_convert_buffer = (uint16_t*)malloc(av_info.geometry.max_width * av_info.geometry.max_height * sizeof(uint16_t));

	xlog("created rgb565_convert_buffer=%p width=%u height=%u\n",
		s_rgb565_convert_buffer, av_info.geometry.max_width, av_info.geometry.max_height);

	g_xrgb888 = true;
}

static int16_t wrap_input_state_cb(unsigned port, unsigned device, unsigned index, unsigned id)
{
	if ((port == 0 || port == 1) && (device == RETRO_DEVICE_JOYPAD)) {
		unsigned logical_port = (g_swap_controllers && (port <= 1)) ? (port ^ 1) : port;
		return retro_input_state_cb(logical_port, device, index, id);
	} else
		return 0;
}

static void frameskip_cb(BOOL flag)
{
	audio_buff_status_cb(flag == 1 /*active*/, 0 /*occupancy*/, true /*underrun_likely*/);
}

static void dummy_retro_run(void)
{
	// Prepare the loader and load the menu
	strcpy(ptr_gs_run_game_file, "menu;p.gba");
	strcpy(ptr_gs_run_game_name, "FrogUI");
	run_game("/mnt/sda1/ROMS/menu;p.gba", 0);
}

void darken_rgb565_buffer(const void* buffer, unsigned width, unsigned height, uint8_t darken_percentage)
{
    const uint16_t* src = (const uint16_t*)buffer;
    uint16_t* dst = rgb565_darken_buffer;
    unsigned pixel_count = width * height;

    // Convert darken_percentage (0-100) to darken_factor_256 (0-255)
	uint8_t darken_factor_256 = ((100 - darken_percentage) * 255) / 100;

    for (unsigned i = 0; i < pixel_count; i++)
    {
        uint16_t pixel = src[i];

        // Extract components
        uint8_t r5 = (pixel >> 11) & 0x1F;
        uint8_t g6 = (pixel >> 5) & 0x3F;
        uint8_t b5 = pixel & 0x1F;

        // Darken components with integer math
        r5 = (r5 * darken_factor_256) >> 8;
        g6 = (g6 * darken_factor_256) >> 8;
        b5 = (b5 * darken_factor_256) >> 8;

        // Repack components
        dst[i] = (r5 << 11) | (g6 << 5) | b5;
    }
}

void wrap_video_refresh_cb(const void *data, unsigned width, unsigned height, size_t pitch)
{
	if (g_show_fps) {
		*fw_fps_counter_enable = 1;
		g_fps_counter = true; 
		static uint32_t prev_msec = 0;
		static int count_all = 0;
		static int count_not_skipped = 0;

		uint32_t curr_msec = os_get_tick_count();

		++count_all;
		if (data)
			++count_not_skipped;

		if (curr_msec - prev_msec > 1000)
		{
			// im not sure that using floats math will calc the fps much more accurately
			// float sec = ((curr_msec - prev_msec) / 1000.0f);
			// *fw_fps_counter1 = count_not_skipped / sec;
			// fps_counter2 = count_all / sec;

			sprintf(fw_fps_counter_format, "%2d/%2d", count_not_skipped, count_all);

			prev_msec = curr_msec;
			count_all = 0;
			count_not_skipped = 0;
		}
	}

    if (capture_screenshot && data) {
        unsigned char* framebuffer = (unsigned char*)data;
        unsigned char* img_data = (unsigned char*)malloc(width * height * 3);

        if (g_xrgb888) {
            // If it's XRGB8888 format (32-bit per pixel), we need to extract R, G, B components
            for (unsigned i = 0; i < width * height; i++) {
                uint32_t color = ((uint32_t*)data)[i];

                // Extract RGB components
                uint8_t r = (color >> 16) & 0xFF;
                uint8_t g = (color >> 8) & 0xFF;
                uint8_t b = color & 0xFF;

                // Store in img_data (24-bit color, RGB order)
                img_data[i * 3 + 0] = b;
                img_data[i * 3 + 1] = g;
                img_data[i * 3 + 2] = r;
            }
        } else {
            // If it's RGB565 format, convert to RGB888 (24-bit)
            for (unsigned i = 0; i < width * height; i++) {
                uint16_t color = ((uint16_t*)data)[i];

                // Extract RGB565 components
                uint8_t r = (color >> 11) & 0x1F;
                uint8_t g = (color >> 5) & 0x3F;
                uint8_t b = color & 0x1F;

                // Convert to 8-bit RGB (scale 5-bit to 8-bit)
                r = (r << 3) | (r >> 2);
                g = (g << 2) | (g >> 4);
                b = (b << 3) | (b >> 2);

                // Store in img_data (24-bit color, RGB order)
                img_data[i * 3 + 0] = b;  
                img_data[i * 3 + 1] = g;  
                img_data[i * 3 + 2] = r; 
            }
        }

        // BMP Header setup
        BMPHeader bmp_header = {0};
        BMPInfoHeader bmp_info_header = {0};

        bmp_header.bfType = 0x4D42;  // 'BM' in little-endian
        bmp_header.bfOffBits = sizeof(BMPHeader) + sizeof(BMPInfoHeader);
        bmp_header.bfSize = bmp_header.bfOffBits + width * height * 3;

        bmp_info_header.biSize = sizeof(BMPInfoHeader);
        bmp_info_header.biWidth = width;
        bmp_info_header.biHeight = -height;  // Negative height to indicate top-down BMP
        bmp_info_header.biPlanes = 1;
        bmp_info_header.biBitCount = 24;  // 24 bits per pixel (RGB)
        bmp_info_header.biSizeImage = width * height * 3;

        char filename[MAXPATH];
		char basename[MAXPATH];
		fill_pathname_base(basename, s_game_filepath, sizeof(basename));
		path_remove_extension(basename);
        sprintf(filename, "/mnt/sda1/%s_screenshot_%d.bmp", basename, screenshot_counter);

        FILE *bmp_file = fopen(filename, "wb");
        if (bmp_file) {
            // Write BMP header and info header
            fwrite(&bmp_header, sizeof(BMPHeader), 1, bmp_file);
            fwrite(&bmp_info_header, sizeof(BMPInfoHeader), 1, bmp_file);

            fwrite(img_data, 1, width * height * 3, bmp_file);

            fclose(bmp_file);
            xlog("Screenshot saved to %s\n", filename);
			g_osd_small_messages ? sprintf(osd_message, "Saved") : sprintf(osd_message, "Screenshot Saved");
			show_osd_message(osd_message);
        } else xlog("Failed to save screenshot\n");

        free(img_data);
        screenshot_counter++;
        capture_screenshot = false;
    }

	if (!rgb565_darken_buffer || width != buffer_prev_width || height != buffer_prev_height) { // AV Out changes resolution?
    	if (rgb565_darken_buffer) free(rgb565_darken_buffer);

    	rgb565_darken_buffer = malloc(width * height * 2);
    	buffer_prev_width = width;
    	buffer_prev_height = height;
	}

	if (g_xrgb888) { //TODO: add darkening filter support
		convert_xrgb8888_to_rgb565((void*)data, width, height, pitch);
		retro_video_refresh_cb(s_rgb565_convert_buffer, width, height, width * 2);		// each pixel is now 16bit, so pass the pitch as width*2
	} else {
		if (data && g_enable_darken_filter) {
			darken_rgb565_buffer(data, width, height, g_darken_percentage);
			retro_video_refresh_cb(rgb565_darken_buffer, width, height, pitch);
		} else { // Handle null data or no filter
			retro_video_refresh_cb(data, width, height, pitch);
		}
	}
}
