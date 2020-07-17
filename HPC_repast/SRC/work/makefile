#************************************************************************************************************
#
# Based on Repast HPC Tutorial Makefile
#
#************************************************************************************************************

include ./env

.PHONY: all
all : 


.PHONY: clean_output_files
clean_output_files:
	rm -f *.csv
	rm -f *.txt
	rm -f ./output/*.csv
	rm -f ./output/*.txt
	rm -f ./logs/*.*

.PHONY: clean_compiled_files
clean_compiled_files:
	rm -f *.exe
	rm -f ./bin/*.exe
	rm -f *.o
	rm -f ./object/*.o
	
.PHONY: clean
clean: clean_compiled_files clean_output_files remove_subdirectories
	rm -f *.cpp
	rm -f ./src/*.cpp
	rm -f *.props
	rm -f ./props/*.props
	rm -f ./include/*.h
	
.PHONY: remove_subdirectories
remove_subdirectories:
	rm -fr objects
	rm -fr output
	rm -fr bin
	rm -fr include
	rm -fr src
	rm -fr props
	mkdir objects
	mkdir output
	mkdir bin


.PHONY: RepastHPC_code_01
RepastHPC_code_01: clean_compiled_files
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -I./include -c ./src/Main.cpp -o ./objects/Main.o
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -I./include -c ./src/Model.cpp -o ./objects/Model.o
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -I./include -c ./src/Agent.cpp -o ./objects/Agent.o
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -I./include -c ./src/Probabilities.cpp -o ./objects/Probabilities.o
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -I./include -c ./src/TickConverter.cpp -o ./objects/TickConverter.o
	$(MPICXX) $(BOOST_LIB_DIR) $(REPAST_HPC_LIB_DIR) -o ./bin/code_01.exe  ./objects/Main.o ./objects/Model.o ./objects/Agent.o ./objects/Probabilities.o ./objects/TickConverter.o $(REPAST_HPC_LIB) $(BOOST_LIBS)

.PHONY: RepastHPC_code_01_step_01
RepastHPC_code_01_step_01 : copy_RepastHPC_code_01_step_01 RepastHPC_code_01

.PHONY: copy_RepastHPC_code_01_step_01
copy_RepastHPC_code_01_step_01 : clean
	cp -r ../RepastHPC/code_01/step_01/* ./
