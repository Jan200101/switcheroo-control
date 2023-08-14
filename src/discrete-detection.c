
#include <fcntl.h>
#include <stdlib.h>
#include <gio/gio.h>
#include <gudev/gudev.h>

#ifdef HAS_LIBDRM_NOUVEAU
#include <xf86drm.h>
#include <nouveau_drm.h>
#include <nouveau/nvif/ioctl.h>
#include <nvif/cl0080.h>
#include <nvif/class.h>
#endif

#ifdef HAS_LIBDRM_AMDGPU
#include <amdgpu.h>
#include <amdgpu_drm.h>

G_DEFINE_AUTOPTR_CLEANUP_FUNC(amdgpu_device_handle, free);
#endif

typedef int handle;
G_DEFINE_AUTO_CLEANUP_FREE_FUNC(handle, close, -1)

static gboolean
get_card_is_discrete_nvidia (GUdevDevice *d)
{
	g_autoptr(GUdevDevice) parent = NULL;
	const char* driver;

	parent = g_udev_device_get_parent (d);
	driver = g_udev_device_get_driver (parent);

	if (g_strcmp0 (driver, "nvidia") == 0)
		return TRUE;

	if (g_strcmp0 (driver, "nouveau") != 0)
		return FALSE;

#ifdef HAS_LIBDRM_NOUVEAU
	const char *devname;
	g_auto(handle) fd = -1;

	devname = g_udev_device_get_property (d, "DEVNAME");
	if (!devname)
		return FALSE;

	fd = open (devname, O_RDWR);
	if (fd < 0)
		return FALSE;

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
			return FALSE;
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
		return FALSE;


	switch (args.info.platform)
	{
		case NV_DEVICE_INFO_V0_IGP:
		case NV_DEVICE_INFO_V0_SOC:
			return FALSE;

		case NV_DEVICE_INFO_V0_PCI:
		case NV_DEVICE_INFO_V0_AGP:
		case NV_DEVICE_INFO_V0_PCIE:
		default:
			return TRUE;
	}
#endif
	return FALSE;
}

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
	return get_card_is_discrete_nvidia(d) \
		|| get_card_is_discrete_amdgpu (d);
}
