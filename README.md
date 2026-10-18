# agl

<a href="https://github.com/gab-angel-dev/agl/releases"><img src="https://img.shields.io/github/v/release/gab-angel-dev/agl" alt="Última versão" /></a>
<a href="https://github.com/gab-angel-dev/agl/blob/main/LICENSE"><img src="https://img.shields.io/github/license/gab-angel-dev/agl" alt="Licença MIT" /></a>
<img src="https://img.shields.io/badge/plataformas-Linux%20%7C%20macOS-blue" alt="Plataformas: Linux e macOS" />

DSL para automatizar a criação de projetos. Escrita em C.

```console
$ agl run template data-python
+ dir   /home/user/meu-projeto/data/raw
+ dir   /home/user/meu-projeto/notebooks
+ file  /home/user/meu-projeto/src/pipeline.py
+ venv  /home/user/meu-projeto/.venv
Initialized empty Git repository in /home/user/meu-projeto/.git/
Template data-python criado.
```

## Destaques

- Um único binário, sem dependências: instala com `curl`, sem precisar de compilador.
- [Templates prontos](#templates) embutidos, como `data-python` e `agents-python`.
- Linguagem declarativa com `create`, `venv`, `git`, `bash` e `print`.
- Cria as pastas que faltam e nunca sobrescreve arquivos existentes.
- Valida a sintaxe sem executar nada, com erros que apontam a linha.
- Suporta Linux e macOS (x86_64 e arm64).

## Instalação

```bash
# No Linux e no macOS.
curl -fsSL https://raw.githubusercontent.com/gab-angel-dev/agl/main/install.sh | sh
```

Para atualizar, rode o mesmo comando de novo. Para instalar uma versão específica ou em outra pasta, use
`AGL_VERSION=v0.3.1` ou `AGL_INSTALL_DIR=/usr/local/bin` antes do `sh`.

Ou pelo código-fonte (precisa de `gcc` e `make`):

```bash
git clone https://github.com/gab-angel-dev/agl.git && cd agl
make && sudo make install
```

No Windows, use o [WSL](https://learn.microsoft.com/windows/wsl/).

## Uso

```console
$ agl run <arquivo.agl>       # executa a partir do diretório atual
$ agl run template <nome>     # executa um template embutido
$ agl templates               # lista os templates
$ agl check <arquivo.agl>     # valida a sintaxe, sem executar
$ agl help
```

Um arquivo `.agl` é uma lista de comandos entre `$ start` e `$ stop`:

```text
# criando a base do projeto
$ start

create dir src, tests, docs
create file main.py in src

venv create .venv
git init

print "Projeto inicializado"

$ stop
```

## Templates

```console
$ agl templates
Templates disponíveis:

  agents-python: Projeto de agentes em Python com LangChain/LangGraph (agents, tools, graphs, prompts)
  data-python:   Projeto de dados em Python (pandas, notebooks, pipeline e testes)

Uso: agl run template <nome>
```

Para criar um template novo, adicione `templates/<nome>.agl` com `# description: ...` na primeira linha e
rode `make`.

## Licença

O agl é licenciado sob a [licença MIT](LICENSE).