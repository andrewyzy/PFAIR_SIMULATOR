#ifndef JOB_H
#define JOB_H

#include <QList>
#include <QColor>


struct Janela
{
    int tarefa;
    int i;
    double inicio, fim;
    bool foi_executado;
    bool b;
    int pseudo_chegada, pseudo_deadline;
    bool invalido;
};

struct Task
{
    long long int inicio;
    double tempo_execucao;
    long long int deadline;
    QList<Janela> janelas;
    QColor cor;
};

struct Simulacoes
{
    int tempo_execucao;
    int numero_simulacao;
    int quantidade_processadores;
    int quantidade_tarefas;
    QList<Task> jobs;
};

class Job
{
public:
    Job();

    int inicio;
    double tempo_execucao;
    int deadline;
    QList<Janela> janelas;
    QColor cor;
    int tarefa;
};

#endif // JOB_H
