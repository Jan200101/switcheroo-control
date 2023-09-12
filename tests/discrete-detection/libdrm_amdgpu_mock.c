#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include <amdgpu_drm.h>

enum {
	OTHER_GPU,
	AMD_APU,
	AMD_GPU,
};

/* Mock open(2) so we can test multiple devices configurations */
int open(const char *pathname, int flags)
{
	if (!strcmp(pathname, "OTHER_GPU"))
		return OTHER_GPU;
	if (!strcmp (pathname, "AMD_APU"))
		return AMD_APU;
	if (!strcmp (pathname, "AMD_GPU"))
		return AMD_GPU;

	return -1;
}

/* open64 may be used for large file support */
int open64(const char *pathname, int flags)
{
	return open (pathname, flags);
}

int amdgpu_device_initialize(int fd, uint32_t *major_version, uint32_t *minor_version, int *device_handle)
{
	// Store the fd in the device handle for access in query_info
	*device_handle = fd;

	if (fd != AMD_GPU && fd != AMD_APU)
		return 1;

	return 0;
}

int amdgpu_query_info(int device_handle, unsigned info_id, unsigned size, void *value)
{
	struct drm_amdgpu_info_device* device_info = value;

	if (device_handle == AMD_GPU) {
		device_info->ids_flags = 0;
		return 0;
	}
	if (device_handle == AMD_APU) {
		device_info->ids_flags = AMDGPU_IDS_FLAGS_FUSION;
		return 0;
	}

	return 1;
}
