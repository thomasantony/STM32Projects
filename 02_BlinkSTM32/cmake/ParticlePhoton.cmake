macro(SetupPhotonFirmware)
    include(ExternalProject)
    find_package(Git REQUIRED)

    set(PARTICLE_PLATFORM "photon" CACHE STRING "Particle platform to compile for")
    set(FIRMWARE_DIR "${CMAKE_SOURCE_DIR}/libs/particle-firmware")
    set(APPDIR "${CMAKE_CURRENT_SOURCE_DIR}")
    set(TARGET_DIR "${CMAKE_BINARY_DIR}")
    set(TARGET_FILE "${PROJECT}-${PARTICLE_PLATFORM}")
    message(STATUS ${PARTICLE_PLATFORM})

    set(MAKE_FLAGS
            PLATFORM=${PARTICLE_PLATFORM} #MODULAR=n #PLATFORM_THREADING=0
            APPDIR=${APPDIR} TARGET_DIR=${TARGET_DIR} TARGET_FILE=${TARGET_FILE}
    )
    # add_definitions(-DPLATFORM_THREADING=0)
    add_definitions(-DPLATFORM_NAME=${PARTICLE_PLATFORM})
    if (${PARTICLE_PLATFORM} STREQUAL "photon")
        add_definitions(-DPLATFORM_ID=6 -DPRODUCT_ID=6)
    endif ()

    # Download firmware and make deps.
    ExternalProject_Add(
        particle-firmware-deps
        EXCLUDE_FROM_ALL 1
        SOURCE_DIR "${FIRMWARE_DIR}"
        GIT_REPOSITORY "https://github.com/particle-iot/firmware/"
        GIT_TAG "release/stable"
        # URL https://github.com/particle-iot/firmware/archive/v0.8.0-rc.3.tar.gz
        CONFIGURE_COMMAND ""
        WORKING_DIRECTORY "${FIRMWARE_DIR}/main"
        BINARY_DIR "${FIRMWARE_DIR}/main"
        BUILD_COMMAND make make_deps -j1 ${MAKE_FLAGS}
        INSTALL_COMMAND ""
    )

    # # CMSIS library
    # set(CMSIS_ROOT "${CMAKE_SOURCE_DIR}/libs/CMSIS/CMSIS" CACHE PATH "Path to the CMSIS root directory")
    # add_library(cmsis STATIC IMPORTED GLOBAL)
    # set_target_properties(
    #     cmsis PROPERTIES
    #     INTERFACE_INCLUDE_DIRECTORIES "${CMSIS_ROOT}/Include"
    #     INTERFACE_COMPILE_DEFINITIONS "ARM_MATH_CM3"
    #     IMPORTED_LOCATION "${CMSIS_ROOT}/Lib/GCC/libarm_cortexM3l_math.a"
    # )

    # Build full firmware
    add_custom_target(firmware ALL
        DEPENDS particle-firmware-deps
        WORKING_DIRECTORY "${FIRMWARE_DIR}/main"
        COMMAND make all ${MAKE_FLAGS}
        # COMMAND make all ${MAKE_FLAGS} ADDITIONAL_LIB_DIRS='$<TARGET_FILE_DIR:${PROJECT_TARGET}> $<TARGET_FILE_DIR:cmsis>' ADDITIONAL_LIBS='${PROJECT_TARGET} arm_cortexM3l_math'

    )

    add_custom_target(firmware-clean
        DEPENDS particle-firmware-deps
        WORKING_DIRECTORY "${FIRMWARE_DIR}/main"
        COMMAND make clean ${MAKE_FLAGS}
    )

    add_custom_target(firmware-upload
        DEPENDS firmware
        DEPENDS particle-firmware-deps
        WORKING_DIRECTORY "${FIRMWARE_DIR}/main"
        COMMAND make all program-dfu ${MAKE_FLAGS}
    )

    add_custom_target(firmware-assembler
        DEPENDS firmware
        COMMAND ${CMAKE_OBJDUMP} --demangle --disassemble --source --wide ${TARGET_DIR}/${TARGET_FILE}.elf > ${TARGET_DIR}/${TARGET_FILE}-source.txt
    )

    # target_link_libraries(${PROJECT_TARGET} PRIVATE cmsis)
    # add_dependencies(${PROJECT_TARGET} particle-firmware-deps)
endmacro()
