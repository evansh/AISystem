################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
CompareValues-DevInit_F2806x.obj: ../CompareValues-DevInit_F2806x.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/evansTi/ccsv5/tools/compiler/c2000_6.2.0/bin/cl2000" --silicon_version=28 -g --define="_DEBUG" --define="LARGE_MODEL" --define="FLASH" --include_path="C:/evansTi/ccsv5/tools/compiler/c2000_6.2.0/include" --include_path="C:/evansTi/xdais_7_21_01_07/packages/ti/xdais" --include_path="C:/ti/controlSUITE/development_kits/~SupportFiles/F2806x_headers" --include_path="C:/ti/controlSUITE/device_support/f2806x/v100/F2806x_headers/include" --include_path="C:/ti/controlSUITE/device_support/f2806x/v100/F2806x_common/include" --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="CompareValues-DevInit_F2806x.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

CompareValues-Main.obj: ../CompareValues-Main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/evansTi/ccsv5/tools/compiler/c2000_6.2.0/bin/cl2000" --silicon_version=28 -g --define="_DEBUG" --define="LARGE_MODEL" --define="FLASH" --include_path="C:/evansTi/ccsv5/tools/compiler/c2000_6.2.0/include" --include_path="C:/evansTi/xdais_7_21_01_07/packages/ti/xdais" --include_path="C:/ti/controlSUITE/development_kits/~SupportFiles/F2806x_headers" --include_path="C:/ti/controlSUITE/device_support/f2806x/v100/F2806x_headers/include" --include_path="C:/ti/controlSUITE/device_support/f2806x/v100/F2806x_common/include" --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="CompareValues-Main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

F2806x_CodeStartBranch.obj: C:/ti/controlSUITE/device_support/f2806x/v100/F2806x_common/source/F2806x_CodeStartBranch.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/evansTi/ccsv5/tools/compiler/c2000_6.2.0/bin/cl2000" --silicon_version=28 -g --define="_DEBUG" --define="LARGE_MODEL" --define="FLASH" --include_path="C:/evansTi/ccsv5/tools/compiler/c2000_6.2.0/include" --include_path="C:/evansTi/xdais_7_21_01_07/packages/ti/xdais" --include_path="C:/ti/controlSUITE/development_kits/~SupportFiles/F2806x_headers" --include_path="C:/ti/controlSUITE/device_support/f2806x/v100/F2806x_headers/include" --include_path="C:/ti/controlSUITE/device_support/f2806x/v100/F2806x_common/include" --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="F2806x_CodeStartBranch.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

F2806x_GlobalVariableDefs.obj: C:/ti/controlSUITE/device_support/f2806x/v100/F2806x_headers/source/F2806x_GlobalVariableDefs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/evansTi/ccsv5/tools/compiler/c2000_6.2.0/bin/cl2000" --silicon_version=28 -g --define="_DEBUG" --define="LARGE_MODEL" --define="FLASH" --include_path="C:/evansTi/ccsv5/tools/compiler/c2000_6.2.0/include" --include_path="C:/evansTi/xdais_7_21_01_07/packages/ti/xdais" --include_path="C:/ti/controlSUITE/development_kits/~SupportFiles/F2806x_headers" --include_path="C:/ti/controlSUITE/device_support/f2806x/v100/F2806x_headers/include" --include_path="C:/ti/controlSUITE/device_support/f2806x/v100/F2806x_common/include" --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="F2806x_GlobalVariableDefs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


