{
    "targets": [
        {
            "target_name": "crypak",
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
              "./libcrypak/src",
            ],
            "library_dirs": [
              "./libcrypak/dist",
            ],
            "libraries": [
              "libcrypak"
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
