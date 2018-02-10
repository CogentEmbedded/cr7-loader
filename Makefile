###################################################
# makefile
###################################################

define add_define
DEFINES			+=	-D$(1)$(if $(value $(1)),=$(value $(1)),)
endef

INCLUDE_DIR	=	-Iinclude

#output file name
FILE_NAME	=	cr7_loader
FILE_NAME_SA0   =	bootparam_sa0
FILE_NAME_SA3   =	cert_header_sa3

OUTPUT_FILE	=	$(FILE_NAME).elf
OUTPUT_FILE_SA0 =	$(FILE_NAME_SA0).elf
OUTPUT_FILE_SA3 =	$(FILE_NAME_SA3).elf

#object file name
OBJ_FILE	=	common/mem.o			\
			common/scif.o			\
			common/div.o			\
			common/llsl.o			\
			common/llsr.o			\
			common/printf.o			\
			common/dma_driver.o		\
			common/rpc_driver.o		\
			common/rcar_pwrc.o		\
			loader/loader.o			\
			loader/loader_main.o		\
			loader/rcar_bl2_addr.o		\
			loader/rcar_version.o		\
			loader/bl2_cpg_init.o		\
			loader/ipmmu.o \
			common/micro_wait.o

OBJ_FILE_SA0	=	tools/dummy_create/sa0.o
OBJ_FILE_SA3	=	tools/dummy_create/sa3.o


#linker script name
MEMORY_DEF	=	loader/loader.ld.S
MEMORY_DEF_SA0	=	tools/dummy_create/sa0.ld.S
MEMORY_DEF_SA3	=	tools/dummy_create/sa3.ld.S

###################################################

# Debug build
DEBUG:=0

# LSI setting common define
RCAR_H3:=0
RCAR_M3:=1
RCAR_V3M:=2
RCAR_V3H:=3
$(eval $(call add_define,RCAR_V3H))
RCAR_CUT_10:=0
RCAR_CUT_11:=1
$(eval $(call add_define,RCAR_CUT_10))
$(eval $(call add_define,RCAR_CUT_11))

ifndef LSI
  $(error "Error: Unknown LSI. Please use LSI=<LSI name> to specify the LSI")
else
  ifeq (${LSI},V3H)
    RCAR_LSI:=${RCAR_V3H}
  else
    $(error "Error: ${LSI} is not supported.")
  endif
  $(eval $(call add_define,RCAR_LSI))
endif

ifneq (${DEBUG}, 0)
  # Use LOG_LEVEL_INFO by default for debug builds
  LOG_LEVEL:=40
else
  # Use LOG_LEVEL_NOTICE by default for release builds
  LOG_LEVEL:=20
endif

# Process DEBUG flag
$(eval $(call assert_boolean,DEBUG))
$(eval $(call add_define,DEBUG))
ifeq (${DEBUG},0)
  $(eval $(call add_define,NDEBUG))
else
CFLAGS			+= 	-g
ASFLAGS			+= 	-g -Wa,--gdwarf-2
endif

# Process LOG_LEVEL flag
ifndef LOG_LEVEL
LOG_LEVEL	:=	20
endif
$(eval $(call add_define,LOG_LEVEL))

# Process RCAR_SECURE_BOOT flag
ifndef RCAR_SECURE_BOOT
RCAR_SECURE_BOOT := 1
endif
$(eval $(call add_define,RCAR_SECURE_BOOT))

# Process RCAR_QOS_TYPE flag
ifndef RCAR_QOS_TYPE
RCAR_QOS_TYPE := 3
endif
$(eval $(call add_define,RCAR_QOS_TYPE))

# Process RCAR_DRAM_SPLIT flag
ifndef RCAR_DRAM_SPLIT
RCAR_DRAM_SPLIT := 0
endif
$(eval $(call add_define,RCAR_DRAM_SPLIT))

# Process RCAR_KICK_MAIN_CPU flag
ifndef RCAR_KICK_MAIN_CPU
RCAR_KICK_MAIN_CPU := 0
endif
$(eval $(call add_define,RCAR_KICK_MAIN_CPU))

# Process RCAR_SYSRAM_ECC_ENABLE flag
ifndef RCAR_SYSRAM_ECC_ENABLE
RCAR_SYSRAM_ECC_ENABLE := 0
endif
$(eval $(call add_define,RCAR_SYSRAM_ECC_ENABLE))

# Process RCAR_SYSRAM_IPMMU_ENABLE flag
ifndef RCAR_SYSRAM_IPMMU_ENABLE
RCAR_SYSRAM_IPMMU_ENABLE := 0
endif
$(eval $(call add_define,RCAR_SYSRAM_IPMMU_ENABLE))

include pfc/pfc.mk
include ddr/ddr.mk
include qos/qos.mk

###################################################

CC			= $(CROSS_COMPILE)gcc
CPP			= ${CROSS_COMPILE}cpp
AS			= ${CROSS_COMPILE}gcc
AR			= ${CROSS_COMPILE}ar
LD			= $(CROSS_COMPILE)ld
OC			= ${CROSS_COMPILE}objcopy
OD			= ${CROSS_COMPILE}objdump

ASFLAGS		+=	-marm -march=armv7-r				\
			-nostdinc -ffreestanding -Wa,--fatal-warnings	\
			-Werror -Wmissing-include-dirs			\
			 -c -D__ASSEMBLY				\
			$(INCLUDE_DIR) $(DEFINES)

CFLAGS		+=	-marm -march=armv7-r				\
			-nostdinc -ffreestanding -Wall			\
			-Werror -Wmissing-include-dirs			\
			-std=c99 -c -Os					\
			-ffunction-sections -fdata-sections		\
			$(INCLUDE_DIR) $(DEFINES)

LDFLAGS		=	--fatal-warnings -O1 --gc-sections

###################################################
.SUFFIXES : .s .c .o

###################################################
# command

.PHONY: all
all: $(OUTPUT_FILE) $(OUTPUT_FILE_SA0) $(OUTPUT_FILE_SA3)

###################################################
# Linker
###################################################
$(OUTPUT_FILE) : $(MEMORY_DEF) $(OBJ_FILE)
	$(LD) $(OBJ_FILE)			\
	-T $(MEMORY_DEF)			\
	-o $(OUTPUT_FILE)			\
	$(LDFLAGS)				\
	-Map $(FILE_NAME).map

	$(OC) -O srec --srec-forceS3  $(OUTPUT_FILE) $(FILE_NAME).srec
	$(OC) -O binary $(OUTPUT_FILE) $(FILE_NAME).bin
	$(OD) -dx $(OUTPUT_FILE) > $(FILE_NAME).dump

$(OUTPUT_FILE_SA0) : $(MEMORY_DEF_SA0) $(OBJ_FILE_SA0)
	$(LD) $(OBJ_FILE_SA0)		 	\
	-T $(MEMORY_DEF_SA0)			\
	-o $(OUTPUT_FILE_SA0)			\
	-Map $(FILE_NAME_SA0).map 		\

	$(OC) -O srec --adjust-vma=0xEB220000 --srec-forceS3  $(OUTPUT_FILE_SA0) $(FILE_NAME_SA0).srec
	$(OC) -O binary --adjust-vma=0xEB220000 --srec-forceS3  $(OUTPUT_FILE_SA0) $(FILE_NAME_SA0).bin

$(OUTPUT_FILE_SA3) : $(MEMORY_DEF_SA3) $(OBJ_FILE_SA3)
	$(LD) $(OBJ_FILE_SA3)		 	\
	-T $(MEMORY_DEF_SA3)			\
	-o $(OUTPUT_FILE_SA3)			\
	-Map $(FILE_NAME_SA3).map 		\

	$(OC) -O srec --adjust-vma=0xEB220000 --srec-forceS3  $(OUTPUT_FILE_SA3) $(FILE_NAME_SA3).srec
	$(OC) -O binary --adjust-vma=0xEB220000 --srec-forceS3  $(OUTPUT_FILE_SA3) $(FILE_NAME_SA3).bin

###################################################
# Compile
###################################################

%.o:../%.c
	$(CC) $(CFLAGS) -o $@ $<

%.o:../%.s
	$(AS) $(ASFLAGS) -o $@ $<

.PHONY: clean
clean:
	$(RM) $(OBJ_FILE) $(OUTPUT_FILE) $(OBJ_FILE_SA0) $(OBJ_FILE_SA3) $(OUTPUT_FILE_SA0) $(OUTPUT_FILE_SA3) \
	$(FILE_NAME).* $(FILE_NAME_SA0).* $(FILE_NAME_SA3).*
