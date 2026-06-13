#ifndef DECISAO_H
#define DECISAO_H

#include "execucao.h"
#include "metricas.h"

const char* decidir_algoritmo(PerfilEntrada p);
MetricasExecucao executar_com_inteligencia(const char* nome, int* arr, int n);

#endif