{
    "targets":[
        {
            "target_name": "libademco_js",
            "sources":[
                "libademco_wrap.cxx",
                "../ademco.c",
                "../hb_core.c",
                "../hb_core_ademco.c",
                "../hb_rf.c",
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