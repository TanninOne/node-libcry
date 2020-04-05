{
    "targets": [
        {
            "target_name": "pakdecrypt",
            "includes": [
                "auto.gypi"
            ],
            "conditions": [
                ['OS=="win"', {
                    "sources": [
                        "src/pakdecrypt.cpp"
                    ],
                }]
            ],
            "include_dirs": [
              "../libpakdecrypt",
            ],
            "library_dirs": [
              "../x64/Release",
            ],
            "libraries": [
              "libpakdecrypt"
            ],
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-exceptions"],
            "defines": [
                "UNICODE",
                "_UNICODE"
            ],
            "msvs_settings": {
                "VCCLCompilerTool": {
                    "ExceptionHandling": 1
                }
            }
        }
    ],
    "includes": [
        "auto-top.gypi"
    ]
}
