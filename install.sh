#!/bin/sh
# Instalador do AGL.
#
#   curl -fsSL https://raw.githubusercontent.com/gab-angel-dev/agl/main/install.sh | sh
#
# Variáveis opcionais:
#   AGL_VERSION      versão a instalar, ex.: v0.2.0   (padrão: a mais recente)
#   AGL_INSTALL_DIR  pasta de instalação              (padrão: $HOME/.local/bin)
#   AGL_REPO         usuario/repositorio no GitHub    (padrão: gab-angel-dev/agl)
set -eu

REPO="${AGL_REPO:-gab-angel-dev/agl}"
VERSION="${AGL_VERSION:-latest}"
INSTALL_DIR="${AGL_INSTALL_DIR:-$HOME/.local/bin}"
BASE_URL="${AGL_BASE_URL:-}"        # só para testes (ex.: file:///tmp/release)

say() { printf '%s\n' "$*"; }
die() { printf 'agl-install: erro: %s\n' "$*" >&2; exit 1; }

# ---- sistema e arquitetura -------------------------------------------------
case "$(uname -s)" in
    Linux)  os=linux ;;
    Darwin) os=macos ;;
    *) die "sistema '$(uname -s)' ainda não suportado. No Windows, use o WSL." ;;
esac

case "$(uname -m)" in
    x86_64|amd64)  arch=x86_64 ;;
    aarch64|arm64) arch=aarch64 ;;
    *) die "arquitetura '$(uname -m)' ainda não suportada." ;;
esac

target="$os-$arch"
asset="agl-$target.tar.gz"

# ---- URL -------------------------------------------------------------------
if [ -n "$BASE_URL" ]; then
    url="$BASE_URL/$asset"
elif [ "$VERSION" = "latest" ]; then
    url="https://github.com/$REPO/releases/latest/download/$asset"
else
    case "$VERSION" in v*) ;; *) VERSION="v$VERSION" ;; esac
    url="https://github.com/$REPO/releases/download/$VERSION/$asset"
fi

# ---- utilitários -----------------------------------------------------------
download() {    # download <url> <destino>
    if command -v curl >/dev/null 2>&1; then
        curl -fsSL "$1" -o "$2"
    elif command -v wget >/dev/null 2>&1; then
        wget -q "$1" -O "$2"
    else
        die "precisa de curl ou wget instalado."
    fi
}

sha256_de() {   # sha256_de <arquivo>
    if command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$1" | cut -d' ' -f1
    elif command -v shasum >/dev/null 2>&1; then
        shasum -a 256 "$1" | cut -d' ' -f1
    else
        echo ""
    fi
}

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT INT TERM

# ---- download e verificação --------------------------------------------------
say "Baixando agl ($target)..."
download "$url" "$tmp/$asset" \
    || die "não consegui baixar $url (a versão existe? há binário para $target?)"
download "$url.sha256" "$tmp/$asset.sha256" \
    || die "não consegui baixar o checksum de $asset"

esperado=$(cut -d' ' -f1 < "$tmp/$asset.sha256")
obtido=$(sha256_de "$tmp/$asset")

if [ -z "$obtido" ]; then
    say "aviso: sha256sum/shasum não encontrados, pulando a verificação do checksum."
elif [ "$esperado" != "$obtido" ]; then
    die "checksum não confere (esperado $esperado, obtido $obtido). Download corrompido?"
fi

# ---- instalação -------------------------------------------------------------
tar xzf "$tmp/$asset" -C "$tmp"
[ -f "$tmp/agl" ] || die "o pacote baixado não contém o binário 'agl'."

mkdir -p "$INSTALL_DIR"
cp "$tmp/agl" "$INSTALL_DIR/agl.new"
chmod 755 "$INSTALL_DIR/agl.new"
mv -f "$INSTALL_DIR/agl.new" "$INSTALL_DIR/agl"      # troca atômica (funciona mesmo com o agl em uso)

say "Instalado: $INSTALL_DIR/agl ($("$INSTALL_DIR/agl" version))"

# ---- PATH -------------------------------------------------------------------
case ":$PATH:" in
    *":$INSTALL_DIR:"*)
        say "Pronto! Rode: agl help"
        ;;
    *)
        case "${SHELL:-}" in
            */zsh)  rc="~/.zshrc" ;;
            */bash) rc="~/.bashrc" ;;
            *)      rc="~/.profile" ;;
        esac
        say ""
        say "'$INSTALL_DIR' não está no seu PATH. Adicione com:"
        say ""
        say "    echo 'export PATH=\"$INSTALL_DIR:\$PATH\"' >> $rc"
        say ""
        say "e abra um novo terminal. Depois rode: agl help"
        ;;
esac