#!/bin/sh
# Gera um .c com o conteúdo dos templates embutido como strings.
# Uso: gen_templates.sh <saida.c> <template1.agl> [template2.agl ...]
set -e

out="$1"
shift

{
    echo '/* GERADO por scripts/gen_templates.sh - não edite */'
    echo '#include "templates.h"'
    echo ''
    echo 'const Template TEMPLATES[] = {'
    for f in "$@"; do
        nome=$(basename "$f" .agl)
        printf '    {"%s",\n' "$nome"
        tr -d '\r' < "$f" \
          | sed -e 's/\\/\\\\/g' -e 's/"/\\"/g' -e 's/^/     "/' -e 's/$/\\n"/'
        echo '    },'
    done
    echo '};'
    echo ''
    echo "const int TOTAL_TEMPLATES = $#;"
} > "$out"