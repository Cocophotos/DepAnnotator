#include "wikigraphdialog.h"

#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QIcon>


WikiGraphDialog::WikiGraphDialog(ConllGraph *graph, QWidget *parent) :
    QDialog(parent), mGraph(graph)
{
    //perlProcess = new QProcess(this);
    //connect(perlProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readOnStdOut()));

    te = new QPlainTextEdit(this);
    mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(te);
    this->setLayout(mainLayout);
    this->setMinimumSize(600, 400);
    this->setWindowIcon(QIcon(":/img/app-icon.png"));
    this->setWindowTitle("Dep2Wiki Representation");
}

/*void WikiGraphDialog::readOnStdOut()
{
    QString content = QString::fromUtf8(perlProcess->readAllStandardOutput());
    te->setPlainText(content);
    te->selectAll();
}*/

void WikiGraphDialog::generate()
{
    QString edges  = "[EDGES] {\n";
    QString result = "<format dep2pict>\n[GRAPH] { background=\"lightgreen\"; scale = 200; fontname=\"Arial\"; }\n";
    result += "[WORDS]{\n";
    result += "N_0 { word=\"_\" }\n";
    ConllGraph::ordered_node_iter nb, ne;
    for(boost::tie(nb, ne) = mGraph->orderedNodes(); nb != ne; nb++){
        ConllGraph::Node node = nb->second;
        ConllNodeProperties nProp = mGraph->nodeProperties(node);

        QString token = nProp.token();
        if(token == "\"")
            token = "\\\"";

        result += "N_" + QString::number(nProp.id) + "{ word=\"" + token + "\" }\n";

        ConllGraph::out_edge_iter eb, ee;
        for(boost::tie(eb, ee) = mGraph->outEdges(nb->first); eb != ee; eb++){
            ConllGraph::Edge edge = *eb;
            ConllEdgeProperties eProp = mGraph->edgeProperties(edge);

            QString color = "";
            if(eProp.type() == ConllEdgeProperties::Deep)
                color = "; color=\"blue\"; forecolor=\"blue\"";
            else if(eProp.type() == ConllEdgeProperties::Surfacic)
                color = "; color=\"red\"; forecolor=\"red\"";
            else if(eProp.type() == ConllEdgeProperties::None)
                color = "; color=\"grey\"; forecolor=\"grey\"";

            ConllGraph::Node target = mGraph->targetNode(edge);
            ConllNodeProperties tProp = mGraph->nodeProperties(target);

            QString rel;
            if(eProp.relFinal() == eProp.relInit())
                rel = eProp.relFinal();
            else
                rel = eProp.relFinal() + ":" + eProp.relInit();

            edges += "N_" + QString::number(nProp.id) + " -> N_" + QString::number(tProp.id)
                    + " { label=\"" + rel
                    + "\"" + color + " }\n";
        }

        /*if(nProp.type() == ConllNodeProperties::Dummy){
            edges += "N_0 -> N_" + QString::number(nProp.id) + " { label=\"root\" }\n";
        }*/
    }

    edges += "}\n";
    result += "}\n" + edges + "</format>";

    te->setPlainText(result);
    te->selectAll();
}
