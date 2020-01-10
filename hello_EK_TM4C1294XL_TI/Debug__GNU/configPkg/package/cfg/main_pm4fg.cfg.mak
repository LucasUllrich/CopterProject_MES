# invoke SourceDir generated makefile for main.pm4fg
main.pm4fg: .libraries,main.pm4fg
.libraries,main.pm4fg: package/cfg/main_pm4fg.xdl
	$(MAKE) -f /home/lullrich/Dokumente/Studium/MES/1_Sem/ESW/CopterProject/CopterProject_MES/hello_EK_TM4C1294XL_TI/src/makefile.libs

clean::
	$(MAKE) -f /home/lullrich/Dokumente/Studium/MES/1_Sem/ESW/CopterProject/CopterProject_MES/hello_EK_TM4C1294XL_TI/src/makefile.libs clean

