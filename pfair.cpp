#include "pfair.h"

PFair::PFair()
{
    processador_atual = 0;

    indice_processador = 0;
    evitar_migracao = false;
    possivel_escalonar = true;
}

void PFair::escalonar()
{
    QList<Janela> falta_escalonar_janelas;

    for(int tempo = 0; tempo < tempo_execucao; ++tempo)
    {
        QList<Janela> ja_executado;

        //qDebug() << "-----------------------------TEMPO =" << tempo << "-----------------------------";
        //qDebug() << "[LOG] [escalonar] : TEMPO = " << tempo;
        //qDebug() << "[LOG] [escalonar]: Faltam escalonar " << falta_escalonar_janelas.size() << "tarefas.";

        for(int i = 0; i < falta_escalonar_janelas.size();++i)
        {
            //qDebug() << "[LOG] [escalonar]: falta_escalonar_janelas T" << falta_escalonar_janelas.at(i).tarefa <<
                        //falta_escalonar_janelas.at(i).i;
        }

        QList<Janela> janelas_que_podem_executar = retornar_janelas_que_podem_executar(tempo);
        //qDebug() << "[LOG] [escalonar]: janelas_que_podem_executar size " << janelas_que_podem_executar.size();

        for(int i = 0; i < janelas_que_podem_executar.size();++i)
        {
            //qDebug() << "[LOG] [escalonar]: janelas_que_podem_executar T" << janelas_que_podem_executar.at(i).tarefa <<
                        janelas_que_podem_executar.at(i).i;
        }


        QList<Janela> janelas;
        if(falta_escalonar_janelas.size() > 0 )
        {


            falta_escalonar_janelas.append(janelas_que_podem_executar);
            //qDebug() << "[LOG] [escalonar] : Size escalonar " << falta_escalonar_janelas.size();
            //qDebug() << "[ENTRANDO PRIMEIRO IF] --------------------------------------------------";
            janelas_que_podem_executar = falta_escalonar(falta_escalonar_janelas, tempo, ja_executado,janelas_que_podem_executar );
            //qDebug() << "[SAINDO PRIMEIRO IF] ----------------------------------------------------";

            //qDebug() << "[LOG] [escalonar] : Janelas que podem executar = " << janelas_que_podem_executar.size();



            for(int i = 0; i < ja_executado.size(); ++i)
            {
                for(int j = 0; j < falta_escalonar_janelas.size(); ++j)
                {
                    if((falta_escalonar_janelas.at(j).tarefa == ja_executado.at(i).tarefa)&&
                            (falta_escalonar_janelas.at(j).i == ja_executado.at(i).i))
                    {
                        //qDebug() << "[LOG] [escalonar]: REMOVENDO" << falta_escalonar_janelas.at(j).tarefa <<
                                    falta_escalonar_janelas.at(j).i;
                        falta_escalonar_janelas.removeAt(j);
                    }
                }
            }


            for(int i = 0; i < ja_executado.size(); ++i)
            {
                //qDebug() << "[LOG] [escalonar]: Ja foi executado T" << ja_executado.at(i).tarefa <<
                            ja_executado.at(i).i;
            }



            // return;

        }
        else
        {
            //qDebug() << "[LOG] [escalonar] : ELSE";
            janelas = verificar_primeiro_caso(tempo);

            for(int i = 0; i < janelas.size();++i)
            {
                ////qDebug() << "[LOG] [escalonar] : +++ JANELAS " << janelas.at(i).tarefa <<
                //            janelas.at(i).i;
            }
        }


        if(janelas.size() == 1)
        {
            //qDebug() << "[LOG] [escalonar] : Escalonar PRIMEIRO CASO";
            adicionar_tarefa_processador(janelas.at(0), indice_processador,janelas_que_podem_executar, ja_executado );
        }
        else if(janelas.size() > 1)
        {
            //qDebug() << "[LOG] [escalonar] : Existem mais de 1 job com o mesmo PSEUDO deadline t" <<tempo;

            for(int i = 0; i < janelas.size();++i)
            {
                //qDebug() << "[LOG] [escalonar] : PSEUDO deadline" << janelas.at(i).tarefa <<
                            janelas.at(i).i;
            }

            resolver_deadlines(tempo, ja_executado, janelas, janelas_que_podem_executar);
        }


        if(janelas_que_podem_executar.size() > 0)
        {

            //qDebug() << "[ENTRANDO FUNCAO] -------------------------------------------------------";
            //qDebug() << "Tamanho" << janelas_que_podem_executar.size();

            int cont = 0;
            for(int i = 0; i < numero_processadores; ++i)
            {
                if(processadores.at(i)->escalonados.at(tempo).foi_executado == false)
                {
                    //qDebug() << "[avancar_tempo] Avancar: Slot vazio em P" << processadores.at(i)->id;
                    cont++;
                }
            }
            QList<Janela> faltam;

            if(cont == 0)
            {
                //qDebug() << "[avancar_tempo] Avancar:Não temos mais espaço";
            }

            faltam = falta_escalonar(falta_escalonar_janelas,tempo, ja_executado, janelas_que_podem_executar);




            //qDebug() << "[SAINDO FUNCAO] -------------------------------------------------------";

            falta_escalonar_janelas.append(faltam);

            /* for(int i = 0; i < falta_escalonar_janelas.size(); ++i)
            {
                //qDebug() << "AQUI EXISTE T" << falta_escalonar_janelas.at(i).tarefa <<
                            falta_escalonar_janelas.at(i).i;
            }*/


            for(int i = 0; i < falta_escalonar_janelas.size(); ++i)
            {
                for(int j = i+1; j< falta_escalonar_janelas.size();++j)
                {
                    if((falta_escalonar_janelas.at(i).tarefa == falta_escalonar_janelas.at(j).tarefa ) &&
                            (falta_escalonar_janelas.at(i).i == falta_escalonar_janelas.at(j).i ))
                    {
                        ////qDebug() << "Temos que remover T" << falta_escalonar_janelas.at(i).tarefa <<
                        //          falta_escalonar_janelas.at(i).i;
                        falta_escalonar_janelas.removeAt(i);
                    }
                }
            }


        }


        avancar_tempo(tempo);
    }
}

QList<Janela> PFair::falta_escalonar(QList<Janela> &falta_escalonar_janelas, int tempo, QList<Janela> &ja_executado,
                                     QList<Janela> &janelas_que_podem_executar)
{

    if(indice_processador >= numero_processadores)
        indice_processador = 0;

    //qDebug() << "\t-----------------------------TEMPO =" << tempo << "-----------------------------";
    //qDebug() << "\t[LOG] [falta_escalonar] : TEMPO = " << tempo;


    //qDebug() << "\t[LOG] [falta_escalonar] : falta_escalonar_janelas= " << falta_escalonar_janelas.size();
    //qDebug() << "\t[LOG] [falta_escalonar] : Janelas que podem executar = " << janelas_que_podem_executar.size();

    bool inverso = false;
    QList<Janela> janelas;
    if(falta_escalonar_janelas.size() > janelas_que_podem_executar.size())
    {
        inverso = true;


        janelas =  verificar_primeiro_caso_new(janelas_que_podem_executar, tempo);

        for(int i = 0; i < janelas_que_podem_executar.size();++i)
        {
            //qDebug() << "\t[LOG] [falta_escalonar]: JANELAS QUE FALTAM EXECUTAR T" << falta_escalonar_janelas.at(i).tarefa <<
                        falta_escalonar_janelas.at(i).i;
        }


        for(int i = 0; i < janelas_que_podem_executar.size();++i)
        {
            //qDebug() << "\t[LOG] [falta_escalonar]: JANELAS QUE PODEM EXECUTAR T" << janelas_que_podem_executar.at(i).tarefa <<
                        janelas_que_podem_executar.at(i).i;
        }

    }
    else
        janelas =  verificar_primeiro_caso_new(janelas_que_podem_executar, tempo);




    for(int i = 0; i < janelas.size();++i)
    {
        //qDebug() << "\t[LOG] [falta_escalonar]: menores_pseudo_deadline T" << janelas.at(i).tarefa <<
                    janelas.at(i).i;
    }


    if(janelas.size() == 1)
    {
        //qDebug() << "\t[LOG] [falta_escalonar] : Escalonar PRIMEIRO CASO";
        adicionar_tarefa_processador(janelas.at(0), indice_processador,janelas_que_podem_executar, ja_executado );

        for(int j =0 ; j < falta_escalonar_janelas.size(); ++j)
        {
            if((falta_escalonar_janelas.at(j).tarefa == janelas.at(0).tarefa) &&
                    (falta_escalonar_janelas.at(j).i == janelas.at(0).i))
            {
                //qDebug() << "------------------[2. NEWWW ATENCAO] [adicionar_tarefa_processador] -->> REMOVER T" << falta_escalonar_janelas.at(j).tarefa << "," <<
                            falta_escalonar_janelas.at(j).i;
                falta_escalonar_janelas.removeAt(j);
            }
        }
    }
    else if(janelas.size() > 1)
    {
        //qDebug() << "\t[LOG] [falta_escalonar] : Existem mais de 1 job com o mesmo deadline";
        //qDebug() << "\t[LOG] [falta_escalonar] : Escolha arbitraria. Escalonando o maximo dos processadores";

        for(int i = 0; i < falta_escalonar_janelas.size(); ++i)
        {
            for(int j = i+1; j< falta_escalonar_janelas.size();++j)
            {
                if((falta_escalonar_janelas.at(i).tarefa == falta_escalonar_janelas.at(j).tarefa ) &&
                        (falta_escalonar_janelas.at(i).i == falta_escalonar_janelas.at(j).i ))
                {
                    falta_escalonar_janelas.removeAt(i);
                }
            }
        }


        int menor_deadline = 999999999;

        QList<Janela> falta_escalonar_janelas_novo;
        falta_escalonar_janelas_novo.append(falta_escalonar_janelas);
        QList<Janela> menores;

        Janela menor_janela;

        while(falta_escalonar_janelas_novo.size() != 0)
        {
            for(int i = 0; i < falta_escalonar_janelas_novo.size(); ++i)
            {
                if(falta_escalonar_janelas_novo.at(i).fim < menor_deadline)
                {
                    menor_janela = falta_escalonar_janelas_novo.at(i);
                    menor_deadline = falta_escalonar_janelas_novo.at(i).fim;

                }
            }
            menores.append(menor_janela);

            for(int x = 0; x < falta_escalonar_janelas_novo.size(); ++x)
            {
                if((falta_escalonar_janelas_novo.at(x).i == menor_janela.i) &&
                        (falta_escalonar_janelas_novo.at(x).tarefa == menor_janela.tarefa))
                {
                    falta_escalonar_janelas_novo.removeAt(x);
                }
            }
            menor_deadline = 999999999;
        }

        falta_escalonar_janelas.clear();
        falta_escalonar_janelas.append(menores);


        if(tentar_evitar_migracao)
        {

            if(tempo >0 )
            {
                if(processadores.at(indice_processador)->escalonados.at(tempo-1).foi_executado)
                {
                    //qDebug() << "TAREFA ANTERIOR T" << processadores.at(indice_processador)->escalonados.at(tempo-1).tarefa  << "," <<
                                processadores.at(indice_processador)->escalonados.at(tempo-1).janela;

                    for(int j = 0; j < janelas.size(); ++j)
                    {
                        if(indice_processador < numero_processadores)
                        {
                            if((processadores.at(indice_processador)->escalonados.at(tempo-1).tarefa == janelas.at(j).tarefa))
                            {
                                //qDebug() << "PODEMOS EVITAR MIGRACAO COM T" << janelas.at(j).tarefa << "," <<
                                            janelas.at(j).i;

                                adicionar_tarefa_processador(janelas.at(j),indice_processador,janelas_que_podem_executar,ja_executado);


                                for(int i =0 ; i < falta_escalonar_janelas.size(); ++i)
                                {
                                    if((falta_escalonar_janelas.at(i).tarefa == janelas.at(j).tarefa) &&
                                            (falta_escalonar_janelas.at(i).i == janelas.at(j).i))
                                    {
                                        //qDebug() << "------------------[NEWWW ATENCAO] [adicionar_tarefa_processador] -->> REMOVER T" << falta_escalonar_janelas.at(i).tarefa << "," <<
                                                    falta_escalonar_janelas.at(i).i;
                                        falta_escalonar_janelas.removeAt(i);
                                    }
                                }


                            }
                        }
                    }
                }

            }
        }

        if(inverso)
        {
            //qDebug() << "\t[falta_escalonar]: INVERSO = TRUE";
            for(int i = 0; i < falta_escalonar_janelas.size(); ++i)
            {
                //qDebug() << "\t[LOG] [falta_escalonar] : Indice processador = " <<indice_processador;
                if(indice_processador < numero_processadores)
                {
                    adicionar_tarefa_processador(falta_escalonar_janelas.at(i),indice_processador,janelas_que_podem_executar,ja_executado);

                    for(int j =0 ; j < falta_escalonar_janelas.size(); ++j)
                    {
                        if((falta_escalonar_janelas.at(j).tarefa == falta_escalonar_janelas.at(i).tarefa) &&
                                (falta_escalonar_janelas.at(j).i == falta_escalonar_janelas.at(i).i))
                        {
                            //qDebug() << "------------------[NEWWW ATENCAO] [adicionar_tarefa_processador] -->> REMOVER T" << falta_escalonar_janelas.at(j).tarefa << "," <<
                                        falta_escalonar_janelas.at(j).i;
                            falta_escalonar_janelas.removeAt(j);
                        }
                    }
                }
                else
                {
                    //qDebug() << "\t[falta_escalonar] Chegamos ao limite maximo de escalonamento";
                    return janelas_que_podem_executar;
                    break;
                }
            }
        }
        else
        {

            //qDebug() << "\t[falta_escalonar]: INVERSO = FALSE";

            for(int i = 0; i < janelas.size(); ++i)
            {
                //qDebug() << "\t[LOG] [falta_escalonar] JANELAS T:" <<janelas.at(i).tarefa<<
                            //janelas.at(i).i << "FIM" << janelas.at(i).fim;
            }

            for(int i = 0; i < janelas.size(); ++i)
            {
                //qDebug() << "\t[LOG] [falta_escalonar] : Indice processador = " <<indice_processador;
                if(indice_processador < numero_processadores)
                {
                    adicionar_tarefa_processador(janelas.at(i),indice_processador,janelas_que_podem_executar,ja_executado);

                    for(int j =0 ; j < falta_escalonar_janelas.size(); ++j)
                    {
                        if((falta_escalonar_janelas.at(j).tarefa == janelas.at(i).tarefa) &&
                                (falta_escalonar_janelas.at(j).i == janelas.at(i).i))
                        {
                            //qDebug() << "------------------[NEWWW ATENCAO] [adicionar_tarefa_processador] -->> REMOVER T" << falta_escalonar_janelas.at(j).tarefa << "," <<
                                        falta_escalonar_janelas.at(j).i;
                            falta_escalonar_janelas.removeAt(j);
                        }
                    }
                }
                else
                {
                    //qDebug() << "\t[falta_escalonar] Chegamos ao limite maximo de escalonamento";
                    return janelas_que_podem_executar;
                    break;
                }
            }
        }





        // return falta_escalonar_janelas;
    }
    else if(janelas.size() == 0)
        return falta_escalonar_janelas;


    int cont = 0;
    for(int i = 0; i < numero_processadores; ++i)
    {
        if(processadores.at(i)->escalonados.at(tempo).foi_executado == false)
        {
            //qDebug() << "\t[LOG] [falta_escalonar] : Verificar espacos vazios P" << processadores.at(i)->id;
            cont++;
        }
    }
    if((cont > 0) && (falta_escalonar_janelas.size() > 0))
    {
        //qDebug() << "\t[LOG] [falta_escalonar]: janelas_que_podem_executar SIZE " << janelas_que_podem_executar.size();
        for(int i = 0; i < janelas_que_podem_executar.size();++i)
        {
            //qDebug() << "\t[LOG] [falta_escalonar]: janelas_que_podem_executar T" << janelas_que_podem_executar.at(i).tarefa <<
                        janelas_que_podem_executar.at(i).i;
        }

        //qDebug() << "\t[LOG] [falta_escalonar] : Vamos nos chamar de novo. Temos espaço livre.";


        QList<Janela> retorno = falta_escalonar(falta_escalonar_janelas,tempo, ja_executado, janelas_que_podem_executar);

        return retorno;

    }


    for(int i = 0; i < janelas_que_podem_executar.size();++i)
    {

        //qDebug() << "\t[LOG] [falta_escalonar]: Falta escalonar T" << janelas_que_podem_executar.at(i).tarefa <<
                    janelas_que_podem_executar.at(i).i;
    }
    return janelas_que_podem_executar;

}

void PFair::criar_processadores(int processadores)
{
    numero_processadores = processadores;
}

void PFair::calcular_simulacao(QList<Simulacoes> simulacoes, int sim_number)
{
    criar_janelas(simulacoes, sim_number);
    povoar_processadores();
    escalonar();
}

void PFair::povoar_processadores()
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

QList<Janela> PFair::retornar_janelas_que_podem_executar(int tempo)
{
    QList<Janela> lista_janelas;

    int novo_tempo;

    for(int i = 0; i < task_list.size(); ++i)
    {
        int constante =  floor(tempo/task_list.at(i)->deadline);

        for(int j = 0; j < task_list.at(i)->janelas.size(); ++j)
        {

            novo_tempo = (task_list.at(i)->deadline*constante) + task_list.at(i)->janelas.at(j).pseudo_chegada ;

            if(( novo_tempo == tempo))
            {
                lista_janelas.append(task_list.at(i)->janelas.at(j));
            }
        }


    }

    return lista_janelas;
}


QList<Janela> PFair::verificar_primeiro_caso(int tempo)
{
    QList<Janela> janelas = retornar_janelas_que_podem_executar(tempo);
    QList<Janela> menor_deadline = menores_pseudo_deadline(janelas,tempo);

    ////qDebug() << "Janelas size = " << janelas.size() << "Menor size " << menor_deadline.size();

    if(menor_deadline.size() == 1)
    {
        //qDebug() << "\t\t[LOG] [verificar_primeiro_caso] -> Achei o menor deadline Tempo = " <<tempo;
        return menor_deadline;
    }
    else
    {
        ////qDebug() << "[LOG] [verificar_primeiro_caso] -> Existem mais JOBS com pseudo deadline iguais. ";
        return menor_deadline;
    }
}

QList<Janela> PFair::verificar_primeiro_caso_new(QList<Janela> janelas, int tempo)
{
    QList<Janela> menor_deadline = menores_pseudo_deadline(janelas, tempo);

    if(menor_deadline.size() == 1)
    {
        //qDebug() << "\t\t[LOG] [verificar_primeiro_caso_new] -> Achei o menor deadline [OK]";
        return menor_deadline;
    }
    else
    {
        //qDebug() << "[LOG] [verificar_primeiro_caso_new] -> Existem mais JOBS com pseudo deadline iguais. ";
        return menor_deadline;
    }
}

void PFair::adicionar_tarefa_processador(Janela janela, int &indice_processador,  QList<Janela> &janelas_que_podem_executar,  QList<Janela> &ja_executado)
{
    if(indice_processador > processadores.size())
        indice_processador = 0;

    if(indice_processador == processadores.size())
        indice_processador = 0;

    if(processadores.at(indice_processador)->tempo >= tempo_execucao)
    {
        //qDebug() << "Não dá pra alocar mais. Acabou o tempo de execução";
        return;
    }

    if( verificar_ja_foi_executado(janela,  processadores.at(indice_processador)->tempo))
    {
        //qDebug() << "Já foi executado";
        //return;
    }


    bool achei = false;
    for(int i = 0; i < task_list.size(); ++i)
    {
        for(int j = 0; j < task_list.at(i)->janelas.size(); ++j)
        {
            if(task_list.at(i)->janelas.at(j).tarefa == janela.tarefa)
            {
                ////qDebug() << "A tarefa termina em " << task_list.at(i)->tempo_execucao;

                if(janela.i >= task_list.at(i)->tempo_execucao)
                {
                    //qDebug() << "[adicionar_tarefa_processador]: NAO VAMOS ADICIONAR ESSA TAREFA";
                    return;
                }
                achei = true;
                break;
            }
        }
        if(achei)
            break;
    }

    //qDebug() << "------------------[ADD] ----->  Adicionando T" << janela.tarefa << "," << janela.i << " no processador " << indice_processador
             //<< "-> Tempo = " << processadores.at(indice_processador)->tempo;

    ja_executado.append(janela);

    Escalonamento esc;
    esc.tempo = processadores.at(indice_processador)->tempo;
    esc.processador = indice_processador;
    esc.janela = janela.i;
    esc.tarefa = janela.tarefa;
    esc.foi_executado = true;
    esc.deadline = janela.fim;

    processadores.at(indice_processador)->escalonados.replace(processadores.at(indice_processador)->tempo, esc);
    processadores.at(indice_processador)->ultimo = esc;
    processadores.at(indice_processador)->tempo++;


    for(int j =0 ; j < janelas_que_podem_executar.size(); ++j)
    {
        if((janelas_que_podem_executar.at(j).tarefa == janela.tarefa) &&
                (janelas_que_podem_executar.at(j).i == janela.i))
        {
            //qDebug() << "------------------[ATENCAO] [adicionar_tarefa_processador] -->> REMOVER T" << janelas_que_podem_executar.at(j).tarefa << "," <<
                        janelas_que_podem_executar.at(j).i;
            ja_escalonados.append(janelas_que_podem_executar.at(j));
            janelas_que_podem_executar.removeAt(j);

        }
    }

    if(indice_processador < processadores.size())
        indice_processador++;
}

QList<Janela> PFair::maiores_b_ti(QList<Janela> menor_deadline)
{
    QList<Janela> maiores_bi;

    for(int i = 0; i < menor_deadline.size(); ++i)
    {
        if(menor_deadline.at(i).b == true)
            maiores_bi.append(menor_deadline.at(i));
    }
    return maiores_bi;

}

void PFair::resolver_deadlines(int tempo, QList<Janela> &ja_executado, QList<Janela> Janelas, QList<Janela> &janelas_que_podem_executar)
{
    int tarefa = -1;

    tempo++;
    //qDebug() << "[LOG] [resolver_deadlines] : -- TEMPO = " << tempo;

    QList<Janela> maiores = maiores_b_ti(Janelas);

    if(maiores.size() == 1)
    {
        //qDebug() << "[LOG] [resolver_deadlines] : Somente uma tarefa possui maior B(ti)";
        //qDebug() << "[LOG] [resolver_deadlines] : --->>>> d(T i ) = d(U j ) and b(T i ) > b(U j ) -> Escalonar SEGUNDO CASO T" << maiores.at(0).tarefa;

        adicionar_tarefa_processador(maiores.at(0),indice_processador, janelas_que_podem_executar,ja_executado);
    }
    else
    {
        //qDebug() << "[ATENCAO] [resolver_deadlines] : Mais de uma tarefa possui maior B(ti)";

        for(int i = 0; i < maiores.size(); ++i)
        {
            //qDebug()<< "[LOG] [resolver_deadlines]: MENORES B(ti) T" << maiores.at(i).tarefa
                    //<<maiores.at(i).i;
        }

        for(int i = (tempo); i < tempo_execucao; ++i)
        {
            QList<Janela> janelas_frente = verificar_primeiro_caso(tempo);

            if(janelas_frente.size() == 1)
            {
                //qDebug() << "[LOG] [resolver_deadlines] : --->>>> TERCEIRO CASO" << janelas_frente.at(0).tarefa;

                tarefa = janelas_frente.at(0).tarefa;
                break;
            }
        }
        if(tarefa != -1)
        {
            for(int i = 0; i < Janelas.size(); ++i)
            {
                if(Janelas.at(i).tarefa == tarefa)
                    adicionar_tarefa_processador(Janelas.at(i),indice_processador, janelas_que_podem_executar,ja_executado);
            }
        }
        else
        {
            QList<Janela> janelas_frente = retornar_janelas_que_podem_executar(tempo+1);

            //qDebug() << "[LOG] [resolver_deadlines] Tarefas iguais com mesmos B(ti). Janelas frente"
                     //<< janelas_frente.size();
            //qDebug() << "[LOG] [resolver_deadlines] TERCEIRO CASO";





            QList<Janela> menores_pseudo = menores_pseudo_deadline(janelas_frente, tempo+1);

            for(int i = 0; i < menores_pseudo.size(); ++i)
            {
                //qDebug()<< "[LOG] [resolver_deadlines]: MENORES PSEUDO DEADLINES T" << menores_pseudo.at(i).tarefa
                        //<<menores_pseudo.at(i).i;
            }



            //qDebug() << "[LOG] [resolver_deadlines] Menores pseudo deadline size: " << menores_pseudo.size();


            for(int i = 0; i < Janelas.size();++i)
            {
                for(int j = 0; j < menores_pseudo.size(); ++j)
                {
                    if(indice_processador < numero_processadores)
                    {
                        if(Janelas.at(i).tarefa == menores_pseudo.at(j).tarefa)
                            adicionar_tarefa_processador(Janelas.at(i),indice_processador, janelas_que_podem_executar,ja_executado);
                    }
                }

            }
        }
    }
}
void PFair::avancar_tempo(int tempo)
{
    for(int i = 0; i < numero_processadores; ++i)
    {
        if(processadores.at(i)->escalonados.at(tempo).foi_executado == false)
        {
            //qDebug() << "[avancar_tempo] Avancar: Slot vazio em P" << processadores.at(i)->id;
            processadores.at(i)->tempo++;
            indice_processador++;
        }
    }
}

void PFair::criar_janelas(QList<Simulacoes> simulacoes, int sim_number)
{
    Simulacoes sim = simulacoes.at(sim_number);
    tempo_execucao = sim.tempo_execucao;

    tarefas.clear();

    //QList<Task> task_list;

    double somatorio = 0;

    for(int i = 0; i < sim.jobs.size(); ++i)
    {
        int indice = 0;
        int indice_sub_tarefa = 0;

        somatorio = somatorio + (sim.jobs.at(i).tempo_execucao/sim.jobs.at(i).deadline);

        for(int j = 1; j<= (sim.tempo_execucao); ++j)
        {
            Job *t_new = new Job;

            indice_sub_tarefa = 0;

            t_new->deadline = sim.jobs.at(i).deadline*(j);
            t_new->inicio = sim.jobs.at(i).deadline*(j-1);
            t_new->tempo_execucao = sim.jobs.at(i).tempo_execucao;
            t_new->tarefa = i;



            if(t_new->deadline <= sim.tempo_execucao)
            {
                for(int x = t_new->inicio; x < t_new->tempo_execucao; x = x+1)
                {

                    Janela window;

                    window.pseudo_chegada = floor((indice)/(t_new->tempo_execucao/sim.jobs.at(i).deadline));
                    window.pseudo_deadline = ceil((indice+1)/(t_new->tempo_execucao/sim.jobs.at(i).deadline));

                    //if(indice == indice_sub_tarefa)
                    // indice_sub_tarefa = 0;


                    //qDebug() << "Inicio: " << t_new->inicio << " Fim: " << t_new->deadline
                             //<< " P-Chegada " << window.pseudo_chegada << " P-Deadline" << window.pseudo_deadline
                             //<< " I: " << indice << " X: " << indice_sub_tarefa << "Tarefa: " << i;

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

    double epsilon = 0.001;
    bool menor = std::abs(somatorio - sim.quantidade_processadores) < epsilon;

    if((somatorio < sim.quantidade_processadores) || (menor))
        qDebug() << "É possível escalonar: Somatório = " <<somatorio << "P" << sim.quantidade_processadores;
    else
    {
        possivel_escalonar = false;
        qDebug() << "[ATENCAO] Nao é possivel escalonar."  <<somatorio << "P" << sim.quantidade_processadores;
    }

}
QList<Janela> PFair::menores_pseudo_deadline_new(QList<Janela> janelas, int tempo)
{
    int menor_pseudo_deadline = 10000000000;
    int pseudo_deadline_anterior = 0;

    for(int i = 0; i < janelas.size();++i)
    {

        //qDebug() << "[menores_pseudo_deadline_new] Falta escalonar T" << janelas.at(i).tarefa <<
                    janelas.at(i).i;
    }


    for(int i = 0; i < janelas.size(); ++i)
    {
        pseudo_deadline_anterior = janelas.at(i).pseudo_deadline;
        if(janelas.at(i).pseudo_deadline < menor_pseudo_deadline)
            menor_pseudo_deadline = janelas.at(i).pseudo_deadline;
    }

    //qDebug() << "Menor pseudo deadline = " << menor_pseudo_deadline;

    QList<Janela> menores_deadlines;
    for(int i = 0; i < janelas.size(); ++i)
    {
        if(janelas.at(i).pseudo_deadline == menor_pseudo_deadline)
        {
            //qDebug() << "Menor pseudo deadline T" <<janelas.at(i).tarefa <<"," << janelas.at(i).i << "Pseudo:" << janelas.at(i).pseudo_deadline
                     //<< "FIM" << janelas.at(i).fim;
            menores_deadlines.append(janelas.at(i));
        }
    }

    return menores_deadlines;
}


QList<Janela> PFair::menores_pseudo_deadline(QList<Janela> janelas, int tempo)
{
    int menor_pseudo_deadline = 10000000000;
    int pseudo_deadline_anterior = 0;

    for(int i = 0; i < janelas.size(); ++i)
    {
        int fator = floor(tempo/janelas.at(i).fim) +1;

        pseudo_deadline_anterior = janelas.at(i).pseudo_deadline*fator;
        if(janelas.at(i).pseudo_deadline < menor_pseudo_deadline)
            menor_pseudo_deadline = janelas.at(i).pseudo_deadline*fator;

    }


    /*for(int i = 0; i < janelas.size(); ++i)
    {
        pseudo_deadline_anterior = janelas.at(i).pseudo_deadline;
        if(janelas.at(i).pseudo_deadline < menor_pseudo_deadline)
            menor_pseudo_deadline = janelas.at(i).pseudo_deadline;
    }*/

    QList<Janela> menores_deadlines;
    for(int i = 0; i < janelas.size(); ++i)
    {
        int fator = floor(tempo/janelas.at(i).fim) +1;

        if(janelas.at(i).pseudo_deadline*fator == menor_pseudo_deadline)
        {
            //  //qDebug() << "Menor pseudo deadline T" <<janelas.at(i).tarefa <<"," << janelas.at(i).i << "Pseudo:" << janelas.at(i).pseudo_deadline*fator
            //        << "FIM" << janelas.at(i).fim;
            menores_deadlines.append(janelas.at(i));
        }
    }

    return menores_deadlines;
}

bool PFair::verificar_ja_foi_executado(Janela janela, int tempo)
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


    /*for(int i = 0; i < numero_processadores; ++i)
    {
        for(int j = inicio; j < deadline; ++j)
        {
            if(processadores.at(i)->escalonados.size() < deadline)
            {
                if((processadores.at(i)->escalonados.at(j).janela == janela.i) &&
                        (processadores.at(i)->escalonados.at(j).tarefa == janela.tarefa))
                {
                    //qDebug() << "[BUG]: JA EXECUTADO tempo " << processadores.at(i)->escalonados.at(j).tempo;;
                    return true;
                }
            }
        }

    }*/

    /*
    for(int i = 0; i < numero_processadores; ++i)
    {
        for(int j = 0; j < processadores.at(i)->escalonados.size(); ++j)
        {
            if((processadores.at(i)->escalonados.at(j).janela == janela.i) &&
                    (processadores.at(i)->escalonados.at(j).tarefa == janela.tarefa))
            {
                //qDebug() << "[BUG]: tempo " << processadores.at(i)->escalonados.at(j).tempo;;
                return true;
            }
        }

    }*/
    return false;
}

