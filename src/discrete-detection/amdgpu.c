
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <gio/gio.h>
#include <gudev/gudev.h>

#include <amdgpu.h>
#include <amdgpu_drm.h>

typedef int handle;
G_DEFINE_AUTO_CLEANUP_FREE_FUNC(handle, close, -1)
G_DEFINE_AUTOPTR_CLEANUP_FUNC(amdgpu_device_handle, free);

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		puts ("check-discrete-amdgpu [DEVNAME]");
		return EXIT_FAILURE;
	}

	const char *devname;
	g_auto(handle) fd = -1;
	g_autoptr(GUdevDevice) parent = NULL;
	struct drm_amdgpu_info_device device_info = {0};
	amdgpu_device_handle device = NULL;
	uint32_t drm_major, drm_minor;

	devname = argv[1];
	fd = open (devname, O_RDWR);
	if (fd < 0)
		return EXIT_FAILURE;

	if (amdgpu_device_initialize (fd, &drm_major, &drm_minor, &device))
		return EXIT_FAILURE;

	if (amdgpu_query_info (device, AMDGPU_INFO_DEV_INFO, sizeof(device_info), &device_info))
		return EXIT_FAILURE;

	/* AMDGPU_IDS_FLAGS_FUSION is set for all APUs */
	if (device_info.ids_flags & AMDGPU_IDS_FLAGS_FUSION)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
