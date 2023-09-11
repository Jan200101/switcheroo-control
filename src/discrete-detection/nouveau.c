
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <gio/gio.h>
#include <gudev/gudev.h>

#include <xf86drm.h>
#include <nouveau_drm.h>
#include <nouveau/nvif/ioctl.h>
#include <nvif/cl0080.h>
#include <nvif/class.h>

typedef int handle;
G_DEFINE_AUTO_CLEANUP_FREE_FUNC(handle, close, -1)

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		puts ("check-discrete-nouveau [DEVNAME]");
		return EXIT_FAILURE;
	}

	const char *devname;
	g_auto(handle) fd = -1;

	devname = argv[1];
	fd = open (devname, O_RDWR);
	if (fd < 0)
		return EXIT_FAILURE;

	g_autofree void *device = malloc(352);

	/* Init device */
	{	
		struct {
			struct nvif_ioctl_v0 ioctl;
			struct nvif_ioctl_new_v0 new;
			struct nv_device_v0 dev;
		} init_args = {
		  .ioctl = {
			 .object = 0,
			 .owner = NVIF_IOCTL_V0_OWNER_ANY,
			 .route = 0x00,
			 .type = NVIF_IOCTL_V0_NEW,
			 .version = 0,
		  },
		  .new = {
			 .handle = 0,
			 .object = (uintptr_t)device,
			 .oclass = NV_DEVICE,
			 .route = NVIF_IOCTL_V0_ROUTE_NVIF,
			 .token = (uintptr_t)device,
			 .version = 0,
		  },
		  .dev = {
			 .device = ~0ULL,
		  },
	   };

		if (drmCommandWrite (fd, DRM_NOUVEAU_NVIF, &init_args, sizeof(init_args)))
			return EXIT_FAILURE;
	}

	/* Query device info */
	struct {
		struct nvif_ioctl_v0 ioctl;
		struct nvif_ioctl_mthd_v0 mthd;
		struct nv_device_info_v0 info;
	} args = {
		.ioctl = {
			.object = (uintptr_t)device,
			.owner = NVIF_IOCTL_V0_OWNER_ANY,
			.route = 0x00,
			.type = NVIF_IOCTL_V0_MTHD,
			.version = 0,
		},
		.mthd = {
			.method = NV_DEVICE_V0_INFO,
			.version = 0,
		},
		.info = {
			.version = 0,
		},
	};

	if (drmCommandWriteRead (fd, DRM_NOUVEAU_NVIF, &args, sizeof(args)))
		return EXIT_FAILURE;


	switch (args.info.platform)
	{
		case NV_DEVICE_INFO_V0_IGP:
		case NV_DEVICE_INFO_V0_SOC:
			return EXIT_FAILURE;

		case NV_DEVICE_INFO_V0_PCI:
		case NV_DEVICE_INFO_V0_AGP:
		case NV_DEVICE_INFO_V0_PCIE:
		default:
			return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}
