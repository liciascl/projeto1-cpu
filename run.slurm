#!/bin/bash
# ------------------------------
# Script SLURM para rodar minerador + cliente
# ------------------------------

#SBATCH --job-name=miner          # Nome do job (aparece no squeue)
#SBATCH --output=miner.out        # Arquivo de saída do job
#SBATCH --nodes=1                 # Número de nós de computação alocados
#SBATCH --ntasks=1                # Número de Ranks MPI solicitados
#SBATCH --cpus-per-task=1         # Quantidade de CPUs para cada thread
#SBATCH --mem=2G                  # Memória RAM reservada
#SBATCH --time=03:20:00           # Tempo máximo de execução
#SBATCH --partition=gpu           # Fila do cluster 

set -euo pipefail                 # Boas práticas: interrompe se houver erro, variáveis indefinidas ou pipe quebrado

PORTA=8080                        # Porta TCP usada pelo minerador 

echo "[INFO] Rodando em $(hostname)"   # Mostra em qual nó o script está rodando

echo "[INFO] Iniciando minerador"
time ./miner 8 &                       # Executa o minerador (com dificuldade 3) em segundo plano
PID_MINER=$!                      # Salva o PID do minerador para monitorar e esperar no final

echo "[INFO] Aguardando porta ${PORTA}..."
# Laço que verifica se a porta do minerador já está aberta
for i in {1..80}; do
  if ss -lnt | grep -q ":${PORTA} "; then   # Checa se a porta está em LISTEN
    echo "[INFO] Porta ${PORTA} disponível."
    break
  fi
  sleep 0.25                               # Espera 250ms entre tentativas
  # Se o minerador morrer antes da porta abrir, encerra com erro
  kill -0 "$PID_MINER" 2>/dev/null || { echo "[ERRO] Minerador saiu."; exit 1; }
done

# Se depois das tentativas a porta não estiver aberta, encerra o job
ss -lnt | grep -q ":${PORTA} " || { echo "[ERRO] Porta ${PORTA} não abriu."; exit 1; }

echo "[INFO] Iniciando gerador de transações"
./transacoes                           # Executa o cliente que envia os blocos via socket

echo "[INFO] Aguardando minerador encerrar"
wait "$PID_MINER"                      # Espera o processo do minerador terminar
echo "[INFO] Job finalizado"
