#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "renderarea.h"
#include "gantt.h"
#include <QDebug>
#include <QVBoxLayout>
#include <Qt>
#include <QFileDialog>
#include <QScrollBar>
#include <QFile>
#include <QList>
#include "job.h"
#include "pfair.h"
#include <iostream>
#include <ctime>
#include <math.h>
#include <QDebug>

namespace Ui {
class MainWindow;
}

struct Prempcoes
{
    int tarefa;
    int numero_preempcoes;
    int migracoes;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    RenderArea *renderArea;
    Gantt *grafico;
    QString arquivo;
    QList<Simulacoes> simulacoes;
    void criar_estruturas();
    void ler_simulacao(int simulacao);
    PFair *pfair;
        bool ok;
     void   preempcao();
     void criar_lista_tarefas();
     int preempcao_deadline(int tarefa, int inicio, int deadline);
     int migracao_deadline(int tarefa, int inicio, int deadline);




public slots:
    void modificou_simulacao(int index);

private slots:
    void gerar_grafico();
    void add_tarefa();
    void abrir_arquivo();




};

#endif // MAINWINDOW_H
