#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    srand(static_cast<unsigned int>(time(0)));

    ok = false;
    QProcess::execute("clear");


    ui->setupUi(this);
    ui->desenhar->setBackgroundRole(QPalette::Base);
    ui->desenhar->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->desenhar->setScaledContents(true);



    connect(ui->simulacao_numero, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index){modificou_simulacao(index); });

    connect(ui->gerar_grafico_gantt, SIGNAL(pressed()), this, SLOT(gerar_grafico()));
    //connect(ui->add_tarefa, SIGNAL(pressed()), this, SLOT(add_tarefa()));
    connect(ui->abrir_arquivos, SIGNAL(pressed()), this, SLOT(abrir_arquivo()));

    QVBoxLayout *mainLayout = new QVBoxLayout();

    renderArea = new RenderArea;



    //GAMBIARRA pra funcionar o scroll
    QString linha;
    for(int i = 0; i<1000; ++i)
        linha.append("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
                     "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
                     "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
                     "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\n");
    QLabel *lab = new QLabel(linha);
    mainLayout->addWidget(lab);


    renderArea->setLayout(mainLayout);

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setWidget(renderArea);

    // ui->groupBox->hide();
    //ui->scrollArea_2->hide();



    // ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    // ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);



    /*  QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(renderArea);

    ui->scrollArea->setLayout(layout);*/



    // ui->scrollArea->setWidgetResizable(true);


    grafico = NULL;

    //arquivo = QString("/home/andre/simulador/uni4x8");
    arquivo = QString("/home/andre/simulador/teste");
    //qDebug() << renderArea->size();




}

/*void MainWindow::preempcao()
{
    //processadores.at(indice_processador)->escalonados
}*/
void MainWindow::modificou_simulacao(int index)
{
    //qDebug() << "Simulacao numero" << index;



    if(ok)
    {
        if(simulacoes.size() == 0 )
        {
            criar_estruturas();

        }

        renderArea->escalonamento.clear();
        renderArea->limpar_tudo();
        renderArea->limpar = true;
        renderArea->tarefas.clear();


        renderArea->duracao = ui->duracao->text().toInt();
        renderArea->criar_processadores(simulacoes.at(ui->simulacao_numero->currentIndex()).quantidade_processadores);

        ler_simulacao(ui->simulacao_numero->currentIndex());

        pfair = new PFair;
        pfair->tentar_evitar_migracao = ui->evitar_migracao->isChecked();

        pfair->duracao = ui->duracao->text().toInt();
        pfair->criar_processadores(simulacoes.at(index).quantidade_processadores);
        pfair->calcular_simulacao(simulacoes, index);

        //renderArea->adicionar_tarefa_processador(pfair->processadores.at(0)->escalonados);


        if(pfair->possivel_escalonar)
        {
            for(int i = 0; i < pfair->processadores.size(); ++i)
            {
                renderArea->adicionar_tarefa_processador(pfair->processadores.at(i)->escalonados);
            }
            renderArea->update();

            ui->scrollArea->update();

        }
        else
        {
            ui->mensagem->setText("Não é possivel escalonar");
        }



        renderArea->nao_fazer_update = false;

        //preempcao();
        criar_lista_tarefas();


    }


}

void MainWindow::criar_estruturas()
{
    simulacoes.clear();


    QFile file(arquivo);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    int cont = 0;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if(line.at(0) != '#') //vamos ignorar os comentários
        {
            QString palavra;
            QList<QString> variaveis;
            for(int i = 0; i < line.size(); i++)
            {
                if(line.at(i) != ' ')
                    palavra.append(line.at(i));

                if((line.at(i) == ' ') && (line.at(i-1) != ' '))
                {
                    variaveis.append(palavra);
                    palavra.clear();
                }
                if((line.at(i) == ';') && (line.at(i-1) != ';'))
                {
                    variaveis.append(palavra);
                    palavra.clear();
                }
            }

            Simulacoes sim;
            sim.numero_simulacao = variaveis.at(0).toInt();
            sim.quantidade_processadores = variaveis.at(1).toInt();
            sim.quantidade_tarefas = variaveis.at(2).toInt();

            if(ui->multiplicar->isChecked())
                sim.tempo_execucao = ui->duracao->text().toInt();
            else
                sim.tempo_execucao = ui->duracao->text().toInt();


            QList<Task> lista_tarefas;

            for(int i = 4; i < sim.quantidade_tarefas+4; ++i)
            {
                Task task;

                QString job = variaveis.at(i);
                job.remove(QChar('('), Qt::CaseInsensitive);
                job.remove(QChar(')'), Qt::CaseInsensitive);
                QString palavra;
                int cont = 0;

                for(int j = 0; j < job.size(); ++j)
                {
                    if(job.at(j) != ',')
                        palavra.append(job.at(j));

                    if((job.at(j) == ';') && (job.at(j-1) != ',') && (cont == 2))
                    {
                        palavra.remove(QChar(';'), Qt::CaseInsensitive);

                        if(ui->multiplicar->isChecked())
                            task.deadline = palavra.toInt()*10;
                        else
                            task.deadline = palavra.toInt();

                        palavra.clear();
                        cont++;
                    }

                    if((job.at(j) == ',') && (job.at(j-1) != ',') && (cont == 1))
                    {
                        task.tempo_execucao = palavra.toDouble();


                        if(ui->numeros_inteiros->isChecked())
                        {
                            double resultado = task.tempo_execucao - floor(task.tempo_execucao);
                            //

                            //if(resultado < 0.6)
                            task.tempo_execucao = floor(task.tempo_execucao);
                        }
                        if(ui->multiplicar->isChecked())
                        {
                            task.tempo_execucao = floor(task.tempo_execucao*10);
                        }



                        //task.tempo_execucao = floor(task.tempo_execucao);

                        palavra.clear();
                        cont++;
                    }

                    if((job.at(j) == ',') && (job.at(j-1) != ',') && (cont == 0))
                    {
                        if(ui->multiplicar->isChecked())
                        {
                            task.inicio = palavra.toInt()*10;

                        }
                        else
                            task.inicio = palavra.toInt();

                        palavra.clear();
                        cont++;
                    }
                }

                task.cor = QColor( rand()&155 +100, rand()&155 +100, rand()&155 +100, 100);
                lista_tarefas.append(task);
            }
            sim.jobs = lista_tarefas;
            ui->simulacao_numero->insertItem(simulacoes.size(),QString("%1").arg(simulacoes.size()));
            simulacoes.append(sim);
        }
        cont++;
    }

}

void MainWindow::ler_simulacao(int simulacao)
{
    simulacoes.at(simulacao);

    if(renderArea != NULL)
    {
        for(int i = 0; i < simulacoes.at(simulacao).jobs.size(); ++i)
        {
            renderArea->adicionar_tarefa(simulacoes.at(simulacao).jobs.at(i).tempo_execucao,
                                         simulacoes.at(simulacao).jobs.at(i).deadline,
                                         simulacoes.at(simulacao).jobs.at(i).inicio,
                                         simulacoes.at(simulacao).jobs.at(i).cor);

            // qDebug() << simulacoes.at(simulacao).jobs.at(i).inicio << simulacoes.at(simulacao).jobs.at(i).deadline;

            renderArea->update();

        }

    }

}

void MainWindow::abrir_arquivo()
{

    ok = false;

    arquivo = QString(QFileDialog::getOpenFileName(this, tr("Abra o arquivo de tarefas"), QDir::currentPath()));
    renderArea->limpar_tudo();

    renderArea->limpar = true;
    renderArea->tarefas.clear();

    criar_estruturas();

    renderArea->duracao = ui->duracao->text().toInt();
    renderArea->criar_processadores(simulacoes.at(0).quantidade_processadores);

    ler_simulacao(0);
    pfair = new PFair;
    pfair->tentar_evitar_migracao = ui->evitar_migracao->isChecked();
    pfair->duracao = ui->duracao->text().toInt();
    pfair->criar_processadores(simulacoes.at(0).quantidade_processadores);
    pfair->calcular_simulacao(simulacoes, 0);

    //renderArea->adicionar_tarefa_processador(pfair->processadores.at(0)->escalonados);


    for(int i = 0; i < pfair->processadores.size(); ++i)
    {
        renderArea->adicionar_tarefa_processador(pfair->processadores.at(i)->escalonados);
    }




    renderArea->update();


    ui->scrollArea->update();

    renderArea->nao_fazer_update = false;

    ok = true;


    criar_lista_tarefas();

    // ui->total->append(QString("Size dos escalonados = %1").arg( pfair->processadores.at(0)->escalonados.size()));

}

void MainWindow::gerar_grafico()
{
    //grafico = new Gantt;
    //grafico->numero_processadores = ui->spinBox->text().toInt();


    renderArea->limpar_tudo();
    renderArea->limpar = true;
    renderArea->tarefas.clear();

    criar_estruturas();



    renderArea->duracao = ui->duracao->text().toInt();
    renderArea->criar_processadores(simulacoes.at(0).quantidade_processadores);

    ler_simulacao(0);

    pfair = new PFair;
    pfair->tentar_evitar_migracao = ui->evitar_migracao->isChecked();

    pfair->duracao = ui->duracao->text().toInt();
    pfair->criar_processadores(simulacoes.at(0).quantidade_processadores);
    pfair->calcular_simulacao(simulacoes, 0);

    //renderArea->adicionar_tarefa_processador(pfair->processadores.at(0)->escalonados);


    if(pfair->possivel_escalonar)
    {
        for(int i = 0; i < pfair->processadores.size(); ++i)
        {
            renderArea->adicionar_tarefa_processador(pfair->processadores.at(i)->escalonados);
        }
        renderArea->update();

        ui->scrollArea->update();

    }
    else
    {
        ui->mensagem->setText("Não é possivel escalonar");
    }



    criar_lista_tarefas();

    renderArea->nao_fazer_update = false;

    ok = true;

    //preempcao();
}

void MainWindow::criar_lista_tarefas()
{
    //qDebug() << "Criando Tarefas";


    int tarefa = 0;
    QList<int> cont_preemcoes_tarefas;
    QList<int> cont_migracoes_tarefas;

    int cont = 0;
    int cont_migracao = 0;
    for(int i = 0; i < pfair->task_list.size(); ++i)
    {
        if(tarefa != pfair->task_list.at(i)->tarefa)
        {
            tarefa = pfair->task_list.at(i)->tarefa;
            cont_preemcoes_tarefas.append(cont);
            cont_migracoes_tarefas.append(cont_migracao);
            cont = 0;
            cont_migracao = 0;
        }
        cont = cont +  preempcao_deadline(pfair->task_list.at(i)->tarefa, pfair->task_list.at(i)->inicio ,  pfair->task_list.at(i)->deadline);
        cont_migracao = cont_migracao +  migracao_deadline(pfair->task_list.at(i)->tarefa, pfair->task_list.at(i)->inicio ,  pfair->task_list.at(i)->deadline);

        if(i == pfair->task_list.size() -1)
        {
            cont_preemcoes_tarefas.append(cont);
            cont_migracoes_tarefas.append(cont_migracao);
        }

    }
    ui->listWidget->clear();
    for (int i = 0; i < cont_preemcoes_tarefas.size(); ++i)
    {

        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(QString("Tarefa %1:  %2 preempções").arg(i).arg((cont_preemcoes_tarefas.at(i))));
        ui->listWidget->insertItem(ui->listWidget->count(), newItem);
    }
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(QString("\nMIGRAÇÕES"));
    ui->listWidget->insertItem(ui->listWidget->count(), newItem);
    for (int i = 0; i < cont_migracoes_tarefas.size(); ++i)
    {

        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(QString("Tarefa %1:  %2 migrações").arg(i).arg((cont_migracoes_tarefas.at(i))));
        ui->listWidget->insertItem(ui->listWidget->count(), newItem);
    }


    int cont_preempcoes = 0;

    int cont_migracoes = 0;

    for(int i = 0; i < pfair->task_list.size(); ++i)
    {
        cont_preempcoes = cont_preempcoes + preempcao_deadline(pfair->task_list.at(i)->tarefa, pfair->task_list.at(i)->inicio ,  pfair->task_list.at(i)->deadline);
        cont_migracoes = cont_migracoes + migracao_deadline(pfair->task_list.at(i)->tarefa, pfair->task_list.at(i)->inicio ,  pfair->task_list.at(i)->deadline);

    }

    //qDebug() << "Prempcoes TOTAL = " << cont_preempcoes;
    ui->preempcoes->setText(QString("Preempções total = %1").arg(cont_preempcoes));
    ui->migracoes->setText(QString("Migrações total = %1").arg(cont_migracoes));


    QListWidgetItem *newItem2 = new QListWidgetItem;
    newItem2->setText(QString("\nPreemção: Média por JOB"));
    ui->listWidget->insertItem(ui->listWidget->count(), newItem2);

    int index = ui->simulacao_numero->currentIndex();
    if(index > 999)
        index = 0;

    if(!ui->multiplicar->isChecked())
    {
        for(int i = 0; i < simulacoes.at(index).jobs.size(); ++i)
        {
            double jobs = ceil(ui->duracao->text().toInt()/simulacoes.at(index).jobs.at(i).deadline);

            double cont = cont_preemcoes_tarefas.at(i);

            double result = cont/jobs;

            qDebug() << "Preempcao  " << cont << "Tamanho Jobs" << jobs;


            QListWidgetItem *newItem = new QListWidgetItem;
            newItem->setText(QString("Tarefa %1:  Média %2").arg(i).arg(result));
            ui->listWidget->insertItem(ui->listWidget->count(), newItem);

        }

        QListWidgetItem *newItem3 = new QListWidgetItem;
        newItem3->setText(QString("\nMigração: Média por JOB"));
        ui->listWidget->insertItem(ui->listWidget->count(), newItem3);

        for(int i = 0; i < simulacoes.at(index).jobs.size(); ++i)
        {
            double jobs = ceil(ui->duracao->text().toInt()/simulacoes.at(index).jobs.at(i).deadline);
            qDebug() << "Tarefa " << i << "Size" << jobs;

            double cont = cont_migracoes_tarefas.at(i);

            double result = cont/jobs;

            QListWidgetItem *newItem = new QListWidgetItem;
            newItem->setText(QString("Tarefa %1:  Média %2").arg(i).arg(result));
            ui->listWidget->insertItem(ui->listWidget->count(), newItem);

        }
    }

}

int MainWindow::preempcao_deadline(int tarefa, int inicio, int deadline)
{
    QList<Escalonamento> escalonados;

    ////qDebug() << "Testando: Inicio: " << inicio << "Tarefa: " << tarefa<< "Deadline: " <<deadline;
    for(int x = 0; x < pfair->processadores.size(); ++x)
    {
        for(int i = 0; i < pfair->processadores.at(x)->escalonados.size(); ++i)
        {
            if(pfair->processadores.at(x)->escalonados.at(i).foi_executado == true)
            {
                if((pfair->processadores.at(x)->escalonados.at(i).tempo < deadline)
                        &&(pfair->processadores.at(x)->escalonados.at(i).tarefa == tarefa)
                        &&(pfair->processadores.at(x)->escalonados.at(i).tempo >= inicio))
                {
                    escalonados.append(pfair->processadores.at(x)->escalonados.at(i));
                }
            }
        }
    }

    for(int x = 0; x<escalonados.size(); ++x)
    {
        for(int i = 0; i < escalonados.size()-1; ++i)
        {
            if(escalonados.at(i+1).tempo < escalonados.at(i).tempo)
            {
                escalonados.swap(i, i+1);
            }
        }
    }


    int preempcoes_cont = 0;
    for(int i = 0; i < escalonados.size()-1; ++i)
    {
        int tempo_futuro = escalonados.at(i).tempo+1;

        if((tempo_futuro != escalonados.at(i+1).tempo) &&
                (escalonados.at(i).tempo != escalonados.at(i+1).tempo))
        {
            /* qDebug() << "PREMPCAO -> Atual T" << escalonados.at(i).tarefa <<
                escalonados.at(i).janela <<
                escalonados.at(i).deadline <<
                 "TEMPO" << escalonados.at(i).tempo
                 << " Processador:" <<  escalonados.at(i).processador;*/
            preempcoes_cont++;
        }
    }

    //qDebug() << "Numero preempcoes = " << preempcoes_cont;
    return preempcoes_cont;

}
int MainWindow::migracao_deadline(int tarefa, int inicio, int deadline)
{
    QList<Escalonamento> escalonados;

    //qDebug() << "MIGRACAO Testando: Inicio: " << inicio << "Tarefa: " << tarefa<< "Deadline: " <<deadline;
    for(int x = 0; x < pfair->processadores.size(); ++x)
    {
        for(int i = 0; i < pfair->processadores.at(x)->escalonados.size(); ++i)
        {
            if(pfair->processadores.at(x)->escalonados.at(i).foi_executado == true)
            {
                if((pfair->processadores.at(x)->escalonados.at(i).tempo < deadline)
                        &&(pfair->processadores.at(x)->escalonados.at(i).tarefa == tarefa)
                        &&(pfair->processadores.at(x)->escalonados.at(i).tempo >= inicio))
                {
                    escalonados.append(pfair->processadores.at(x)->escalonados.at(i));
                }
            }
        }
    }

    for(int x = 0; x<escalonados.size(); ++x)
    {
        for(int i = 0; i < escalonados.size()-1; ++i)
        {
            if(escalonados.at(i+1).tempo < escalonados.at(i).tempo)
            {
                escalonados.swap(i, i+1);
            }
        }
    }


    int migracoes_cont = 0;
    for(int i = 0; i < escalonados.size()-1; ++i)
    {
        if(escalonados.at(i).processador != escalonados.at(i+1).processador)
        {
            //qDebug() << "MIGRACAO -> Atual T" << escalonados.at(i).tarefa <<
            //escalonados.at(i).janela <<
            // escalonados.at(i).deadline <<
            //  "TEMPO" << escalonados.at(i).tempo
            //  << " Processador:" <<  escalonados.at(i).processador;
            migracoes_cont++;
        }

    }

    //qDebug() << "Numero Migracoes = " << migracoes_cont;
    return migracoes_cont;

}


void MainWindow::preempcao()
{


    QList<Job *> tarefas;

    for(int i = 0; i <                 pfair->task_list.size(); ++i)
    {
        if(pfair->task_list.at(i)->janelas.size() > 0)
            tarefas.append(pfair->task_list.at(i));
    }

    for(int x = 0; x < pfair->processadores.size(); ++x)
    {
        for(int i = 1; i < pfair->processadores.at(x)->escalonados.size(); ++i)
        {
            if(pfair->processadores.at(x)->escalonados.at(i).foi_executado == true)
            {

                //qDebug() << "Atual T" << pfair->processadores.at(x)->escalonados.at(i-1).tarefa <<
                //pfair->processadores.at(x)->escalonados.at(i-1).janela <<
                //pfair->processadores.at(x)->escalonados.at(i-1).deadline <<
                //   pfair->processadores.at(x)->escalonados.at(i-1).tempo;

                //qDebug() << "Proximo T" << pfair->processadores.at(x)->escalonados.at(i).tarefa <<
                // pfair->processadores.at(x)->escalonados.at(i).janela <<
                // pfair->processadores.at(x)->escalonados.at(i).deadline <<
                //    pfair->processadores.at(x)->escalonados.at(i).tempo;

                int tamanho_janela = 0;

                for(int j = 0; j < tarefas.size(); ++j)
                {
                    if(tarefas.at(j)->janelas.at(0).tarefa == pfair->processadores.at(x)->escalonados.at(i-1).tarefa )
                        tamanho_janela = tarefas.at(j)->janelas.size();
                }

                //qDebug() << "Tamnanho = " << tamanho_janela;

                if(pfair->processadores.at(x)->escalonados.at(i-1).janela < (tamanho_janela-1))
                {
                    if(pfair->processadores.at(x)->escalonados.at(i-1).tarefa !=  pfair->processadores.at(x)->escalonados.at(i).tarefa)
                    {
                        //qDebug() << "Prempcao T" << pfair->processadores.at(x)->escalonados.at(i-1).tarefa;
                    }
                }

            }
        }
        //qDebug() << "Pulando Processador";
    }


    //ui->total->append(

    //  QListWidgetItem *newItem = new QListWidgetItem;
    //newItem->setText(QString("Total de Preempções= %1").arg(cont));
    //ui->listWidget->insertItem(ui->listWidget->count(), newItem);


    // QListWidgetItem *newItem2 = new QListWidgetItem;
    //newItem2->setText(QString("Size dos escalonados = %1").arg( pfair->processadores.at(0)->escalonados.size()));
    //ui->listWidget->insertItem(ui->listWidget->count(), newItem2);
}

void MainWindow::add_tarefa()
{
    if(renderArea != NULL)
    {
        ler_simulacao(ui->simulacao_numero->currentIndex());
        renderArea->update();
    }
    else
    {
        gerar_grafico();
        add_tarefa();
    }

}
MainWindow::~MainWindow()
{
    delete ui;
}
