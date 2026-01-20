#ifndef __CORE_H
#define __CORE_H
// Collection of hacky workarounds to communicate between core_api and a core

typedef struct {
    uint32_t signature;
    uint16_t version_needed;
    uint16_t general_flag;
    uint16_t compression_method;
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t filename_length;
	uint16_t extra_field_length;
    char *filename;
} zip_local_file_header;

// Unzip functions (maybe useful for cores)
void unzip_file(const char* path, void* buffer, bool is_wqw, uint32_t preview_size); // Unzip a file into a buffer
bool is_zip_wqw_file(const char* path, bool* is_wqw); // Check if a file is .zip or WQW (.zfc, .zsf, .zmd, .zgb, .zpc)
void dump_zip_header(const char *zip_path, bool is_wqw, uint32_t preview_size, zip_local_file_header *zip_header);
int load_rgb565_image(const char* filename, uint16_t* framebuffer, int width, int height);

// Wrap run_game so we can call it from frogui and retro_deinit
void wrap_run_game(const char *filename, int load_state);

// Custom functions manually added to tgb dual's and doublecherry's libretro.h
int retro_dual_emulated_count();
size_t retro_dual_get_sram_size(int slot);
void *retro_dual_get_sram_ptr(int slot);

#endif