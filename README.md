# Projeto 1 – Minerador e Gerador de Transações em HPC

Este projeto implementa uma versão didática de um **sistema de mineração de blocos** e de um **gerador de transações**, pensado para rodar no **Cluster Franky** usando **SLURM**.  
O código foi escrito de forma propositalmente **ineficiente**, para que os alunos identifiquem gargalos de desempenho e os otimizem com técnicas de C++, OpenMP e MPI.

---

## Estrutura de Arquivos

```bash
├── hash_sim.hpp         # Biblioteca com funções auxiliares de hash (SHA-256 e validação de dificuldade)
├── miner                # Binário compilado do minerador (gerado a partir de miner_server.cpp)
├── miner.out            # Saída do SLURM com os logs de execução do minerador e cliente
├── miner_server.cpp     # Código-fonte do minerador
├── README.md            # Documentação do projeto
├── run.slurm            # Script SLURM para rodar o minerador + gerador de transações no cluster
├── transacoes           # Binário compilado do gerador de transações (a partir de transacoes.cpp)
└── transacoes.cpp       # Código-fonte do cliente/gerador de transações (envia blocos via socket)

````

## Descrição dos Componentes

### `hash_sim.hpp`
- Implementa o cálculo de **SHA-256**.
- Função `validaHash(hash, dificuldade)` que verifica se o hash tem o número correto de zeros no início.
- Usado tanto pelo minerador quanto pelo gerador para simulação.

### `miner_server.cpp`
- Código-fonte do **minerador sequencial**.
- Tenta encontrar um **nonce** que produza um hash válido para os blocos enviados pelo `transacoes`.

### `miner`
- Binário compilado do `miner_server.cpp`.

### `transacoes.cpp`
- Código-fonte do **gerador de transações (cliente)**.
- Cria blocos com transações fakes.
- Envia blocos sequenciais para o minerador através de **socket TCP**.

### `transacoes`
- Binário compilado do `transacoes.cpp`.

### `run.slurm`
- Script para rodar o projeto no **SLURM**.
- Passos principais:
  1. Sobe o minerador em segundo plano.
  2. Aguarda a porta TCP ficar disponível.
  3. Inicia o gerador de transações.
  4. Aguarda o término do minerador.

### `miner.out`
- Arquivo de saída com os logs do minerador e do cliente durante a execução.

## Execução com SLURM

Submeter o job:

```bash
sbatch run.slurm
```

Acompanhar a fila:

```bash
squeue -u $USER
```

Ver a saída:

```bash
cat miner.out
```
