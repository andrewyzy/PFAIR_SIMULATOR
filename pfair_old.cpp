#include "pfair_old.h".h"

PFair_OLD::PFair_OLD()
{
    processador_atual = 0;

    indice_processador = 0;
    evitar_migracao = false;
}

void PFair_OLD::criar_processadores(int processadores)
{
    numero_processadores = processadores;
}

void PFair_OLD::calcular_simulacao(QList<Simulacoes> simulacoes)
{
    criar_janelas(simulacoes);
    povoar_processadores();
    escalonar();
}

void PFair_OLD::povoar_processadores()
{
    for(int i = 0; i < numero_processadores; ++i)
    {
        QList<Escalonamento> escalonados;
        Processador_Escalonamento *p = new Processador_Escalonamento;
        p->tempo = 0;

        for(int j = 0; j < duracao; ++j)
        {
            Escalonamento esc;
            esc.processador = i;
            esc.tempo = j;
            esc.tarefa = NULL;
            esc.foi_executado = false;
            escalonados.append(esc);
        }
        p->id = i;
        p->escalonados = escalonados;

        processadores.append(p);
    }
}

QList<Janela> PFair_OLD::menores_pseudo_deadline(QList<Janela> janelas)
{
    int menor_pseudo_deadline = 10000000000;
    int pseudo_deadline_anterior = 0;

    for(int i = 0; i < janelas.size(); ++i)
    {
        pseudo_deadline_anterior = janelas.at(i).pseudo_deadline;
        if(janelas.at(i).pseudo_deadline < menor_pseudo_deadline)
            menor_pseudo_deadline = janelas.at(i).pseudo_deadline;
    }

    QList<Janela> menores_deadlines;
    for(int i = 0; i < janelas.size(); ++i)
    {
        if(janelas.at(i).pseudo_deadline == menor_pseudo_deadline)
        {
            //  qDebug() << "Menor pseudo deadline T" <<janelas.at(i).tarefa <<"," << janelas.at(i).i << "Pseudo:" << janelas.at(i).pseudo_deadline;
            menores_deadlines.append(janelas.at(i));
        }
    }

    return menores_deadlines;

}

bool PFair_OLD::verificar_ja_foi_executado(Janela janela, int tempo)
{
    int inicio = -1;
    int deadline = -1;
    for(int i = 0; i < task_list.size(); ++i)
    {
        for(int j = 0; j < task_list.at(i)->janelas.size(); ++j)
        {
            if(task_list.at(i)->janelas.at(j).tarefa == janela.tarefa)
            {
               inicio = task_list.at(i)->inicio;
               deadline = task_list.at(i)->deadline;

            }
        }
    }
    int fator = ceil(tempo/deadline) +1;
    int fator2 = ceil(tempo/deadline);

    inicio = deadline*fator2;
    deadline = deadline*fator;

   /* for(int i = 0; i < numero_processadores; ++i)
    {
        for(int j = inicio; j < deadline; ++j)
        {
            if((processadores.at(i)->escalonados.at(j).janela == janela.i) &&
              (processadores.at(i)->escalonados.at(j).tarefa == janela.tarefa))
            {
                qDebug() << "[BUG]: tempo " << processadores.at(i)->escalonados.at(j).tempo;;
                return true;
            }
        }

    } BOM -> funcionando */

    for(int i = 0; i < numero_processadores; ++i)
     {
         for(int j = 0; j < processadores.at(i)->escalonados.size(); ++j)
         {
             if((processadores.at(i)->escalonados.at(j).janela == janela.i) &&
               (processadores.at(i)->escalonados.at(j).tarefa == janela.tarefa))
             {
                 qDebug() << "[BUG]: tempo " << processadores.at(i)->escalonados.at(j).tempo;;
                 return true;
             }
         }

     }
    return false;
}
void PFair_OLD::adicionar_tarefa_processador(Janela janela, int &indice_processador,  QList<Janela> &janelas_que_podem_executar,  QList<Janela> &ja_executado)
{

    if(indice_processador > processadores.size())
        indice_processador = 0;

    if(indice_processador == processadores.size())
        indice_processador = 0;

   if( verificar_ja_foi_executado(janela,  processadores.at(indice_processador)->tempo))
   {
       qDebug() << "Já foi executado";
       return;
   }


    bool achei = false;
    for(int i = 0; i < task_list.size(); ++i)
    {
        for(int j = 0; j < task_list.at(i)->janelas.size(); ++j)
        {
            if(task_list.at(i)->janelas.at(j).tarefa == janela.tarefa)
            {
                //qDebug() << "A tarefa termina em " << task_list.at(i)->tempo_execucao;

                if(janela.i >= task_list.at(i)->tempo_execucao)
                {
                    qDebug() << "[adicionar_tarefa_processador]: NAO VAMOS ADICIONAR ESSA TAREFA";
                    return;
                }
                achei = true;
                break;
            }
        }
        if(achei)
            break;
    }

    qDebug() << "[ADD] ----->  Adicionando T" << janela.tarefa << "," << janela.i << " no processador " << indice_processador
            << "-> Tempo = " << processadores.at(indice_processador)->tempo;

    ja_executado.append(janela);

    Escalonamento esc;
    esc.tempo = processadores.at(indice_processador)->tempo;
    esc.processador = indice_processador;
    esc.janela = janela.i;
    esc.tarefa = janela.tarefa;
    esc.foi_executado = true;

    processadores.at(indice_processador)->escalonados.replace(processadores.at(indice_processador)->tempo, esc);
    processadores.at(indice_processador)->ultimo = esc;
    processadores.at(indice_processador)->tempo++;


    for(int j =0 ; j < janelas_que_podem_executar.size(); ++j)
    {
        if((janelas_que_podem_executar.at(j).tarefa == janela.tarefa) &&
                (janelas_que_podem_executar.at(j).i == janela.i))
        {
            qDebug() << "[ATENCAO] [adicionar_tarefa_processador] -->> REMOVER T" << janelas_que_podem_executar.at(j).tarefa << "," <<
                        janelas_que_podem_executar.at(j).i;
            janelas_que_podem_executar.removeAt(j);

        }
    }

    if(indice_processador < processadores.size())
        indice_processador++;
}


void PFair_OLD::add_secundario(Janela janela, int &indice_processador, QList<Janela> &ja_executado)
{

    if(indice_processador > processadores.size())
        indice_processador = 0;

    if(indice_processador == processadores.size())
        indice_processador = 0;

    if (verificar_ja_foi_executado(janela,  processadores.at(indice_processador)->tempo))
    {
        qDebug() << "Já foi executado";
        return;
    }

    ja_executado.append(janela);


    qDebug() << "[ADD SECUNDARIO] ----->  Adicionando T" << janela.tarefa << "," << janela.i << " no processador " << indice_processador
             << "-> Tempo = " << processadores.at(indice_processador)->tempo;
    Escalonamento esc;
    esc.tempo = processadores.at(indice_processador)->tempo;
    esc.processador = indice_processador;
    esc.janela = janela.i;
    esc.tarefa = janela.tarefa;
    esc.foi_executado = true;

    processadores.at(indice_processador)->escalonados.replace(processadores.at(indice_processador)->tempo, esc);

    // processadores.at(processador)->escalonados.append(esc);
    processadores.at(indice_processador)->ultimo = esc;
    processadores.at(indice_processador)->tempo++;

    if(indice_processador < processadores.size())
        indice_processador++;
}


QList<Janela> PFair_OLD::verificar_primeiro_caso(int tempo, QList<Janela> janelsas)
{

    QList<Janela> janelas = retornar_janelas_que_podem_executar(tempo);
    QList<Janela> menor_deadline = menores_pseudo_deadline(janelas);

    if(menor_deadline.size() == 1)
    {
        qDebug() << "[LOG] [verificar_primeiro_caso] -> Achei o menor deadline [OK]";
        return menor_deadline;
    }
    else
    {
       // qDebug() << "[LOG] [verificar_primeiro_caso] -> Existem mais JOBS com pseudo deadline iguais. ";
        return menor_deadline;
    }
}
QList<Janela> PFair_OLD::falta_escalonar(QList<Janela> &falta_escalonar, int tempo, QList<Janela> &ja_executado)
{

   //if(falta_escalonar.size() == 0)
    //    return;


    for(int i = 0; i < falta_escalonar.size(); ++i)
    {
        qDebug() << "\t[LOG] [falta escalonar] Falta escalonar T" << falta_escalonar.at(i).tarefa << "," <<
                    falta_escalonar.at(i).i;
    }

    QList<Janela> janelas = menores_pseudo_deadline(falta_escalonar);


    if(janelas.size() == 1)
    {
        qDebug() << "\t[LOG] [falta_escalonar] : Escalonar PRIMEIRO CASO";
        add_secundario(janelas.at(0), indice_processador, ja_executado);

        for(int j =0 ; j < falta_escalonar.size(); ++j)
        {
            if((falta_escalonar.at(j).tarefa == janelas.at(0).tarefa) &&
                    (falta_escalonar.at(j).i == janelas.at(0).i))
            {
                qDebug() << "\t2. [ATENCAO] REMOVER T" << falta_escalonar.at(j).tarefa << "," <<
                            falta_escalonar.at(j).i;
                falta_escalonar.move(j, falta_escalonar.size()-1);
            }
        }
        falta_escalonar.removeLast();

        QList<Processador_Escalonamento> processadore_livres;
        for(int i = 0; i < numero_processadores; ++i)
        {
            if(processadores.at(i)->escalonados.at(tempo).foi_executado == false)
            {
                qDebug() << "\t[LOG 3] [escalonar]: Slot vazio em P" << processadores.at(i)->id;
                processadore_livres.append(*processadores.at(i));

                processadores.at(i)->tempo++;

                indice_processador++;
            }
        }



    }
    else
    {
        qDebug() << "\t[LOG] [falta_escalonar] : Existem mais de 1 job com o mesmo deadline";
        qDebug() << "\t[LOG] [falta_escalonar] : Escolha arbitraria. Escalonando o maximo dos processadores";

        for(int i = 0; i < janelas.size(); ++i)
        {
            if(indice_processador < numero_processadores)
            {
                add_secundario(janelas.at(i), indice_processador, ja_executado);
                for(int j =0 ; j < falta_escalonar.size(); ++j)
                {
                    if((falta_escalonar.at(j).tarefa == janelas.at(i).tarefa) &&
                            (falta_escalonar.at(j).i == janelas.at(i).i))
                    {
                        qDebug() << "\t3. [ATENCAO] REMOVER T" << falta_escalonar.at(j).tarefa << "," <<
                                    falta_escalonar.at(j).i;
                        falta_escalonar.removeAt(j);
                    }
                }
            }
            else
            {
                qDebug() << "\t[falta_escalonar] Chegamos ao limite maximo de escalonamento";
                return falta_escalonar;
                break;
            }
        }
    }

}
void PFair_OLD::escalonar_restante(QList<Janela> &janelas_que_podem_executar, QList<Janela> &falta_escalonar_janelas, int tempo, QList<Janela> &ja_executado)
{
    qDebug() << "[LOG] [escalonar_restante] : Janelas que podem executar = " << janelas_que_podem_executar.size();
    QList<Janela> janelas = janelas_que_podem_executar;

    if(janelas.size() == 1)
    {
        qDebug() << "[LOG] [escalonar_restante] : Escalonar PRIMEIRO CASO";
        adicionar_tarefa_processador(janelas.at(0), indice_processador,janelas_que_podem_executar, ja_executado );
    }
    else if(janelas.size() > 1)
    {
        qDebug() << "[LOG] [escalonar_restante] : Existem mais de 1 job com o mesmo deadline";
        resolver_deadlines(tempo, janelas, falta_escalonar_janelas, ja_executado, janelas_que_podem_executar);
    }


    for(int i = 0; i < janelas_que_podem_executar.size();++i)
    {
        qDebug() << "[LOG][escalonar_restante] Falta escalonar T" << janelas_que_podem_executar.at(i).tarefa <<
                    janelas_que_podem_executar.at(i).i;
    }
}
void PFair_OLD::escalonar()
{
    QList<Janela> falta_escalonar_janelas;

    for(int tempo = 0; tempo < tempo_execucao; ++tempo)
    {
        QList<Janela> ja_executado;

        qDebug() << "-----------------------------TEMPO =" << tempo << "-----------------------------";
        qDebug() << "[LOG] [escalonar] : TEMPO = " << tempo;


        QList<Janela> janelas_que_podem_executar = retornar_janelas_que_podem_executar(tempo);

        qDebug() << "[LOG] [escalonar]: Faltam escalonar " << falta_escalonar_janelas.size() << "tarefas.";
        if(falta_escalonar_janelas.size() > 0 )
        {
            qDebug() << "[LOG] [escalonar] : Size escalonar " << falta_escalonar_janelas.size();
            falta_escalonar(falta_escalonar_janelas, tempo-1, ja_executado);
        }


        qDebug() << "[LOG] [escalonar] : Janelas que podem executar = " << janelas_que_podem_executar.size();
        QList<Janela> janelas = verificar_primeiro_caso(tempo, janelas_que_podem_executar);


        if(janelas.size() == 1)
        {
            qDebug() << "[LOG] [escalonar] : Escalonar PRIMEIRO CASO";
            adicionar_tarefa_processador(janelas.at(0), indice_processador,janelas_que_podem_executar, ja_executado );
        }
        else if(janelas.size() > 1)
        {
            qDebug() << "[LOG] [escalonar] : Existem mais de 1 job com o mesmo deadline";
            resolver_deadlines(tempo, janelas, falta_escalonar_janelas, ja_executado, janelas_que_podem_executar);
        }


        for(int i = 0; i < janelas_que_podem_executar.size();++i)
        {
            qDebug() << "Falta escalonar T" << janelas_que_podem_executar.at(i).tarefa <<
                        janelas_que_podem_executar.at(i).i;
        }
        if(janelas_que_podem_executar.size() > 0)
        {
            //falta_escalonar_janelas.append(falta_escalonar(janelas_que_podem_executar,tempo, ja_executado));
        }
        else
            return;

        avancar_tempo(tempo);
    }
}
void PFair_OLD::avancar_tempo(int tempo)
{
    for(int i = 0; i < numero_processadores; ++i)
    {
        if(processadores.at(i)->escalonados.at(tempo).foi_executado == false)
        {
                qDebug() << "[avancar_tempo] Avancar: Slot vazio em P" << processadores.at(i)->id;
                processadores.at(i)->tempo++;
                indice_processador++;
        }
    }
}
void PFair_OLD::resolver_deadlines(int tempo, QList<Janela> janelas, QList<Janela> &falta_escalonar_janelas, QList<Janela> &ja_executado, QList<Janela> &janelas_que_podem_executar)
{
    int tarefa = -1;
    for(int i = tempo+1; i < tempo_execucao; ++i)
    {
        QList<Janela> janelas_frente = verificar_primeiro_caso(i,janelas_que_podem_executar);

        if(janelas_frente.size() == 1)
        {
            qDebug() << "[LOG] [resolver_deadlines] : --->>>> d(T i ) = d(U j ) and b(T i ) > b(U j ) -> Escalonar SEGUNDO CASO T" << janelas_frente.at(0).tarefa;

            tarefa = janelas_frente.at(0).tarefa;
            break;
        }
    }
    if(tarefa != -1)
    {
        for(int i = 0; i < janelas_que_podem_executar.size(); ++i)
        {
            if(janelas_que_podem_executar.at(i).tarefa == tarefa)
                 adicionar_tarefa_processador(janelas_que_podem_executar.at(i),indice_processador, janelas_que_podem_executar,ja_executado);
        }
    }
    else
    {
        qDebug() << "[LOG] [resolver_deadlines] Tarefas iguais com mesmos B(ti)";
    }

}
/*void PFair_OLD::escalonar_old()
{

    QList<Janela> falta_escalonar_janelas;

    for(int tempo = 0; tempo < tempo_execucao; ++tempo)
    {
        QList<Janela> ja_executado;

        qDebug() << "----------------------------------------------------------------------";
        qDebug() << "[LOG] [escalonar] : TEMPO = " << tempo;
        if(falta_escalonar_janelas.size() > 0 )
        {
            qDebug() << "[LOG] [escalonar] : Size escalonar " << falta_escalonar_janelas.size();
            falta_escalonar(falta_escalonar_janelas, tempo-1, ja_executado);

        }

        qDebug() << "[LOG] [escalonar]: Size escalonar " << falta_escalonar_janelas.size();


        QList<Janela> janelas_que_podem_executar = retornar_janelas_que_podem_executar(tempo);


        qDebug() << "[LOG] [escalonar] : Janelas que podem executar = " << janelas_que_podem_executar.size();


        QList<Janela> janelas = verificar_primeiro_caso(tempo);
        if(janelas.size() == 1)
        {
            qDebug() << "[LOG] [escalonar] : Escalonar PRIMEIRO CASO";
            adicionar_tarefa_processador(janelas.at(0), indice_processador,janelas_que_podem_executar, ja_executado );
        }
        else if(janelas.size() > 1)
        {
            qDebug() << "[LOG] [2. escalonar] : Existem mais de 1 job com o mesmo deadline";

            int tarefa = -1;
            for(int i = tempo+1; i < tempo_execucao; ++i)
            {
                QList<Janela> janelas_frente = verificar_primeiro_caso(i);
                //qDebug() << "[LOG] [escalonar] : Verificando no tempo = " << i << "Janelas size = " << janelas_frente.size();

                if(janelas_frente.size() == 1)
                {
                    qDebug() << "[LOG] [escalonar] : --->>>> d(T i ) = d(U j ) and b(T i ) > b(U j ) -> Escalonar SEGUNDO CASO T" << janelas_frente.at(0).tarefa;

                    tarefa = janelas_frente.at(0).tarefa;
                    break;
                }
            }
            if(tarefa != -1)
            {
                qDebug() << "[LOG] [escalonar] : ENTREI AQUI. janela size" << janelas.size();

                for(int i = 0; i < janelas_que_podem_executar.size(); ++i)
                {
                      qDebug() << "[LOG] [escalonar] : Janela tarefa " << janelas_que_podem_executar.at(i).tarefa <<
                                  "tarefa encontrada " << tarefa;

                    if(janelas_que_podem_executar.at(i).tarefa == tarefa)
                    {
                        add_secundario(janelas_que_podem_executar.at(i), indice_processador, ja_executado);
                         janelas_que_podem_executar.removeAt(i);
                    }
                }
            }
            else
            {
                qDebug() << "[LOG] [escalonar] -----------------------> BUG: Tarefas iguais";
                qDebug() << "[LOG] [escalonar]-> Janelas size" << janelas.size();


                QList<Processador_Escalonamento> processadore_livres;
                for(int i = 0; i < numero_processadores; ++i)
                {
                    if(processadores.at(i)->escalonados.at(tempo).foi_executado == false)
                    {
                        qDebug() << "[DEBUG] Slot vazio em P" << processadores.at(i)->id;
                        processadore_livres.append(*processadores.at(i));
                    }
                }

                if(janelas.size() <= processadore_livres.size())
                {
                    qDebug() << "[LOG] [escalonar] : 1. Temos espaco para escalonar";


                   falta_escalonar(janelas,tempo, ja_executado);
                }
                else
                {
                    QList<Janela> executados;
                    QList<Janela> janelas_frente = verificar_primeiro_caso(tempo+1);
                    qDebug() << "[LOG] [escalonar] : Temos pouco espaco. Menor pseudo = " << janelas_frente.size();

                    for(int i = 0; i < janelas.size(); ++i)
                    {
                        for(int x =0 ; x< janelas_frente.size(); ++x)
                        {
                            if(janelas_frente.at(x).tarefa == janelas.at(i).tarefa)
                            {
                                executados.append(janelas_frente.at(x));
                                adicionar_tarefa_processador(janelas.at(i), indice_processador,janelas_que_podem_executar,ja_executado);
                            }
                        }
                    }

                    if((janelas_que_podem_executar.size() > 0) && (executados.size() >0))
                    {
                        for(int i =0; i < janelas_que_podem_executar.size(); ++i)
                        {
                            if(i < executados.size())
                            {
                                if((janelas_que_podem_executar.at(i).tarefa != executados.at(i).tarefa) &&
                                        (janelas_que_podem_executar.at(i).i != executados.at(i).i))
                                {
                                    falta_escalonar_janelas.append(janelas_que_podem_executar.at(i));
                                }
                            }
                        }
                    }


                    qDebug() << "[LOG] [escalonar] :  Falta executar:" << falta_escalonar_janelas.size();


                    for(int i = 0; i < numero_processadores; ++i)
                    {
                        if(processadores.at(i)->escalonados.at(tempo).foi_executado == false)
                        {

                            falta_escalonar(falta_escalonar_janelas,tempo, ja_executado);
                        }
                    }

                    qDebug() << "[LOG] [escalonar] : 2. Falta executar:" << falta_escalonar_janelas.size();

                }

            }



            QList<Processador_Escalonamento> processadore_livres;
            for(int i = 0; i < numero_processadores; ++i)
            {
                if(processadores.at(i)->escalonados.at(tempo).foi_executado == false)
                {
                    qDebug() << "[LOG 2] [escalonar]: Slot vazio em P" << processadores.at(i)->id;
                    processadore_livres.append(*processadores.at(i));
                }
            }

            if(janelas.size() <= processadore_livres.size())
            {
                qDebug() << "[LOG] [escalonar] : 2. Temos espaco para escalonar";


                for(int i = 0; i < janelas.size(); ++i)
                {
                    adicionar_tarefa_processador(janelas.at(i), indice_processador,janelas_que_podem_executar,ja_executado);

                }
                if((janelas.size() == 0) && (falta_escalonar_janelas.size() > 0))
                {

                        qDebug() << "[LOG] [escalonar] : TEMOS QUE ESCALONAR ESSES CARAS Size escalonar " << falta_escalonar_janelas.size();

                        falta_escalonar(falta_escalonar_janelas, tempo-1, ja_executado);
                }

            }
            else
            {


            }
        }
        else if(janelas.size() == 0)
        {
            qDebug() << "[LOG] [escalonar]: Não tem tarefas pra escalonar";
        }

        qDebug() << "[LOG] [escalonar]: ------------> ADICIONANDO";
        // janelas_que_podem_executar - executados

        for(int i = 0; i < ja_executado.size(); ++i)
        {
            qDebug() << "[LOG] [escalonar]: Ja foi executado T" <<ja_executado.at(i).tarefa <<
                        "," << ja_executado.at(i).i;

        }

        for(int i = 0; i < janelas_que_podem_executar.size(); ++i)
        {
            qDebug() << "[LOG] [escalonar]: PODEM EXECUTAR T" <<janelas_que_podem_executar.at(i).tarefa <<
                        "," << janelas_que_podem_executar.at(i).i;

        }

        for(int i = 0; i < janelas_que_podem_executar.size(); ++i)
        {
            for(int j =0 ; j < ja_executado.size(); ++j)
            {
                if(i < janelas_que_podem_executar.size())
                {
                    if((janelas_que_podem_executar.at(i).tarefa == ja_executado.at(j).tarefa) &&
                            (janelas_que_podem_executar.at(i).i == ja_executado.at(j).i))
                    {
                        qDebug() << "[LOG] [escalonar]: Removendo da lista T" <<janelas_que_podem_executar.at(i).tarefa <<
                                    "," << janelas_que_podem_executar.at(i).i;

                        janelas_que_podem_executar.removeAt(i);

                    }
                }
            }
        }

        for(int i = 0; i < falta_escalonar_janelas.size(); ++i)
        {
            for(int j =0 ; j < janelas_que_podem_executar.size(); ++j)
            {
                if((falta_escalonar_janelas.at(i).tarefa == janelas_que_podem_executar.at(j).tarefa) &&
                  (falta_escalonar_janelas.at(i).i == janelas_que_podem_executar.at(j).i))
                {
                    qDebug() << "[LOG] [escalonar]: 2. Removendo da lista T" <<falta_escalonar_janelas.at(i).tarefa <<
                                "," << falta_escalonar_janelas.at(i).i;

                    falta_escalonar_janelas.removeAt(i);

                }
            }
        }

        qDebug() << "[LOG] [escalonar]: falta_escalonar_janelas: " << falta_escalonar_janelas.size();

        qDebug() << "[LOG] [escalonar]: janelas_que_podem_executar: " << janelas_que_podem_executar.size();

        qDebug() << "[LOG] [escalonar]: Ja executados: " << ja_executado.size();
       // falta_escalonar_janelas.append(janelas_que_podem_executar);

        bool achei = false;

        for(int i = 0; i < numero_processadores; ++i)
        {
            if(processadores.at(i)->escalonados.at(tempo).foi_executado == false)
            {
                if(falta_escalonar_janelas.size() == 0)
                {
                    qDebug() << "[FIM] Slot vazio em P" << processadores.at(i)->id;
                    achei = true;

                    processadores.at(i)->tempo++;
                    indice_processador++;
                }
            }
        }
        if(!achei)
        {

            for(int i = 0; i < numero_processadores; ++i)
            {
                if(processadores.at(i)->escalonados.at(tempo).foi_executado == false)
                {
                    if((janelas.size() == 0))
                    {
                        qDebug() << "[2. FIM] Slot vazio em P" << processadores.at(i)->id
                                 << "no tempo = " << tempo;
                        //processadores.at(i)->tempo++;

                       // indice_processador++;
                    }

                }
            }
        }
    }
}*/

QList<Janela> PFair_OLD::retornar_janelas_que_podem_executar(int tempo)
{
    QList<Janela> lista_janelas;




    int novo_tempo;

    for(int i = 0; i < task_list.size(); ++i)
    {
        int constante =  floor(tempo/task_list.at(i)->deadline);

        for(int j = 0; j < task_list.at(i)->janelas.size(); ++j)
        {

            novo_tempo = (task_list.at(i)->deadline*constante) + task_list.at(i)->janelas.at(j).pseudo_chegada ;
           /* qDebug() << "[LOG] [retornar_janelas_que_podem_executar] Pode executar T" <<
                        task_list.at(i)->janelas.at(j).tarefa << ","
                     << task_list.at(i)->janelas.at(j).i << " em "
                     << task_list.at(i)->janelas.at(j).pseudo_chegada << "Constante"
                     <<  constante <<
                         "Tempo " << tempo <<
                         "novo tempo " << novo_tempo;*/

            if(tempo  == novo_tempo)
            {

                lista_janelas.append(task_list.at(i)->janelas.at(j));
            }
        }


    }

    /*
    for(int i = 0; i < tarefas.size(); ++i)
    {
        for(int j = 0; j < tarefas.at(i).size(); ++j)
        {
            for(int x = 0; x < tarefas.at(i).at(j).janelas.size(); ++x)
            {
                if(tarefas.at(i).at(j).janelas.at(x).pseudo_chegada == tempo)
                    lista_janelas.append(tarefas.at(i).at(j).janelas.at(x));
            }
        }
    }*/
    return lista_janelas;
}

void PFair_OLD::criar_janelas(QList<Simulacoes> simulacoes)
{
    Simulacoes sim = simulacoes.at(0);
    tempo_execucao = sim.tempo_execucao;

    tarefas.clear();

    //QList<Task> task_list;


    for(int i = 0; i < sim.jobs.size(); ++i)
    {
        int indice = 0;
        int indice_sub_tarefa = 0;
        for(int j = 1; j<= (sim.tempo_execucao); ++j)
        {
            Job *t_new = new Job;

            indice_sub_tarefa = 0;

            t_new->deadline = sim.jobs.at(i).deadline*(j);
            t_new->inicio = sim.jobs.at(i).deadline*(j-1);
            t_new->tempo_execucao = sim.jobs.at(i).tempo_execucao;


            if(t_new->deadline <= sim.tempo_execucao)
            {
                for(int x = t_new->inicio; x < t_new->tempo_execucao; x = x+1)
                {

                    Janela window;

                    window.pseudo_chegada = floor((indice)/(t_new->tempo_execucao/sim.jobs.at(i).deadline));
                    window.pseudo_deadline = ceil((indice+1)/(t_new->tempo_execucao/sim.jobs.at(i).deadline));

                    //if(indice == indice_sub_tarefa)
                      // indice_sub_tarefa = 0;


                    qDebug() << "Inicio: " << t_new->inicio << " Fim: " << t_new->deadline
                             << " P-Chegada " << window.pseudo_chegada << " P-Deadline" << window.pseudo_deadline
                            << " I: " << indice << " X: " << indice_sub_tarefa << "Tarefa: " << i;

                    window.foi_executado = false;
                    window.inicio = t_new->inicio;
                    window.fim = t_new->deadline;
                    window.i = indice_sub_tarefa;
                    window.tarefa = i;
                    window.invalido = false;

                    int topo, chao;


                    topo = ceil((indice+1)/(t_new->tempo_execucao/sim.jobs.at(i).deadline));
                    chao = floor((indice+1)/(t_new->tempo_execucao/sim.jobs.at(i).deadline));


                    if(topo == chao)
                        window.b = false;
                    else
                        window.b = true;


                    //  qDebug() << "Topo: " << topo << "Chao: " << chao << "Indice" << indice << "B(ti) = " << window.b;



                    t_new->janelas.append(window);

                    indice_sub_tarefa++;
                    indice++;
                }

                task_list.append(t_new);

            }
        }
        // tarefas.append(task_list);
        // task_list.clear();

    }

}
