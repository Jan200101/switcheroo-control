#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include <xf86drm.h>
#include <nouveau_drm.h>
#include <nouveau/nvif/ioctl.h>
#include <nvif/cl0080.h>
#include <nvif/class.h>

enum {
	OTHER_GPU,
	NVIDIA_IGPU,
	NVIDIA_GPU,
};

/* Mock open(2) so we can test multiple devices configurations */
int open(const char *pathname, int flags)
{
	if (!strcmp(pathname, "OTHER_GPU"))
		return OTHER_GPU;
	if (!strcmp (pathname, "NVIDIA_IGPU"))
		return NVIDIA_IGPU;
	if (!strcmp (pathname, "NVIDIA_GPU"))
		return NVIDIA_GPU;

	return -1;
}

/* open64 may be used for large file support */
int open64(const char *pathname, int flags)
{
	return open (pathname, flags);
}

int drmCommandWrite(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
	if (drmCommandIndex != DRM_NOUVEAU_NVIF)
		return 1;

	if (fd != NVIDIA_GPU && fd != NVIDIA_IGPU)
		return 1;

	return 0;
}

int drmCommandWriteRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
	if (drmCommandIndex != DRM_NOUVEAU_NVIF)
		return 1;

	struct {
		struct nvif_ioctl_v0 ioctl;
		struct nvif_ioctl_mthd_v0 mthd;
		struct nv_device_info_v0 info;
	} *args = data;

	if (fd == NVIDIA_GPU) {
		args->info.platform = NV_DEVICE_INFO_V0_PCIE;
		return 0;
	}
	if (fd == NVIDIA_IGPU) {
		args->info.platform = NV_DEVICE_INFO_V0_IGP;
		return 0;	
	}

	return 1;
}