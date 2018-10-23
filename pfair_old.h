#ifndef PFAIR_H
#define PFAIR_H

#include <QList>
#include <QDebug>
#include "job.h"
#include <math.h>
#include <QColor>
#include "processador_escalonamento.h"


class PFair_OLD
{
public:
    PFair_OLD();
    void calcular_simulacao(QList<Simulacoes> simulacoes);
    void criar_processadores(int processadores);
    QList<Processador_Escalonamento *> processadores;
    int duracao;


private:
    void criar_janelas(QList<Simulacoes> simulacoes);

    int indice_processador;
    bool evitar_migracao;


    QList<QList<Task>> tarefas; //TENHO que refazer essa Lista pra Lista: solucao = criar classe tarefa
    QList<Job *> task_list;

    QList<Janela> verificar_primeiro_caso(int tempo, QList<Janela> janelas);
    QList<Janela> falta_escalonar(QList<Janela> &falta_escalonar, int tempo, QList<Janela> &ja_executado);

    void escalonar_restante(QList<Janela> &janelas_que_podem_executar,QList<Janela> &falta_escalonar_janelas, int tempo, QList<Janela> &ja_executado);

    void escalonar();
    void escalonar_old();
    QList<Janela> retornar_janelas_que_podem_executar(int tempo);
    int tempo_execucao;
    int numero_processadores;
    int processador_atual;
    void povoar_processadores();
    QList<Janela> menores_pseudo_deadline(QList<Janela> janelas);
    void adicionar_tarefa_processador(Janela janela, int &indice_processador,
                                      QList<Janela> &janelas_que_podem_executar, QList<Janela> &ja_executado);
   // QList<Janela> escalonar_maior_b_ti(QList<Janela> menor_deadline, int &indice_processador, QList<Janela> &ja_executado);

    bool verificar_ja_foi_executado(Janela janela, int tempo);

    void add_secundario(Janela janela, int &indice_processador, QList<Janela> &ja_executado);

    void resolver_deadlines(int tempo, QList<Janela> janelas, QList<Janela> &falta_escalonar_janelas, QList<Janela> &ja_executado, QList<Janela> &janelas_que_podem_executar);

    void avancar_tempo(int tempo);


};

#endif // PFAIR_H
