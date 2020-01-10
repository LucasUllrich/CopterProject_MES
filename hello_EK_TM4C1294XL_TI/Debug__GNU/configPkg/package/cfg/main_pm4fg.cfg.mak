# invoke SourceDir generated makefile for main.pm4fg
main.pm4fg: .libraries,main.pm4fg
.libraries,main.pm4fg: package/cfg/main_pm4fg.xdl
	$(MAKE) -f /mnt/e70811e6-a2f3-439a-87af-aa14d26ca3b9/FHTW/Master/ESW/CopterProject_MES/hello_EK_TM4C1294XL_TI/src/makefile.libs

clean::
	$(MAKE) -f /mnt/e70811e6-a2f3-439a-87af-aa14d26ca3b9/FHTW/Master/ESW/CopterProject_MES/hello_EK_TM4C1294XL_TI/src/makefile.libs clean

