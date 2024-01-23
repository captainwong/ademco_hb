{
    "targets":[
        {
            "target_name": "libademco_js",
            "sources":[
                "libademco_wrap.cxx",
                "../ademco.c",
                "../hb_com.c",
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