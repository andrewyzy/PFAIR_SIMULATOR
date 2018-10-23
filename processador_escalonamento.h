#ifndef PROCESSADOR_ESCALONAMENTO_H
#define PROCESSADOR_ESCALONAMENTO_H
#include <QColor>
#include <QRect>

struct Escalonamento
{
    QColor cor;
    QString nome;
    QRect rect;
    int tarefa, janela;
    int tempo;
    int processador;
    bool foi_executado;
    double deadline;
};

class Processador_Escalonamento
{
public:
    Processador_Escalonamento();
    QList<Escalonamento> escalonados;
    int tempo;
    Escalonamento ultimo;
    int id;
};

#endif // PROCESSADOR_ESCALONAMENTO_H
