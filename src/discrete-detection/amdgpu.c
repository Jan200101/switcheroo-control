#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <gio/gio.h>
#include <gudev/gudev.h>
#include <glib.h>

#include <amdgpu.h>
#include <amdgpu_drm.h>

typedef int handle;
G_DEFINE_AUTO_CLEANUP_FREE_FUNC(handle, close, -1)
G_DEFINE_AUTOPTR_CLEANUP_FUNC(amdgpu_device_handle, free);

int main (int argc, char** argv)
{
	const char *devname;
	g_auto(handle) fd = -1;
	struct drm_amdgpu_info_device device_info = {0};
	amdgpu_device_handle device = NULL;
	uint32_t drm_major, drm_minor;
	g_autoptr(GOptionContext) option_context = NULL;
	g_autoptr(GError) error = NULL;

	setlocale (LC_ALL, "");
	option_context = g_option_context_new ("");

	if (!g_option_context_parse (option_context, &argc, &argv, &error)) {
		g_print ("Failed to parse arguments: %s\n", error->message);
		return EXIT_FAILURE;
	}

	if (argc < 2)
	{
		g_print ("%s\n", g_option_context_get_help (option_context, TRUE, NULL));
		return EXIT_FAILURE;
	}
	devname = argv[1];
;

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
