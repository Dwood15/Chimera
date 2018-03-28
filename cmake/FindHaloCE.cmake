#This way we can have a global haloce file.
set(CE_EXE_NAME "haloce.exe")
set(HCE_REG_KEY "HKEY_LOCAL_MACHINE\\SOFTWARE\\")

cmake_host_system_information(RESULT HOST_IS_64 QUERY IS_64BIT)
message(STATUS "HOST_IS_64?: ${HOST_IS_64} ")

IF (${CMAKE_HOST_WIN32})
	if (${HOST_IS_64})
		#don't know if this is _actually_ necessary or not. Pretty sure it is?
		set(HCE_REG_KEY "${HCE_REG_KEY}Wow6432Node\\Microsoft\\Microsoft\ Games\\Halo\ CE")
	else ()
		set(HCE_REG_KEY "${HCE_REG_KEY}Microsoft\\Microsoft\ Games\\Halo\ CE")
	endif ()
else ()
	message(FATAL_ERROR "Unable to build on non-win32 host. If you're on windows (not wine or any flavor of linux) and see this message, complain to dwood.")
endif ()

message(STATUS "CE REG KEY: ${HCE_REG_KEY}")
get_filename_component(HaloCE_ROOT_DIR "[${HCE_REG_KEY};EXE\ Path]" ABSOLUTE CACHE NO_DEFAULT_PATH)

#Todo: Figure out how to get version number from cmake's registry functionality.
#get_property(HaloCE_VERSION CACHE "[${HCE_REG_KEY}]" PROPERTY "Version")

message(STATUS "CE DIR: ${CE_DIR}")
#message(STATUS "CE VER: ${HaloCE_VERSION}")

#get_filename_component(EXE_DIR

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HaloCE DEFAULT_MSG HaloCE_ROOT_DIR)
mark_as_advanced(HaloCE_ROOT_DIR)
