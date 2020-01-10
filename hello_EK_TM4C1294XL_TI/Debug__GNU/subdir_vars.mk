################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CFG_SRCS += \
../main.cfg 

CMD_SRCS += \
../EK_TM4C1294XL.cmd 

LDS_SRCS += \
../tm4c1294ncpdt.lds 

C_SRCS += \
../EK_TM4C1294XL.c \
../com.c \
../control.c \
../main.c 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_MISC_DIRS += \
./configPkg/ 

C_DEPS += \
./EK_TM4C1294XL.d \
./com.d \
./control.d \
./main.d 

GEN_OPTS += \
./configPkg/compiler.opt 

OBJS += \
./EK_TM4C1294XL.o \
./com.o \
./control.o \
./main.o 

GEN_MISC_DIRS__QUOTED += \
"configPkg/" 

OBJS__QUOTED += \
"EK_TM4C1294XL.o" \
"com.o" \
"control.o" \
"main.o" 

C_DEPS__QUOTED += \
"EK_TM4C1294XL.d" \
"com.d" \
"control.d" \
"main.d" 

GEN_FILES__QUOTED += \
"configPkg/linker.cmd" \
"configPkg/compiler.opt" 

C_SRCS__QUOTED += \
"../EK_TM4C1294XL.c" \
"../com.c" \
"../control.c" \
"../main.c" 


