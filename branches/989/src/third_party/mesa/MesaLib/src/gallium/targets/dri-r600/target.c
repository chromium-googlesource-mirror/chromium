
#include "state_tracker/drm_driver.h"
#include "target-helpers/inline_debug_helper.h"
#include "r600/drm/r600_drm_public.h"
#include "r600/r600_public.h"

static struct pipe_screen *
create_screen(int fd)
{
   struct radeon *rw;
   struct pipe_screen *screen;

   rw = r600_drm_winsys_create(fd);
   if (!rw)
      return NULL;

   screen = r600_screen_create(rw);
   if (!screen)
      return NULL;

   screen = debug_screen_wrap(screen);

   return screen;
}

DRM_DRIVER_DESCRIPTOR("r600", "radeon", create_screen)
