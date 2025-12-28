#include "da.h"
#include <time.h>
#include <stdint.h>

//================================================================
//Platform
//================================================================

#ifdef _WIN32
	#include <direct.h>
    #include <windows.h>
    #include <io.h>
	#define MUS_NL "\r\n"
	#define MUS_PS '\\'
	#define MUS_MKDIR_CMD "mkdir %s"
    #define MUS_CP_CMD "copy /Y %s %s"
    #define MUS_CP_R_CMD "xcopy /Y /E %s %s"
    #define MUS_MV_CMD "move /Y %s %s"
	#define MUS_RM_CMD "del /Q %s"
	#define MUS_RMDIR_CMD "rmdir /S /Q %s"
	#define MUS_FETCH_CMD "curl -L -o lib\\%s %s"
#else
    #include <unistd.h>
    #include <sys/stat.h>
	#define MUS_NL "\n"
	#define MUS_PS '/'
	#define MUS_MKDIR_CMD "mkdir -p %s"
    #define MUS_CP_CMD "cp %s %s"
    #define MUS_CP_R_CMD "cp -r %s %s"
    #define MUS_MV_CMD "mv %s %s"
	#define MUS_RM_CMD "rm %s"
	#define MUS_RMDIR_CMD "rm -rf %s"
	#define MUS_FETCH_CMD "wget -q --show-progress -O lib/%s %s"
#endif

//================================================================
//File Handling
//================================================================

static inline bool MUS_file_exists(char* file_path){FILE* file; if ((file = fopen(file_path, "r"))) {fclose(file); return true;} return false;}
static inline time_t MUS_get_mtime(char* file_path) {struct stat st; if (stat(file_path, &st) == -1) return 0; return st.st_mtime;}
static inline bool MUS_is_newer(char* new_path, char* old_path){
	time_t new_mtime = MUS_get_mtime(new_path);
	time_t old_mtime = MUS_get_mtime(old_path);
	if (new_mtime == 0 || old_mtime == 0) return true;
	return new_mtime > old_mtime;
}
static inline const char* MUS_path_basename(const char* path){
    #ifdef _WIN32
        const char* p1 = strrchr(path, '/');
        const char* p2 = strrchr(path, MUS_PS);
        const char* p = (p1 > p2)? p1 : p2;
        return p ? p + 1 : path;
    #else
        const char* p = strrchr(path, MUS_PS);
        return p ? p + 1 : path;
    #endif
}

static inline bool MUS_mkdir_if_not_exists(const char* path){return (!system(MUS_temp_sprintf(MUS_MKDIR_CMD, path)));}
static inline bool MUS_copy_file(const char* src_path, const char* dst_path){return (!system(MUS_temp_sprintf(MUS_CP_CMD, src_path, dst_path)));}
static inline bool MUS_copy_directory_recursively(const char* src_path, const char* dst_path) {return (!system(MUS_temp_sprintf(MUS_CP_R_CMD, src_path, dst_path)));}
static inline bool MUS_move_file(const char* src_path, const char* dst_path){return (!system(MUS_temp_sprintf(MUS_MV_CMD, src_path, dst_path)));}
static inline bool MUS_delete_file(const char* path) {return (!system(MUS_temp_sprintf(MUS_RM_CMD, path)));}
static inline bool MUS_delete_directory(const char* path) {return (!system(MUS_temp_sprintf(MUS_RMDIR_CMD, path)));}
static inline bool MUS_fetch_file(const char* path, const char* url){return (!system(MUS_temp_sprintf(MUS_FETCH_CMD, path, url)));}

MUS_da_define(uint8_t, MUS_DABytes);
static inline bool MUS_read_entire_file(const char* path, MUS_DABytes* da_bytes) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) return false;
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (file_size < 0) {fclose(file); return false;}
    MUS_da_reserve(da_bytes, da_bytes->count + file_size);
    size_t bytes_read = fread(da_bytes->items + da_bytes->count, 1, file_size, file);
    da_bytes->count += bytes_read;
    fclose(file);
    return bytes_read == (size_t)file_size;
}
static inline bool MUS_write_entire_file(const char* path, const void* data, size_t size) {
    FILE* file = fopen(path, "wb");
    if (file == NULL) return false;
    size_t bytes_written = fwrite(data, 1, size, file);
    fclose(file);
    return bytes_written == size;
}

#define MUS_mkdir_if_not_exists_l(path) do {if(MUS_mkdir_if_not_exists(path)) MUS_print_log(MUS_INFO, "CREATED DIRECTORY %s", #path); else MUS_print_log(MUS_ERROR, "COULDNT CREATE DIRECTORY %s", #path);} while(0)
#define MUS_copy_file_l(src_path, dst_path) do {if(MUS_copy_file(src_path, dst_path)) MUS_print_log(MUS_INFO, "COPIED FILE %s TO %s", #src_path, #dst_path); else MUS_print_log(MUS_ERROR, "COULDNT COPY FILE %s TO %s", #src_path, #dst_path);} while(0)
#define MUS_copy_directory_recursively_l(src_path, dst_path) do {if(MUS_copy_directory_recursively(src_path, dst_path)) MUS_print_log(MUS_INFO, "RECURSIVELY COPIED DIRECTORY %s TO %s", #src_path, #dst_path); else MUS_print_log(MUS_ERROR, "COULDNT RECURSIVELY COPY DIRECTORY %s TO %s", #src_path, #dst_path);} while(0)
#define MUS_move_file_l(src_path, dst_path) do {if(MUS_move_file(src_path, dst_path)) MUS_print_log(MUS_INFO, "MOVED FILE %s TO %s", #src_path, #dst_path); else MUS_print_log(MUS_ERROR, "COULDNT MOVE FILE %s TO %s", #src_path, #dst_path);} while(0)
#define MUS_delete_file_l(path) do {if(MUS_delete_file(path)) MUS_print_log(MUS_INFO, "DELETED FILE %s", #path); else MUS_print_log(MUS_ERROR, "COULDNT DELETE FILE %s", #path);} while(0)
#define MUS_delete_directory_l(path) do {if(MUS_delete_directory(path)) MUS_print_log(MUS_INFO, "DELETED DIRECTORY %s", #path); else MUS_print_log(MUS_ERROR, "COULDNT DELETE DIRECTORY %s", #path);} while(0)
#define MUS_fetch_file_l(path, url) do {if(MUS_fetch_file(path, url)) MUS_print_log(MUS_INFO, "FETCHED FILE %s TO %s", #path, #url); else MUS_print_log(MUS_ERROR, "COULDNT FETCH FILE %s TO %s", #path, #url);} while(0)
#define MUS_read_entire_file_l(path, dab) do {if(MUS_read_entire_file(path, dab)) MUS_print_log(MUS_INFO, "READ FILE %s", #path); else MUS_print_log(MUS_ERROR, "COULDNT READ FILE %s", #path);} while(0)
#define MUS_write_entire_file_l(path, data, size) do {if(MUS_write_entire_file(path, data, size)) MUS_print_log(MUS_INFO, "WROTE FILE %s", #path); else MUS_print_log(MUS_ERROR, "COULDNT WRITE FILE %s", #path);} while(0)

#ifndef MUS_FH_STRIP_PREFIX_GUARD_
#define MUS_FH_STRIP_PREFIX_GUARD_
    #ifndef MUS_DONT_STRIP_PREFIX
        #define file_exists MUS_file_exists
        #define get_mtime MUS_get_mtime
        #define is_newer MUS_is_newer
        #define path_basename MUS_path_basename
        #define mkdir_if_not_exists MUS_mkdir_if_not_exists
        #define copy_file MUS_copy_file
        #define copy_directory_recursively MUS_copy_directory_recursively
        #define move_file MUS_move_file
        #define delete_file MUS_delete_file
        #define delete_directory MUS_delete_directory
        #define fetch_file MUS_fetch_file
        #define DABytes MUS_DABytes
        #define read_entire_file MUS_read_entire_file
        #define write_entire_file MUS_write_entire_file
        #define mkdir_if_not_exists_l MUS_mkdir_if_not_exists_l
        #define copy_file_l MUS_copy_file_l
        #define copy_directory_recursively_l MUS_copy_directory_recursively_l
        #define move_file_l MUS_move_file_l
        #define delete_file_l MUS_delete_file_l
        #define delete_directory_l MUS_delete_directory_l
        #define fetch_file_l MUS_fetch_file_l
        #define read_entire_file_l MUS_read_entire_file_l
        #define write_entire_file_l MUS_write_entire_file_l
    #endif
#endif // MUS_FH_STRIP_PREFIX_GUARD_