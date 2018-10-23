#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QList>
#include <QDebug>
#include "job.h"
#include <QSizePolicy>
#include <QPixmap>
#include <QFile>
#include <QSize>
#include <QDebug>
#include "pfair.h"

struct Processador
{
    int i;
    QString nome;
    QRect rect;
    //se precisar de algo, colocamos aqui.
};

struct Tarefa
{
    QString nome;
    QRect rect;
    int ci, ti, inicio;
    int x, y;
    QColor cor;

    //se precisar de algo, colocamos aqui.
};

//! [0]
class RenderArea : public QWidget
{
    Q_OBJECT

public:
    enum Shape { Line, Points, Polyline, Polygon, Rect, RoundedRect, Ellipse, Arc,
                 Chord, Pie, Path, Text, Pixmap };

    RenderArea(QWidget *parent = 0);
    bool limpar;

//    QSize minimumSizeHint() const override;
 //   QSize sizeHint() const override;

    int duracao, top, top_old;
    QList<Tarefa> tarefas;
    bool nao_fazer_update;
    QList<Escalonamento> escalonamento;



public slots:
    void setShape(Shape shape);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);
    void setTransformed(bool transformed);

    void adicionar_tarefa(double ci, int ti, int inicio, QColor cor);
    void criar_processadores(int numero_processadores);
    void adicionar_tarefa_processador(QList<Escalonamento> tarefas);
    void limpar_tudo();


protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Shape shape;
    QPen pen;
    QBrush brush;
    bool antialiased;
    bool transformed;
    QPainter *p;


    QList<Processador> processadores;

    void desenhar_processadores(QPainter *painter);
    void desenhar_tarefas(QPainter *painter);
    void desenhar_linha_tempo(QPainter *painter,  QRect rect, QString nome);
    void desenhar_escalonamento(QPainter *painter);

};
//! [0]

#endif // RENDERAREA_H
