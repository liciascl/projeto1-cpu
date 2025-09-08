// Versão sequencial do tx_client enviando blocos via socket 
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

constexpr int BLOCO_TOTAL = 40;
constexpr int TX_POR_BLOCO = 35;
constexpr int PORTA = 8080;
constexpr const char* ENDERECO = "127.0.0.1";  // altere para o IP do nó minerador se necessário

// Gera uma transação simples
std::string gerar_transacao(int id_bloco, int id_tx) {
    return "{\"bloco\": " + std::to_string(id_bloco) +
           ", \"tx\": " + std::to_string(id_tx) +
           ", \"conteudo\": \"exemplo\"}";
}

// Envia um bloco completo para o minerador
void enviar_bloco(int id_bloco, const std::vector<std::string>& transacoes) {
    std::ostringstream oss;
    for (const auto& tx : transacoes) {
        oss << tx << "\n";
    }
    std::string corpo = oss.str();

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "[ERRO] Falha ao criar socket\n";
        return;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORTA);

    if (inet_pton(AF_INET, ENDERECO, &serv_addr.sin_addr) <= 0) {
        std::cerr << "[ERRO] Endereço inválido\n";
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "[ERRO] Conexão falhou\n";
        close(sock);
        return;
    }

    ssize_t sent = send(sock, corpo.c_str(), corpo.size(), 0);
    if (sent < 0) {
        std::cerr << "[ERRO] Falha no envio\n";
    }
    close(sock);

    std::cout << "[TXGEN] Bloco " << id_bloco
              << " enviado com " << transacoes.size() << " transações.\n";
}

int main() {
    std::cout << "[TXGEN] Iniciando gerador de transações (sequencial, sem pausa): "
              << BLOCO_TOTAL << " blocos, "
              << TX_POR_BLOCO << " transações/bloco\n";

    for (int bloco = 0; bloco < BLOCO_TOTAL; ++bloco) {
        std::vector<std::string> transacoes;
        transacoes.reserve(TX_POR_BLOCO);
        for (int tx = 0; tx < TX_POR_BLOCO; ++tx) {
            transacoes.push_back(gerar_transacao(bloco, tx));
        }
        enviar_bloco(bloco, transacoes);
    }

    std::cout << "[TXGEN] Finalizado.\n";
    return 0;
}
