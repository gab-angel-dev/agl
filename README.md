# Simbolos
================================================================================================
#: símbolo de comentário 


================================================================================================
$: símbolo de controle de fluxo

1. start: inicia o fluxo mas podendo aceitar parâmetros:
- into: diz o caminho que aquele arquivo tem que rodar, se não especificado ele roda no diretorio onde se encontra o .agl
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

2. active: para ativar a venv
ex: venv active

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
seguido pelo simbolo > para determinar o comando do terminal
ex: bash > python3 -m main.py 


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