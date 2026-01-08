set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)

# fast-cpp-csv-parser include path
# set(FAST_CPP_CSV_PARSER_INCLUDE_DIRS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/fast-cpp-csv-parser")

if (${PORT} MATCHES "fully-dynamic-game-engine|skse|qt*")
    set(VCPKG_LIBRARY_LINKAGE dynamic)
else ()
    set(VCPKG_LIBRARY_LINKAGE static)
endif ()
