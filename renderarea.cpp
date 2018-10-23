#include "renderarea.h"

#include <QPainter>


RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    //shape = Polygon;
    antialiased = true;
    transformed = false;
    limpar = false;

    top_old = 0;

    setBackgroundRole(QPalette::Base);

    nao_fazer_update = true;
    // setAutoFillBackground(true);
}

void RenderArea::limpar_tudo()
{
    QPainter *novo;
    p = novo;
    escalonamento.clear();
    processadores.clear();
}

void RenderArea:: desenhar_escalonamento(QPainter *painter)
{
    for(int i = 0; i < escalonamento.size(); ++i)
    {
        QPen penHText( QColor(0,0,0));
        painter->setPen(penHText);

        QPainterPath path;
        path.addRect(escalonamento.at(i).rect);

        painter->fillPath(path, tarefas.at(escalonamento.at(i).tarefa).cor);

        painter->drawPath(path);

        penHText.setColor(QColor("#000000"));
        painter->setPen(penHText);

        //Desenho do texto indicando a tarfa

        QFont font = painter->font() ;

        font.setPointSize(font.pointSize() - 2);

        /* set the modified font to the painter */
        painter->setFont(font);

        painter->drawText(escalonamento.at(i).tempo*30 +53,
                          escalonamento.at(i).rect.y() + 15,
                          QString("T%1").arg(escalonamento.at(i).tarefa));
        painter->drawText(escalonamento.at(i).tempo*30 +53,
                          escalonamento.at(i).rect.y() + 25,
                          QString("J%1").arg(escalonamento.at(i).janela));

        font.setPointSize(font.pointSize() +2);
        painter->setFont(font);

    }
}

void RenderArea::criar_processadores(int numero_processadores)
{

    processadores.clear();

    int posicao_vertical = 20;
    int duracao_total = duracao*30;

    for(int i = 0; i < numero_processadores; ++i)
    {
        Processador p;
        p.rect = QRect(50, posicao_vertical, duracao_total, 50);
        posicao_vertical = posicao_vertical + 70;

        p.nome = QString("P%1").arg(processadores.size());
        processadores.append(p);
    }

    update();
}


void RenderArea:: adicionar_tarefa(double ci, int ti, int inicio, QColor cor)
{
    //qDebug() << QString("Ci = %1 | Ti = %2").arg(ci).arg(ti);

    Tarefa t;
    if(tarefas.size() == 0)
    {
         top = processadores.at(processadores.size()-1).rect.top()+70;
         t.rect = QRect(inicio*30 +50, top, ci*30, 50);
         t.nome = QString("T%1").arg(tarefas.size());
         t.ci = ci;
         t.ti = ti;
         t.x = 50;
         t.inicio = inicio;
         t.cor = cor;
         tarefas.append(t);
    }
    else
    {
         top = tarefas.at(tarefas.size()-1).rect.top()+70;
         t.rect = QRect(inicio*30 +50, top, ci*30, 50);
         t.nome = QString("T%1").arg(tarefas.size());
         t.ci = ci;
         t.ti = ti;
         t.x = 50;
         t.inicio = inicio;
         t.cor = cor;
         tarefas.append(t);
    }
}

void RenderArea:: adicionar_tarefa_processador(QList<Escalonamento> tarefas)
{
    if(tarefas.size() == 0)
        return;

    for(int i = 0; i < tarefas.size(); ++i)
    {
        if(tarefas.at(i).foi_executado == true)
        {
            Escalonamento t;

            top = processadores.at(tarefas.at(i).processador).rect.top() +14;
            t.rect = QRect(tarefas.at(i).tempo*30 +50, top, 25, 35);
            t.nome = QString("T%1,%2").arg(tarefas.at(i).tarefa).arg(tarefas.at(i).janela);
            t.janela = tarefas.at(i).janela;
            t.processador = tarefas.at(i).processador;
            t.tarefa = tarefas.at(i).tarefa;
            t.tempo = tarefas.at(i).tempo;
            t.cor = tarefas.at(tarefas.at(i).tarefa).cor;

            escalonamento.append(t);
        }

    }


}

void RenderArea:: desenhar_linha_tempo(QPainter *painter, QRect rect, QString nome)
{
    painter->drawRect(rect); //Desenho do retangulo principal

    //Desenho do texto indicando o processador
    painter->drawText(rect.x() -45,
                      rect.y() + 30,
                      QString("%1").arg(nome));

    int cont = 0;
    int linha = 0;

    for(int x = 0; x <= duracao; ++x)
    {
       /* painter->drawLine(rect.x()+(cont*30), rect.y()+45,
                       rect.x()+(cont*30), rect.y() +50);

        painter->drawText(rect.x()+(cont*30)-3, rect.y()+63,
                          QString("%1").arg(cont)); */

        if(linha == 5)
        {
            /*
            QPen penHText(QColor("#00e0fc"));
            painter->setPen(penHText);

            painter->drawLine(rect.x()+(cont*30), rect.y()+45,
                              rect.x()+(cont*30), rect.y() +25);

            penHText.setColor(QColor("#000000"));
            painter->setPen(penHText);
            */

            linha = 0;
        }

        cont++;
        linha++;
    }

}

void RenderArea:: desenhar_processadores(QPainter *painter)
{

    painter->setBrush(Qt::white);
    painter->drawRect(0,0,10000,10000);
    painter->setBrush(Qt::NoBrush);

    // Criação dos processadores (linha temporal)
    for(int i = 0; i < processadores.size(); ++i)
    {
        painter->drawRect(processadores.at(i).rect); //Desenho do retangulo principal

        //Desenho do texto indicando o processador
        painter->drawText(processadores.at(i).rect.x() -45,
                          processadores.at(i).rect.y() + 30,
                          QString("%1").arg(processadores.at(i).nome));

        int cont = 0;
        int linha = 0;

        for(int x = 0; x <= duracao; ++x)
        {
            painter->drawLine(processadores.at(i).rect.x()+(cont*30), processadores.at(i).rect.y()+45,
                              processadores.at(i).rect.x()+(cont*30), processadores.at(i).rect.y() +50);

            painter->drawText(processadores.at(i).rect.x()+(cont*30)-3, processadores.at(i).rect.y()+63,
                              QString("%1").arg(cont));

            if(linha == 5)
            {

                QPen penHText(QColor("#00e0fc"));
                painter->setPen(penHText);

                painter->drawLine(processadores.at(i).rect.x()+(cont*30), processadores.at(i).rect.y()+45,
                                  processadores.at(i).rect.x()+(cont*30), processadores.at(i).rect.y() +35);

                penHText.setColor(QColor("#000000"));
                painter->setPen(penHText);


                linha = 0;
            }

            cont++;
            linha++;
        }
    }
}


void RenderArea:: desenhar_tarefas(QPainter *painter)
{
    for(int i = 0; i < tarefas.size(); ++i)
    {
        QPen penHText(QColor(0,0,0));
        painter->setPen(penHText);

        QPainterPath path;
        path.addRect(tarefas.at(i).rect);


        painter->fillPath(path, tarefas.at(i).cor);

        painter->drawPath(path);

        penHText.setColor(QColor("#000000"));
        painter->setPen(penHText);

        //Desenho do texto indicando a tarfa
        painter->drawText(10,
                          tarefas.at(i).rect.y() + 30,
                          QString("%1").arg(tarefas.at(i).nome));

        painter->drawText(tarefas.at(i).inicio*30 +57,
                          tarefas.at(i).rect.y() + 30,
                          QString("%1").arg(tarefas.at(i).nome));



        painter->setPen(penHText);

        painter->drawLine(tarefas.at(i).ti*30 +50, tarefas.at(i).rect.y()+50,
                          tarefas.at(i).ti*30 +50, tarefas.at(i).rect.y() +25);

        QPoint point(tarefas.at(i).ti*30 +50, tarefas.at(i).rect.y()+20);

        painter->setBrush(Qt::red);
        painter->drawEllipse(point, 5, 5);
        painter->setBrush(Qt::NoBrush);

        penHText.setColor(QColor("#000000"));
        painter->setPen(penHText);



        int cont = 0;
        int linha = 0;


        int duracao_total = duracao*30;




        QRect rect;
        top = tarefas.at(i).rect.top();
        rect = QRect(50, top, duracao_total, 50);


        desenhar_linha_tempo(painter, rect, "");



        for(int x = 0; x <= duracao; ++x)
        {

            painter->drawLine(tarefas.at(i).x+(cont*30), tarefas.at(i).rect.y()+45,
                              tarefas.at(i).x+(cont*30), tarefas.at(i).rect.y() +50);

            painter->drawText(tarefas.at(i).x+(cont*30)-3, tarefas.at(i).rect.y()+63,
                              QString("%1").arg(cont));


            if((x != 0) && (tarefas.at(i).ti*30*x < duracao_total))
            {
                QPen penHText(QColor(0,0,0));
                painter->setPen(penHText);

                QPainterPath path;

                QRect n = QRect(tarefas.at(i).ti*30*x +50, tarefas.at(i).rect.top(),
                                tarefas.at(i).rect.width(), 50);

                path.addRect(n);

              //  qDebug() << "Tamanho = " <<  tarefas.at(i).rect.width()*x << "outro tamanho" << tarefas.at(i).ti*30*x;

                painter->fillPath(path,  tarefas.at(i).cor);

                painter->drawPath(path);

                penHText.setColor(QColor("#000000"));
                painter->setPen(penHText);


                painter->drawText(tarefas.at(i).ti*30*x  +57,
                                  tarefas.at(i).rect.y() + 30,
                                  QString("%1").arg(tarefas.at(i).nome));



                painter->setPen(penHText);

                painter->drawLine(tarefas.at(i).ti*30*x +50, tarefas.at(i).rect.y()+50,
                                  tarefas.at(i).ti*30*x +50, tarefas.at(i).rect.y() +25);


                QPoint point(tarefas.at(i).ti*30*x +50, tarefas.at(i).rect.y()+20);

                painter->setBrush(Qt::red);
                painter->drawEllipse(point, 5, 5);
                painter->setBrush(Qt::NoBrush);

                penHText.setColor(QColor("#000000"));
                painter->setPen(penHText);

            }

            if(linha == 5)
            {

                QPen penHText(QColor("#80ff00"));
                painter->setPen(penHText);

                painter->drawLine(tarefas.at(i).x+(cont*30), tarefas.at(i).rect.y()+45,
                                  tarefas.at(i).x+(cont*30), tarefas.at(i).rect.y() +35);

                penHText.setColor(QColor("#000000"));
                painter->setPen(penHText);

                linha = 0;
            }

            cont++;
            linha++;
        }
    }
}

void RenderArea::setShape(Shape shape)
{
    this->shape = shape;
    update();
}

void RenderArea::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void RenderArea::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void RenderArea::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void RenderArea::setTransformed(bool transformed)
{
    this->transformed = transformed;
    update();
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
  //  if(nao_fazer_update)
   // {
        QPainter painter(this);

      //  qDebug() << "OPA";

        p = &painter;
        painter.setPen(pen);
        painter.setBrush(brush);
        if (antialiased)
            painter.setRenderHint(QPainter::Antialiasing, true);

        desenhar_processadores(p);

        desenhar_tarefas(p);

        desenhar_escalonamento(p);
  //  }
}

