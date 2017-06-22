#include "RSA.h"
#include <fstream>
#include "PRNG.h"
#include <iostream>
#include <string>
#include <sstream>
#include <NTL/ZZ.h>
#include "Funciones Matematicas.h"
using namespace NTL;
using namespace std;
void RSA::generar_claves(int bits)
{
    p = ga(200,bits,7,3);
    q = ga(200,bits,3,4);
    while(ProbPrime(p,10)!=1)
    {
        p = ga(200,bits,7,3);
    }
    while(ProbPrime(q,10)!=1)
    {
        q = ga(200,bits,3,4);
    }
    N = p * q;
    ZZ phi_N = (p - 1) * (q - 1);
    e = ga(200,bits,3,2);
    while(e > phi_N || euclides(e, phi_N) != 1)
    {
        e = ga(200,bits,4,5);
    }
    d = inversa(e, phi_N);
}
string RSA::firmar(string firma)
{
    string firma_cifrada;
    int digitos_N_r = obtener_digitos(N_r);
    string bloque, string_potencia, aux, aux_2;
    ZZ res_potencia;
    for(int i = 0; i < firma.size(); i++)
    {
        res_potencia = potencia(to_ZZ(alfabeto.find(firma[i])), d, N);
        string_potencia = to_string(res_potencia);
        if(string_potencia.size() > (digitos_N_r - 1))
        {
            for(int i = 0; i < ((digitos_N_r - 1) - modulo(to_ZZ(string_potencia.size()), to_ZZ(digitos_N_r - 1))); i++)
            {
                bloque += '0';
            }
        }
        bloque += string_potencia;
    }
    res_potencia = 0;
    int count = 0;
    for(int i = 0; i < bloque.size(); i++)
    {
        aux += bloque[i];
        count++;
        if(count == (digitos_N_r - 1))
        {
            res_potencia = potencia(string_toZZ(aux), e_r, N_r);
            if(obtener_digitos(res_potencia) < digitos_N_r)
            {
                for(int i = 0; i < (digitos_N_r - obtener_digitos(res_potencia)); i++)
                {
                    aux_2 += '0';
                }
            }
            aux_2 += to_string(res_potencia);
            firma_cifrada += aux_2;
            count = 0;
            res_potencia = 0;
            aux_2 = "";
        }
    }
    while(firma_cifrada.size() % (digitos_N_r - 1) != 0)
    {
            firma_cifrada += to_string(to_ZZ(alfabeto.find("w")));
    }
    tam_firma = firma_cifrada.size();
    return firma_cifrada;
}
RSA::RSA(int bits, ZZ e_r, ZZ N_receptor) //EMISOR
{
    generar_claves(bits);
    this->e_r = e_r;
    N_r = N_receptor;
}
RSA::RSA(int bits) //RECEPTOR
{
    generar_claves(bits);

    ofstream claves;
    claves.open ("claves_mias.txt");
    claves << "e: " << e << endl;
    claves << "d: " << d << endl;
    claves << "N: " << N << endl;
    claves << "p: " << p << endl;
    claves << "q: " << q << endl;
    claves.close();
    ofstream claves_o;
    claves_o.open ("claves.txt");
    claves_o << "e: " << e << endl;
    claves_o << "N: " << N << endl;
    claves_o.close();
}
void RSA::set_N(ZZ n)
{
    N = n;
}
void RSA::set_e(ZZ E)
{
    e = E;
}
void RSA::set_d(ZZ D)
{
    d = D;
}
void RSA::set_p(ZZ P)
{
    p = P;
}
void RSA::set_q(ZZ Q)
{
    q = Q;
}
int RSA::get_tamfirma()
{
    return tam_firma;
}
ZZ RSA::get_e()
{
    return e;
}
ZZ RSA::get_n()
{
    return N;
}
ZZ RSA::resto_chino(ZZ c)
{
    ZZ P = p * q;
    ZZ dp = modulo(d, p - 1);
    ZZ dq = modulo(d, q - 1);
    ZZ a_1 = potencia(modulo(c, p), dp, p);
    ZZ a_2 = potencia(modulo(c, q), dq, q);
    ZZ p_1 = P/p;
    ZZ p_2 = P/q;
    ZZ q_1 = inversa(p_1, p);
    ZZ q_2 = inversa(p_2, q);
    ZZ d_0 = modulo(modulo((a_1 * p_1 * q_1), P) + modulo((a_2 * p_2 * q_2), P), P);
    return d_0;
}
string RSA::cifrar(string mensaje, string firma)
{
    string firma_cifrada;
    firma_cifrada = firmar(firma);
    int tam_firma = firma_cifrada.size();
    int digitos_alfabeto = obtener_digitos(to_ZZ((alfabeto.length() - 1)));
    int digitos_N = obtener_digitos(N);

    string mensaje_cifrado, mensaje_temporal;
    ZZ numero_letra;
    string string_letra, string_letra_temp;
    for(int i = 0; i < mensaje.length(); i++)
    {
        string_letra_temp = "";
        numero_letra = to_ZZ(alfabeto.find(mensaje[i]));
        string_letra = to_string(numero_letra);
        if(obtener_digitos(numero_letra) < digitos_alfabeto) //llenar con 0 si es que tiene menos digitos 2 -> 02
        {
            for(int i = 0; i < digitos_alfabeto - obtener_digitos(numero_letra); i++)
            {
                string_letra_temp += "0";
            }
            string_letra_temp += string_letra;
            mensaje_temporal += string_letra_temp;
        }
        else
            mensaje_temporal += string_letra;
    }
    while(mensaje_temporal.size() % (digitos_N - 1) != 0)
    {
            mensaje_temporal += to_string(to_ZZ(alfabeto.find("w")));
    }
    string aux;
    string string_letra_cifrada;
    int count = 0;
    for(int i = 0; i < mensaje_temporal.length(); i++)
    {
        aux += mensaje_temporal[i];
        count += 1;
        if(count == (digitos_N - 1))
        {
            ZZ letra_cifrada = string_toZZ(aux);
            string_letra_cifrada = to_string(potencia(letra_cifrada, e, N));
            if(string_letra_cifrada.length() < digitos_N)
            {
                for(int i = 0; i < (digitos_N - string_letra_cifrada.length()); i++)
                {
                    mensaje_cifrado += "0";
                }
                mensaje_cifrado += string_letra_cifrada;
            }
            else
                mensaje_cifrado += string_letra_cifrada;
            aux = "";
            string_letra_cifrada = "";
            count = 0;
        }
    }
    return mensaje_cifrado + firma_cifrada;
}
string RSA::descifrar(string mensaje, int tam_firma, ZZ e_firma, ZZ N_firma)
{
    string mensaje_descifrado, mensaje_temporal;
    int digitos_alfabeto = obtener_digitos(to_ZZ((alfabeto.length() - 1)));
    int digitos_N = obtener_digitos(N);
    ZZ letra_descifrada;
    string aux;
    int count = 0;
    for(int i = 0; i < (mensaje.size() - tam_firma); i++)
    {
        aux += mensaje[i];
        count += 1;
        if(count == digitos_N)
        {
            letra_descifrada = resto_chino(string_toZZ(aux));
            if(obtener_digitos(letra_descifrada) < (digitos_N - 1))
            {
                for(int i = 0; i < digitos_N - obtener_digitos(letra_descifrada) - 1; i++)
                {
                    mensaje_temporal += "0";
                }
                mensaje_temporal += to_string(letra_descifrada);
            }
            else
                mensaje_temporal += to_string(letra_descifrada);
            aux = "";
            count = 0;
            letra_descifrada = 0;
        }
    }
    ZZ letra_reemplazada;
    for(int i = 0; i < mensaje_temporal.size(); i++)
    {
        aux += mensaje_temporal[i];
        count += 1;
        if(count == digitos_alfabeto)
        {
            letra_reemplazada = string_toZZ(aux);
            mensaje_descifrado += alfabeto[to_int(letra_reemplazada)];
            count = 0;
            aux = "";
        }
    }

    aux = "";
    mensaje_temporal = "";
    count = 0;
    string mensaje_descifrado_firma;
    ZZ letra_descifrada_firma;
    int digitos_N_firma = to_string(N_firma).length();
    for(int i = (mensaje.size() - tam_firma); i < mensaje.size(); i++)
    {
        aux += mensaje_descifrado[i];
        count += 1;
        if(count == digitos_N_firma)
        {
            letra_descifrada_firma = potencia(string_toZZ(aux), e_firma, N_firma);
            if(obtener_digitos(letra_descifrada_firma) < (digitos_N_firma - 1))
            {
                for(int i = 0; i < digitos_N_firma - obtener_digitos(letra_descifrada_firma) - 1; i++)
                {
                    mensaje_temporal += "0";
                }
                mensaje_temporal += to_string(letra_descifrada_firma);
            }
            else
                mensaje_temporal += to_string(letra_descifrada_firma);
            aux = "";
            count = 0;
            letra_descifrada_firma = 0;
        }
    }
    cout << endl;
    cout << endl;
    cout << mensaje_temporal << endl;
    aux = "";
    count = 0;
    ZZ letra_reemplazada_firma;
    for(int i = 0; i < mensaje_temporal.size(); i++)
    {
        aux += mensaje_temporal[i];
        count += 1;
        if(count == digitos_alfabeto)
        {
            letra_reemplazada_firma = string_toZZ(aux);
            mensaje_descifrado_firma += alfabeto[to_int(letra_reemplazada_firma)];
            count = 0;
            aux = "";
        }
    }
    return mensaje_descifrado;
}



