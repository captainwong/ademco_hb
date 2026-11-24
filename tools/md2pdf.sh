#!/bin/bash
# Convert all docs/*.md to PDF (incremental: only re-run pandoc when md is newer than pdf).
# Used by mk_examples.sh so the distributed zip contains PDFs for users
# without GitHub access or markdown viewers.

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
DOCS_DIR=$(cd "$CURRENT_DIR/../docs" && pwd -P)

# On Windows pandoc cannot exec the extensionless npm shim directly,
# so use the .cmd shim.
MERMAID_FILTER=/c/Users/jack/AppData/Roaming/npm/mermaid-filter.cmd
MERMAID_FILTER_OPT=""
if [ -x "$MERMAID_FILTER" ]; then
    MERMAID_FILTER_OPT="--filter $MERMAID_FILTER"
fi

convert() {
    local md="$1"
    local pdf="$2"

    if [ ! -f "$pdf" ] || [ "$md" -nt "$pdf" ]; then
        echo "Converting $(basename "$md") -> $(basename "$pdf")"
        # gfm_auto_identifiers: keep CJK heading ids so #anchors match GitHub style
        pandoc "$md" -o "$pdf" \
            -f markdown+gfm_auto_identifiers \
            --pdf-engine=xelatex \
            -V CJKmainfont="宋体" \
            -V CJKoptions="BoldFont=黑体" \
            -V mainfont="Consolas" \
            -V monofont="Consolas" \
            -V geometry:margin=1in \
            $MERMAID_FILTER_OPT
    else
        echo "Up to date: $(basename "$pdf")"
    fi
}

cd "$DOCS_DIR"

convert "README.md" "协议文档.pdf"
convert "快速上手.md" "快速上手.pdf"
convert "对接说明.md" "对接说明.pdf"
convert "二次开发.md" "二次开发.pdf"
convert "模拟器使用说明.md" "模拟器使用说明.pdf"
convert "报文解析器使用说明.md" "报文解析器使用说明.pdf"
