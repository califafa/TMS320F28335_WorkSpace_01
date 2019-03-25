################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
DSP2833x_headers/source/%.obj: ../DSP2833x_headers/source/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="C:/Users/Cal/Documents/files/CCS_workSpace_pre_02/daoLiBai_01" --include_path="C:/Users/Cal/Documents/files/CCS_workSpace_pre_02/daoLiBai_01/DSP2833x_common/include" --include_path="C:/Users/Cal/Documents/files/CCS_workSpace_pre_02/daoLiBai_01/DSP2833x_headers/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.4.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="DSP2833x_headers/source/$(basename $(<F)).d_raw" --obj_directory="DSP2833x_headers/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


