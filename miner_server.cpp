// miner_server.cpp — sequencial, TCP
#include <iostream>
#include <string>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "hash_sim.hpp"

constexpr int BLOCO_TOTAL = 40;
constexpr int PORTA = 8080;

void minerar_bloco(int id_bloco, std::string conteudo, int dificuldade) { 
    auto inicio = std::chrono::high_resolution_clock::now();

    std::string hash_base = sha256(conteudo);
    int nonce = 0;
    std::string tentativa;
    std::string hash;

    
    while (true) {
        tentativa = hash_base + std::to_string(nonce);
        hash = sha256(tentativa);

        if (validaHash(hash, dificuldade)) {
            double duracao = std::chrono::duration<double>(
                                 std::chrono::high_resolution_clock::now() - inicio
                             ).count();
            std::cout << "[BLOCK " << id_bloco << "] nonce=" << nonce
                      << " hash=" << hash << " tempo=" << duracao << "s\n";
            break;
        }
        ++nonce;
    }
}

int main(int argc, char* argv[]) {
    int dificuldade = 3;
    if (argc > 1) {
        try { dificuldade = std::stoi(argv[1]); }
        catch (...) { std::cerr << "[ERRO] Arg inválido. Usando dificuldade 3.\n"; dificuldade = 3; }
    }

    std::cout << "[INFO] Iniciando minerador TCP na porta " << PORTA
              << " com dificuldade " << dificuldade << "\n";

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) { perror("[ERRO] socket"); return 1; }

    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORTA);

    if (bind(srv, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("[ERRO] bind"); close(srv); return 1;
    }
    if (listen(srv, 16) < 0) {
        perror("[ERRO] listen"); close(srv); return 1;
    }
    std::cout << "[INFO] Aguardando conexões...\n";

    for (int bloco = 0; bloco < BLOCO_TOTAL; ++bloco) {
        int cli = accept(srv, nullptr, nullptr);
        if (cli < 0) { perror("[ERRO] accept"); --bloco; continue; }

        std::string conteudo;               
        char buf[256];                      
        ssize_t n;
        while ((n = recv(cli, buf, sizeof(buf), 0)) > 0) {
            conteudo.append(buf, buf + n);  
        }
        if (n < 0) { perror("[ERRO] recv"); }
        close(cli);

        minerar_bloco(bloco, conteudo, dificuldade); 
    }

    close(srv);
    std::cout << "[INFO] Finalizado\n";
    return 0;
}
