
#include <fcntl.h>
#include <stdlib.h>
#include <gio/gio.h>
#include <gudev/gudev.h>

#ifdef HAS_LIBDRM_AMDGPU
#include <amdgpu.h>
#include <amdgpu_drm.h>

G_DEFINE_AUTOPTR_CLEANUP_FUNC(amdgpu_device_handle, free);
#endif

typedef int handle;
G_DEFINE_AUTO_CLEANUP_FREE_FUNC(handle, close, -1)

static gboolean
get_card_is_discrete_amdgpu (GUdevDevice *d)
{
#ifdef HAS_LIBDRM_AMDGPU
	const char *devname;
	g_auto(handle) fd = -1;
	g_autoptr(GUdevDevice) parent = NULL;
	struct drm_amdgpu_info_device device_info = {0};
	amdgpu_device_handle device = NULL;
	uint32_t drm_major, drm_minor;

	parent = g_udev_device_get_parent (d);
	if (g_strcmp0 (g_udev_device_get_driver (parent), "amdgpu") != 0)
		return FALSE;

	devname = g_udev_device_get_property (d, "DEVNAME");
	if (!devname)
		return FALSE;

	fd = open (devname, O_RDWR);
	if (fd < 0)
		return FALSE;

	if (amdgpu_device_initialize (fd, &drm_major, &drm_minor, &device))
		return FALSE;

	if (amdgpu_query_info (device, AMDGPU_INFO_DEV_INFO, sizeof(device_info), &device_info))
		return FALSE;

	/* Check if the device is an APU */
	return !(device_info.ids_flags & AMDGPU_IDS_FLAGS_FUSION);
#endif
	return FALSE;
}

gboolean
get_card_is_discrete (GUdevDevice *d)
{
	return get_card_is_discrete_amdgpu (d);
}
