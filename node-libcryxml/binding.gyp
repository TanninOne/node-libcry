{
    "targets": [
        {
            "target_name": "cryxml",
            "includes": [
                "auto.gypi"
            ],
            "conditions": [
                ['OS=="win"', {
                    "sources": [
                        "src/cryxml.cpp"
                    ],
                }]
            ],
            "include_dirs": [
              "./libcryxml",
            ],
            "library_dirs": [
              "./libcryxml/dist",
            ],
            "libraries": [
              "libcryxml"
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
