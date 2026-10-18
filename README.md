# Instalação
================================================================================================
Linux e macOS (x86_64 e arm64):

```
curl -fsSL https://raw.githubusercontent.com/gab-angel-dev/agl/main/install.sh | sh
```

Instala em `~/.local/bin/agl`. Se essa pasta não estiver no seu PATH, o instalador mostra o comando para adicionar.
Para atualizar, rode o mesmo comando de novo. Para desinstalar: `rm ~/.local/bin/agl`.

Opções (variáveis de ambiente):
- `AGL_VERSION=v0.2.0` instala uma versão específica
- `AGL_INSTALL_DIR=/usr/local/bin` muda a pasta de instalação

Pelo código-fonte (precisa de gcc e make):

```
git clone https://github.com/gab-angel-dev/agl.git && cd agl
make && sudo make install
```

Windows: use o WSL. Para os comandos de venv é preciso ter `python3` com o módulo `venv`.


# Uso
```
agl run <arquivo.agl>     executa a partir do diretório atual
agl run template <nome>   executa um template embutido
agl templates             lista os templates e descrições
agl check <arquivo.agl>   só valida a sintaxe
agl version | agl help
```
Comandos só funcionam entre `$ start` e `$ stop`.
Saída: `+` criado, `=` já existia, `~` venv.

# Simbolos
================================================================================================
#: símbolo de comentário 


================================================================================================
$: símbolo de controle de fluxo

1. start: inicia o fluxo mas podendo aceitar parâmetros:
- into: diz em qual pasta o bloco roda (criada automaticamente se não existir). Relativo ao diretório de onde você rodou o `agl run`, ou absoluto.
- sem `into`: o bloco roda no diretório atual de quem executou o `agl run`
ex: $ start into src/projeto/

1. stop: encerra o bloco de instruções

================================================================================================


# Comandos
================================================================================================
create: comando para criar pastas e arquivos

1. dir: cria pastas 
parametros:
- nome: nome da pasta
ex: create dir teste

2. file: cria arquivo
parametros:
- nome: nome do arquivo
ex: create file teste.py

o comando create aceita varios arquivos separados por virgula
ex: create file teste.py, arquivo.c, programa.txt


================================================================================================
in: comando de destino. Ele pode see usado em junto de outros comandos como create, venv, git 

ex: create dir teste in src/
ex: create file teste.py in src/teste/


obs: caso não seja passado o comando in ele cria no diretorio base definido no start


================================================================================================
venv: comando de manipular a venv

1. create: cria a venv e recebe o parametro nome

ex: venv create .venv
ex: venv create .venv in dir/projeto

2. active: ativa a venv (vale para os `bash >` seguintes, até `venv deactive` ou `$ stop`). Sem nome usa a última criada, ou `.venv`
ex: venv active
ex: venv active .venv in dir/projeto

3. deactive: desativa a venv
ex: venv deactive


================================================================================================
git: comando para manipular git

1. init: inicializa o git no projeto
ex: git init
ex: git init in projeto/teste/

2. ignr: cria .gitignore
ex: git ignr


================================================================================================
bash: comando que roda algo no terminal
seguido pelo simbolo > ; o resto da linha vai direto para o shell (roda na base do bloco; falhou = o script para)
ex: bash > python3 main.py 


================================================================================================
print: comando que printa algo
ex: print "Projeto inicializado"


================================================================================================



# Exemplo de codigo
================================================================================================

# criando diretorios principais 
$ start

create dir src
create dir projeto in src/

$ stop

# estruturando projeto dentro do criado
$ start into src/projeto/

venv create .venv
venv active

create dir teste
create file teste.py in teste

git init
git ignr

bash > ls -la

print "Projeto inicializado"

$ stop


# Templates
================================================================================================
Arquivos `.agl` em `templates/` são embutidos no binário no `make`. A descrição que aparece em
`agl templates` vem da primeira linha do arquivo:

```
# description: Projeto de dados em Python
$ start
...
```

Para criar um template novo: adicione `templates/<nome>.agl` e rode `make`.