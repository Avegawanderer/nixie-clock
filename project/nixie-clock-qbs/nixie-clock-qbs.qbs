import qbs
import qbs.FileInfo
import qbs.ModUtils

Product {    
    name: "nixie-clock"
    type: ["application"]
    consoleApplication: true
    Depends { name:"cpp" }

    cpp.positionIndependentCode: false
    cpp.debugInformation: true
    cpp.executablePrefix: ""
    cpp.executableSuffix: ".elf"


    property string srcPath: "../../source/"
    property string libPath: "../../library/"

    cpp.includePaths: [        
        srcPath,
        libPath + "STM32F10x_StdPeriph_Driver/",
        libPath + "STM32F10x_StdPeriph_Driver/inc/",
        libPath + "CMSIS/CoreSupport",
        libPath + "CMSIS/DeviceSupport/STM32F10x",
    ]

    files: [        
    ]

    cpp.defines: [
        "STM32F10X",
        "USE_STDPERIPH_DRIVER",
        "STM32F10X_MD",
        "HSE_VALUE=16000000"
    ]

    Properties {
        condition: cpp.debugInformation
        cpp.defines: outer.concat("DEBUG")
    }

    cpp.commonCompilerFlags: [
        "-mfpu=vfp",
        "-O0",
        "-fdata-sections",
        "-ffunction-sections",
    ]

    cpp.driverFlags: [
        "-mthumb",
        "-mcpu=cortex-m3",
        "-mfloat-abi=soft",
        "--specs=nosys.specs",
    ]

    cpp.linkerFlags: [
        "-T"+path+"/linker/stm32f103c8_flash.ld",
        "-Map="+path+"/output.map"
    ]

    cpp.cxxFlags: ["-std=c++11"]
    cpp.cFlags: ["-std=gnu99"]
    cpp.warningLevel: "all"


    Group {
        name: "Sources"
        prefix: srcPath
        files: [
            "*.c",
            "*.cpp",
            "*.h",
            "startup_ARMCM3.S",
        ]
        excludeFiles: [
        ]
    }

    Group {
        name: "StdPeriph_Driver"
        prefix: libPath+"STM32F10x_StdPeriph_Driver/**/"
        files: [
            "*.h",
            "*.c",
        ]
    }

    Group {
        name: "DeviceSupport"
        prefix: libPath+"CMSIS/DeviceSupport/STM32F10x/"
        files: [
            "*.h",
            "*.c",
        ]
    }

    Group {
        name: "CoreSupport"
        prefix: libPath+"CMSIS/CoreSupport/"
        files: [
            "*.h",
            "*.c",
        ]
    }

    Group {
        name: "Linker"
        prefix: "linker/"
        files: [
            "*.ld",
        ]
    }

}
