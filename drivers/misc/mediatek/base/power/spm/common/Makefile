ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/spm/$(MTK_PLATFORM)/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/include/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/$(MTK_PLATFORM)/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/spm/common/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/spm/$(MTK_PLATFORM)/
ifeq ($(CONFIG_MTK_UFS_SUPPORT), y)
ccflags-y += -I$(srctree)/drivers/scsi/ufs/
endif

# Include cpu dvfs header cpufreq_api.h
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/mach/
# Include mtk_ram_console.h for aee_xxx api
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
# Include uart header mtk_uart_api.h
ccflags-y += -I$(srctree)/drivers/misc/mediatek/uart/include/
# Include teei header teei_client_main.h
ifeq ($(CONFIG_MICROTRUST_TEE_SUPPORT), y)
include $(srctree)/drivers/misc/mediatek/teei/Makefile.include
endif
# Include pwm header ddp_pwm.h
ccflags-y += -I$(srctree)/drivers/misc/mediatek/video/include/
# Include mcdi header mtk_mcdi_api.h
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/mcdi/include/
# Include clkbuf header mtk_clkbuf_ctl.h
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/include/clkbuf_v1/
# Include soc header mtk-soc-afe-control.h
ifeq ($(CONFIG_MTK_SND_SOC_NEW_ARCH), y)
ifeq ($(CONFIG_MACH_MT6761),y)
ccflags-y += -I$(srctree)/sound/soc/mediatek/mt6765/
else
ccflags-y += -I$(srctree)/sound/soc/mediatek/$(MTK_PLATFORM)/
endif
ccflags-y += -I$(srctree)/sound/soc/mediatek/common_int/
endif
# Include thermal header mtk_thermal.h
ifeq ($(CONFIG_THERMAL), y)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/thermal/$(MTK_PLATFORM)/inc/mach/
endif

obj-y += mtk_idle.o
obj-y += mtk_idle_select.o
obj-y += mtk_idle_internal.o
obj-y += mtk_idle_profile.o
obj-y += mtk_idle_twam.o
obj-y += mtk_dpidle.o
obj-y += mtk_sodi.o
obj-y += mtk_spm_resource_req.o
obj-y += mtk_sleep.o
obj-y += mtk_sleep_init.o
obj-y += mtk_vcore_dvfs.o
obj-y += mtk_idle_sysfs.o
obj-y += mtk_idle_sysfs_debugfs.o
