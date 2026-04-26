#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include "debug.h"
#include "stockfw.h"
#include "dirent.h"
#include "video_sf2000.h"

#define MAX_LINE_LENGTH 256
#define MAX_FILE_NAME_LENGTH 256
const char *directory = "/mnt/sda1/bios/patches";

// Helper Functions for Memory Patcher Function
unsigned flip_bytes(unsigned value) {
    unsigned char *byte_value = (unsigned char *)&value;
    return (byte_value[0] << 24) | (byte_value[1] << 16) | (byte_value[2] << 8) | byte_value[3];
}

int custom_isspace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v');
}

int is_empty_line(const char *line) {
    while (*line) {
        if (!custom_isspace(*line)) return 0; // Not an empty line
        line++;
    }
    return 1; // Empty line
}

// Memory Patcher function
void patch_memory() {
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        xlog("PATCHER: Unable to open directory: %s\n", directory);
        return;
    }

    struct dirent *entry;
    char file_path[MAX_FILE_NAME_LENGTH];
    FILE *cfg_file;
    char line[MAX_LINE_LENGTH];

    // Iterate through each file in the directory
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".cfg") != NULL) {  // Process only .cfg files
            snprintf(file_path, sizeof(file_path), "%s/%s", directory, entry->d_name);
            cfg_file = fopen(file_path, "r");
            if (cfg_file == NULL) {
                xlog("PATCHER: Unable to open file: %s\n", file_path);
                continue;
            }

            int file_valid = 1;

            // Process each line in the .cfg file
            while (fgets(line, sizeof(line), cfg_file)) {
                if (is_empty_line(line)) continue;

                // Parse the line: Memory Location;Unpatched Memory;Patched Memory
                char memory_location[MAX_LINE_LENGTH], unpatched_memory[MAX_LINE_LENGTH], patched_memory[MAX_LINE_LENGTH];
                char *token, *context = NULL;
                int token_index = 0;

                token = strtok_r(line, ";", &context);
                while (token != NULL) {
                    if (token_index == 0) {
                        strncpy(memory_location, token, sizeof(memory_location) - 1);
                    } else if (token_index == 1) {
                        strncpy(unpatched_memory, token, sizeof(unpatched_memory) - 1);
                    } else if (token_index == 2) {
                        strncpy(patched_memory, token, sizeof(patched_memory) - 1);
                    }
                    token = strtok_r(NULL, ";", &context);
                    token_index++;
                }

                // Convert memory_location to address and check for validity
                uintptr_t address = strtoul(memory_location, NULL, 16);
                unsigned patched_value = (unsigned)strtol(patched_memory, NULL, 16);
                unsigned unpatched_value = (unsigned)strtol(unpatched_memory, NULL, 16);
                unpatched_value = flip_bytes(unpatched_value);
                unsigned current_value = *(unsigned *)((void *)address);

                // Compare the current memory value with the expected unpatched value
                if (unpatched_value != current_value) {
                    xlog("PATCHER: Memory mismatch at location %p: Expected 0x%08X, but found 0x%08X\n", 
                           address, unpatched_value, current_value);
                    file_valid = 0;
                    break;
                } else {
                    // Patch the memory with the new value
                    patched_value = flip_bytes(patched_value);
                    *(unsigned *)((void *)address) = patched_value;
                    __builtin___clear_cache((char *)((void *)address), (char *)((void *)address + sizeof(unsigned)));
                    xlog("PATCHER: Successfully patched memory at 0x%s with value 0x%s\n", memory_location, patched_memory);
                }
            }

            if (!file_valid) {
                xlog("PATCHER: Skipping file %s due to memory mismatch\n", entry->d_name);
            } else {
                xlog("PATCHER: Successfully patched file %s\n", entry->d_name);
            }

            fclose(cfg_file);
        }
    }
    closedir(dir);
}

extern int g_run_osd_color_mode;
extern int DAT_80c1076c;
extern void firmware_update(const char *path);
extern void rom_buf_init();
extern char *gp_small_buf;
extern char *gp_small_buf_copy;
void key_map_info_load();
void disable_sound_amp(int param);

void autoboot() {
    int fwupd = fs_access("/mnt/sda1/UpdateFirmware/Firmware.upk",0);
    if (fwupd == 0) firmware_update("/mnt/sda1/UpdateFirmware/Firmware.upk");
    rom_buf_init();
    gp_small_buf = (char *)malloc(0x40000);
    g_run_osd_height = 0;
    g_run_osd_width = 0;
    DAT_80c1076c = 10;
    gp_small_buf_copy = gp_small_buf;
    key_map_info_load();
    g_run_osd_color_mode = 5;
    disable_sound_amp(0);
    strcpy(ptr_gs_run_game_file, "menu;m.gba");
	strcpy(ptr_gs_run_game_name, "FrogUI");
    run_game("/mnt/sda1/ROMS/menu;m.gba", 0);
}

void __run_menu_shim__() {
    patch_memory();

    // TODO: Loading screen here or core_api and optional autoboot
    // Init a black framebuffer to black out the screen before loading a new game
    size_t framebuffer_size = 640 * 480 * sizeof(uint16_t);
    uint16_t* framebuffer = (uint16_t*)malloc(framebuffer_size);
    memset(framebuffer, 0x0000, framebuffer_size);
	uint16_t pixel_pitch = 640 * sizeof(uint16_t);

	// Write framebuffer to screen then free it
	run_screen_write(framebuffer, 640, 480, pixel_pitch);
	free(framebuffer);

    autoboot();
}