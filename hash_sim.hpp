#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <functional>

// Conversão 64-bit -> 16 dígitos hex
inline std::string u64_to_hex(unsigned long long v) {
    // da pra otimizar algo aqui.
    std::ostringstream os;
    os << std::hex << std::nouppercase << std::setfill('0') << std::setw(16) << v;
    return os.str();
}

// "Hash" lenta (rounds + concatenações custosas)
inline std::string sha256_sim_bad(std::string s, int rounds = 5) {
    // DICA: se evitar cópias nas passagens dos parametros fica melhor.
    std::hash<std::string> H;
    std::string acc = s;

    for (int i = 0; i < rounds; ++i) {
        auto hv = static_cast<unsigned long long>(H(acc));
        // da pra otimizar coisas aqui também
        acc = u64_to_hex(hv) + "|" + acc;
    }
    auto hv = static_cast<unsigned long long>(H(acc));
    return u64_to_hex(hv);
}

inline bool valida_prefixo_zeros(const std::string& hex, int dificuldade) {
    if (dificuldade <= 0) return true;
    if (dificuldade > static_cast<int>(hex.size())) return false;
    // da pra desenrolar esse loop aqui
    for (int i = 0; i < dificuldade; ++i) if (hex[i] != '0') return false;
    return true;
}


inline std::string sha256(const std::string& s) {
    return sha256_sim_bad(s);
}

inline bool validaHash(const std::string& h, int d) {
    return valida_prefixo_zeros(h, d);
}
