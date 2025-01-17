#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "libretro.h"
#include "machine.h"


#define MC_WIDTH (64*MACHINE_SCALE)
#define MC_HEIGHT (64*MACHINE_SCALE)

static uint32_t *frame_buf=NULL;
static struct retro_log_callback logging;
static retro_log_printf_t log_cb;

void mfb_setpix(int x,int y,uint32_t rgba)
{
	if (frame_buf!=NULL)
		frame_buf[x+(y*MC_WIDTH)]=rgba;
}

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
   (void)level;
   va_list va;
   va_start(va, fmt);
   vfprintf(stderr, fmt, va);
   va_end(va);
}

void retro_init(void)
{
	log_cb(RETRO_LOG_INFO,"retro_init\n");
	frame_buf = calloc(MC_WIDTH * MC_HEIGHT, sizeof(uint32_t));
}

void retro_deinit(void)
{
   free(frame_buf);
   frame_buf = NULL;
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);
}

void retro_get_system_info(struct retro_system_info *info)
{
	memset(info, 0, sizeof(*info));
	info->library_name     = "MiniCube64";
	info->library_version  = "v1.1";
	info->need_fullpath    = true;
	info->block_extract    = false;
	info->valid_extensions = "asm|s|bin|rom";
}

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   float aspect = 4.0f / 4.0f;

   info->timing = (struct retro_system_timing) {
      .fps = 60.0,
      .sample_rate = 0.0,
   };

   info->geometry = (struct retro_game_geometry) {
      .base_width   = MC_WIDTH,
      .base_height  = MC_HEIGHT,
      .max_width    = MC_WIDTH,
      .max_height   = MC_HEIGHT,
      .aspect_ratio = aspect,
   };

}

void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;

   bool no_content = true;
   cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_content);

   if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
      log_cb = logging.log;
   else
      log_cb = fallback_log;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
   audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}


void retro_reset(void)
{
	log_cb(RETRO_LOG_INFO,"retro_reset\n");
	reset_machine(NULL);
}

static void update_input(void)
{
   input_poll_cb();

   if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP))
   {
      /* Stub */
   }
}

static void render_checkered(void)
{
   /* Try rendering straight into VRAM if we can. */
   uint32_t *buf = NULL;
   unsigned stride = 0;
   struct retro_framebuffer fb = {0};
   fb.width = MC_WIDTH;
   fb.height = MC_HEIGHT;
   fb.access_flags = RETRO_MEMORY_ACCESS_WRITE;
   if (environ_cb(RETRO_ENVIRONMENT_GET_CURRENT_SOFTWARE_FRAMEBUFFER, &fb) && fb.format == RETRO_PIXEL_FORMAT_XRGB8888)
   {
      buf = fb.data;
      stride = fb.pitch >> 2;
   }
   else
   {
      buf = frame_buf;
      stride = MC_WIDTH;
   }

	display_machine();
	video_cb(buf, MC_WIDTH, MC_HEIGHT, stride << 2);
}

static void check_variables(void)
{
	log_cb(RETRO_LOG_INFO,"check_variables\n");
}

static void audio_callback(void)
{
   audio_cb(0, 0);
}

void retro_run(void)
{
   update_input();
   render_checkered();
   audio_callback();

   bool updated = false;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      check_variables();
}


bool retro_load_game(const struct retro_game_info *info)
{
	enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
	log_cb(RETRO_LOG_INFO,"retro_load_game\n");

	if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
	{
		log_cb(RETRO_LOG_INFO, "XRGB8888 is not supported.\n");
		return false;
	}

	check_variables();
	if (info!=NULL)
	{
		log_cb(RETRO_LOG_INFO,"load game %s\n",info->path);
		if (info->path!=NULL)
			reset_machine(info->path);
	}
	else 
	{
		reset_machine(NULL);
		log_cb(RETRO_LOG_INFO,"no path retro_load_game\n");
	}
   return true;
}

void retro_unload_game(void)
{
}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
	log_cb(RETRO_LOG_INFO,"no path retro_load_game_special\n");
	return false;
}

size_t retro_serialize_size(void)
{
   return 2;
}

bool retro_serialize(void *data_, size_t size)
{
   if (size < 2)
      return false;

   uint8_t *data = data_;
	 (void)data;
   return true;
}

bool retro_unserialize(const void *data_, size_t size)
{
   if (size < 2)
      return false;

   const uint8_t *data = data_;
	 (void)data;
   return true;
}

void *retro_get_memory_data(unsigned id)
{
   (void)id;
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   (void)id;
   return 0;
}

void retro_cheat_reset(void)
{}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
   (void)index;
   (void)enabled;
   (void)code;
}

