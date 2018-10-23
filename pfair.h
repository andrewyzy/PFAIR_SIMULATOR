#ifndef PFAIR_H
#define PFAIR_H

#include <QList>
#include <QDebug>
#include "job.h"
#include <math.h>
#include <QColor>
#include "processador_escalonamento.h"

#include <QtAlgorithms>   // for qSort()

class PFair
{
public:
    PFair();
    void calcular_simulacao(QList<Simulacoes> simulacoes, int sim_number);
    void criar_processadores(int processadores);
    QList<Processador_Escalonamento *> processadores;
    int duracao;
    bool possivel_escalonar;
    bool tentar_evitar_migracao;
    QList<Job *> task_list;



private:
    void criar_janelas(QList<Simulacoes> simulacoes, int sim_number);
    QList<Janela> ja_escalonados;

    int indice_processador;
    bool evitar_migracao;


    QList<QList<Task>> tarefas; //TENHO que refazer essa Lista pra Lista: solucao = criar classe tarefa

    QList<Janela> verificar_primeiro_caso(int tempo);
    QList<Janela> falta_escalonar(QList<Janela> &falta_escalonar_janelas, int tempo, QList<Janela> &ja_executado,
                                  QList<Janela> &janelas_que_podem_executar);


    void escalonar_restante(QList<Janela> &janelas_que_podem_executar,QList<Janela> &falta_escalonar_janelas, int tempo, QList<Janela> &ja_executado);

    QList<Janela> verificar_primeiro_caso_new(QList<Janela> janelas, int tempo);

    void escalonar();
    void escalonar_old();
    QList<Janela> retornar_janelas_que_podem_executar(int tempo);
    int tempo_execucao;
    int numero_processadores;
    int processador_atual;
    void povoar_processadores();
    QList<Janela> menores_pseudo_deadline(QList<Janela> janelas, int tempo);
    void adicionar_tarefa_processador(Janela janela, int &indice_processador,
                                      QList<Janela> &janelas_que_podem_executar, QList<Janela> &ja_executado);
   // QList<Janela> escalonar_maior_b_ti(QList<Janela> menor_deadline, int &indice_processador, QList<Janela> &ja_executado);

    bool verificar_ja_foi_executado(Janela janela, int tempo);

    void add_secundario(Janela janela, int &indice_processador, QList<Janela> &ja_executado);

    void resolver_deadlines(int tempo, QList<Janela> &ja_executado, QList<Janela> Janelas, QList<Janela> &janelas_que_podem_executar);

    void avancar_tempo(int tempo);
    QList<Janela> maiores_b_ti(QList<Janela> menor_deadline);
    QList<Janela> menores_pseudo_deadline_new(QList<Janela> janelas, int tempo);



};

#endif // PFAIR_H
