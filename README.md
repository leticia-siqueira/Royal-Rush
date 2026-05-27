# Royal Rush

O jogador controla uma princesa guerreira que deve enfrentar diferentes inimigos utilizando ataques à distância, desviando de obstáculos e sobrevivendo o máximo possível para derrotar o inimigo chefe e alcançar a maior pontuação possível.

---

## Sobre o Jogo

Em Royal Rush, o jogador deve derrotar inimigos para acumular pontos e sobreviver aos desafios do cenário.

Durante a partida, é possível:

- Movimentar a personagem pelo mapa;
- Saltar entre plataformas;
- Atirar projéteis utilizando o mouse;
- Enfrentar diferentes tipos de inimigos;
- Acumular pontuação;
- Registrar o resultado em um ranking ao final da partida.

O jogo possui sistema de vidas, pontuação e armazenamento de ranking local.

---

## Controles do Jogo

| Ação | Comando |
|--------|--------|
| Mover para esquerda | A ou ← |
| Mover para direita | D ou → |
| Pular | Espaço ou ↑ |
| Atirar | Botão esquerdo do mouse |
| Confirmar telas | Enter |

---

## Requisitos

Para compilar e executar o projeto é necessário possuir:

- GCC
- Make
- Raylib
- Ubuntu/Linux

---

## Instalação das Dependências

Atualize os pacotes:

```bash
sudo apt update
```

Instale as ferramentas de compilação:

```bash
sudo apt install build-essential gcc make
```

Instale a biblioteca Raylib:

```bash
sudo apt install libraylib-dev
```

Caso sua versão do Ubuntu não possua esse pacote disponível, consulte a documentação oficial da Raylib para instalação manual.

---

## Clonando o Repositório 

```bash
git clone https://github.com/leticia-siqueira/Royal-Rush.git
```

Entre na pasta do projeto:

```bash
cd Royal-Rush
```

---

## Compilação 

Para compilar o jogo, execute:

```bash
make
```

Ao final da compilação será gerado o executável:

```bash
main
```

---

## Execução 

Execute o jogo com:

```bash
./main
```

Ou utilize:

```bash
make run
```

---

## Limpeza dos Arquivos Compilados

Para remover os arquivos gerados durante a compilação:

```bash
make clean
```

---

## Estrutura do Projeto

```text
Royal-Rush/
│
├── src/            
├── include/        
├── imagens/        
├── musicas/        
├── ranking.txt     
├── Makefile        
└── README.md
```

---

## Tecnologias Utilizadas

- Linguagem C
- Raylib
- GCC
- Makefile
- Ubuntu Linux

---

## Observações importantes

- O jogo deve ser executado a partir da pasta raiz do projeto.
- Os arquivos de imagem, áudio e ranking são carregados utilizando caminhos relativos.
- Certifique-se de que todos os recursos do projeto estejam presentes antes da execução.

---

## Equipe

- Leticia Maria da Silva Siqueira
- Davi Maltez Costa Vasconcelos
- Ranilton Araújo da Silva Raposo Júnior

  
## 👥 Equipe

Projeto desenvolvido como atividade acadêmica utilizando a linguagem C e a biblioteca Raylib para desenvolvimento de jogos 2D.
