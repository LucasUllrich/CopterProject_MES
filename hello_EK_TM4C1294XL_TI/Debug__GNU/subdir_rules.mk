################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/tobias/CCS8/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C1294NCPDT -I"/mnt/e70811e6-a2f3-439a-87af-aa14d26ca3b9/FHTW/Master/ESW/CopterProject_MES/hello_EK_TM4C1294XL_TI" -I"/home/tobias/CCS8/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/arm-none-eabi/include" -I"/home/tobias/ti/tivaware_c_series_2_1_4_178" -Og -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

build-172448111:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-172448111-inproc

build-172448111-inproc: ../main.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"/home/tobias/CCS8/xdctools_3_32_00_06_core/xs" --xdcpath="/home/tobias/ti/tirtos_tivac_2_16_00_08/packages;/home/tobias/ti/tirtos_tivac_2_16_00_08/products/tidrivers_tivac_2_16_00_08/packages;/home/tobias/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages;/home/tobias/ti/tirtos_tivac_2_16_00_08/products/ndk_2_25_00_09/packages;/home/tobias/ti/tirtos_tivac_2_16_00_08/products/uia_2_00_05_50/packages;/home/tobias/ti/tirtos_tivac_2_16_00_08/products/ns_1_11_00_10/packages;" xdc.tools.configuro -o configPkg -t gnu.targets.arm.M4F -p ti.platforms.tiva:TM4C1294NCPDT -r release -c "/home/tobias/CCS8/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-172448111 ../main.cfg
configPkg/compiler.opt: build-172448111
configPkg/: build-172448111


