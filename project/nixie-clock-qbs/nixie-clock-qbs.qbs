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

//    cpp.defines: [
//        "STM32F10X",
//        "USE_STDPERIPH_DRIVER",
//        "STM32F10X_MD",
//        "HSE_VALUE=16000000",
//    ]

//    Properties {
//        condition: cpp.debugInformation
//        cpp.defines: outer.concat("__DEBUG")
//    }

    cpp.defines: {
        var defines = [
            "STM32F10X",
            "USE_STDPERIPH_DRIVER",
            "STM32F10X_MD",
            "HSE_VALUE=16000000",
        ];
        if (qbs.buildVariant.contains("debug"))
            defines.push("__DEBUG");
        return defines;
    }


    cpp.commonCompilerFlags: [
        "-O0",
        "-fdata-sections",
        "-ffunction-sections",
    ]

    cpp.driverFlags: [
        "-mthumb",
        "-mcpu=cortex-m3",
        "--specs=nosys.specs",
        "--specs=rdimon.specs",
        "-lc",
        "-lrdimon",
        "-Wl,--gc-sections"     // Remove unreferenced sections
    ]

    cpp.linkerFlags: [
        "-T"+path+"/linker/libs.ld",
        "-T"+path+"/linker/mem.ld",
        "-T"+path+"/linker/sections.ld",
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
            "startup_stm32f10x_md.s",
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
