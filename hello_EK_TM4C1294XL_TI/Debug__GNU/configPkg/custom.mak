## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,m4fg linker.cmd package/cfg/main_pm4fg.om4fg

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/main_pm4fg.xdl
	$(SED) 's"^\"\(package/cfg/main_pm4fgcfg.cmd\)\"$""\"/home/lullrich/Dokumente/Studium/MES/1_Sem/ESW/CopterProject/CopterProject_MES/hello_EK_TM4C1294XL_TI/Debug__GNU/configPkg/\1\""' package/cfg/main_pm4fg.xdl > $@
	-$(SETDATE) -r:max package/cfg/main_pm4fg.h compiler.opt compiler.opt.defs
