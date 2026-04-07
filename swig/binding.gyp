{
    "targets":[
        {
            "target_name": "libademco_js",
            "sources":[
                "libademco_wrap.cxx",
                "../src/libademco/ademco.c",
                "../src/libhb/hb_core.c",
                "../src/libhb/hb_core_ademco.c",
                "../src/libhbiot/rf/hb_rf.c",
            ],
            "include_dirs": [
                "../src",
            ],
            'conditions': [
                ['OS=="win"', {
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            "AdditionalOptions": [
                            "/source-charset:utf-8",
                        ]},
                    },
                }],
            ],
        }
    ]
}