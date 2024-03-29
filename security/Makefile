#
# Makefile for the kernel security code
#

subdir-ccflags-y += -Werror

obj-$(CONFIG_KEYS)			+= keys/
subdir-$(CONFIG_SECURITY_SELINUX)	+= selinux
subdir-$(CONFIG_SECURITY_SMACK)		+= smack
subdir-$(CONFIG_SECURITY_TOMOYO)        += tomoyo
subdir-$(CONFIG_SECURITY_APPARMOR)	+= apparmor
subdir-$(CONFIG_SECURITY_YAMA)		+= yama
subdir-$(CONFIG_SECURITY_LOADPIN)	+= loadpin

# always enable default capabilities
obj-y					+= commoncap.o
obj-$(CONFIG_MMU)			+= min_addr.o

# Object file lists
obj-$(CONFIG_SECURITY)			+= security.o
obj-$(CONFIG_SECURITYFS)		+= inode.o
obj-$(CONFIG_SECURITY_SELINUX)		+= selinux/
obj-$(CONFIG_SECURITY_SMACK)		+= smack/
obj-$(CONFIG_AUDIT)			+= lsm_audit.o
obj-$(CONFIG_SECURITY_TOMOYO)		+= tomoyo/
obj-$(CONFIG_SECURITY_APPARMOR)		+= apparmor/
obj-$(CONFIG_SECURITY_YAMA)		+= yama/
obj-$(CONFIG_SECURITY_LOADPIN)		+= loadpin/
obj-$(CONFIG_CGROUP_DEVICE)		+= device_cgroup.o

# Object integrity file lists
subdir-$(CONFIG_INTEGRITY)		+= integrity
obj-$(CONFIG_INTEGRITY)			+= integrity/

# HW Object
subdir-$(CONFIG_HUAWEI_SELINUX_DSM)	+= hwselinux
obj-$(CONFIG_HUAWEI_SELINUX_DSM)	+= hwselinux/

obj-$(CONFIG_HIE)			+= hie/
obj-$(CONFIG_HUAWEI_PROC_CHECK_ROOT)    += check_root/
obj-$(CONFIG_HW_ROOT_SCAN)		+= hw_root_scan/
obj-$(CONFIG_HW_KERNEL_STP)		+= kernel_stp/
obj-$(CONFIG_HUAWEI_KASLR_DEBUGFS) += kernel_harden/hw_kaslr_debugfs.o
obj-$(CONFIG_HUAWEI_FREELIST_RANDOM_DEBUGFS) += kernel_harden/hw_freelist_random_debugfs.o
