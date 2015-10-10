#include "conllproject.h"
#include "scheme/schemereader.h"

#include <QStringBuilder>
#include <QtGui>

bool sortByIndexes(const QPair<double, QString> &i1, const QPair<double, QString> &i2)
{
    return i1.first <= i2.first;
}

ConllProject::ConllProject(QString name, QString path)
    :Project(name, path){}

bool ConllProject::addFile(QString filepath)
{
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Unable to open the file";
        return false;
    }

    QList<CompleteAnnotationItem*> items;
    unsigned id = 0;
    QString acc = "";
    while(!file.atEnd())
    {
        QString line = QString::fromUtf8(file.readLine());
        if(line.trimmed() != "" && line.trimmed() != "\n")
            acc += line;
        else
        {
            if(!acc.isEmpty()){

                ConllGraph *g = new ConllGraph();
                if(!generateGraph(acc, g))
                    qDebug() << "Error during graph's creation";
                acc.clear();
                items << new CompleteAnnotationItem(id,
                                                    new AnnotationItem(id,
                                                                       new ConllGraph(*g),
                                                                       new ConllGraph(*g)),
                                                    RevisionAnnotationList()
                                                    );
                id++;
                delete g;
            }
        }
    }

    if(!acc.isEmpty()){
        ConllGraph *g = new ConllGraph();
        if(!generateGraph(acc, g))
            qDebug() << "Error during graph's creation";
        acc.clear();
        items << new CompleteAnnotationItem(id,
                                            new AnnotationItem(id,
                                                               new ConllGraph(*g),
                                                               new ConllGraph(*g)),
                                            RevisionAnnotationList()
                                            );
        delete g;
    }

    container << qMakePair(QFileInfo(filepath).fileName(), items);
    return true;
}

bool ConllProject::saveProject()
{
    QFile file(mProjectPath+QDir::separator()+mProjectName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
	out.setCodec("UTF-8");
	out.setGenerateByteOrderMark(true);

    foreach(StringAndAnnotations file_annotations, container){
        QString filename = file_annotations.first;
        QList<CompleteAnnotationItem*> annotations = file_annotations.second;

        out << QString("#File:"+filename+"\n");

        foreach(CompleteAnnotationItem* annotation, annotations){
            for(int r = 0; r < annotation->getRevisionsCount(); r++)
                saveRevision(annotation->getRevision(r), out);
            saveAnnotation(annotation->getItem(), out);            
        }
    }

    file.close();
    return true;
}

void ConllProject::saveAnnotation(AnnotationItem *annotation, QTextStream &out)
{
    out << QString("#Id:"+QString::number(annotation->getId())+"\n");
    out << QString("#Status:"+QString::number((int)annotation->status())+"\n");
    out << generateConll(annotation->getCurrentParse());
}

void ConllProject::saveRevision(Revision<AnnotationItem*> revision, QTextStream &out)
{
    out << "#Revision:" << revision.getTimestamp().toString(Qt::ISODate) << "\n";
    saveAnnotation(revision.getValue(), out);
}

bool ConllProject::exportProject(QString path)
{        
    foreach(StringAndAnnotations file_annotations, container){
        QString filename = file_annotations.first;
        QList<CompleteAnnotationItem*> annotations = file_annotations.second;

        QFile file(path+QDir::separator()+filename);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
            continue;
        QTextStream out(&file);
		out.setCodec("UTF-8");
		out.setGenerateByteOrderMark(true);

        foreach(CompleteAnnotationItem* annotation, annotations){
            out << generateConll(annotation->getItem()->getCurrentParse()) << "\n";
        }
        file.close();
    }
    return true;
}

bool ConllProject::openProject()
{
    QFile file(mProjectPath+QDir::separator()+mProjectName);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QList<CompleteAnnotationItem*> items;
    RevisionAnnotationList revisions;
    QString revisionTimestamp = "";
    QString filename = "";
    originalParse = NULL;
    unsigned id = 0;
    AnnotationItem::Status status = AnnotationItem::NotStarted;


    QString acc = "";

    while(!file.atEnd())
    {
        QString line = QString::fromUtf8(file.readLine());
        if(!line.trimmed().startsWith("#"))
            acc += line;
        else
        {
            QRegExp rx("#(File|Id|Revision|Status):(.*)$", Qt::CaseInsensitive);
            if(rx.indexIn(line) != -1)
            {
                QStringList captured = rx.capturedTexts();
                assert(captured.size() == 3);

                if(captured[1] == "Revision"){
                    processDap(id, acc, revisionTimestamp, items, revisions);
                    acc.clear();
                    revisionTimestamp = captured[2];

                }else if(captured[1] == "Status"){
                    switch(captured[2].toInt()){
                        case 0: status = AnnotationItem::NotStarted; break;
                        case 1: status = AnnotationItem::InProgress; break;
                        case 2: status = AnnotationItem::Finished; break;
                        default: status = AnnotationItem::NotStarted;
                    }
                }else if(captured[1] == "Id"){
                    processDap(id, acc, revisionTimestamp, items, revisions);
                    acc.clear();
                    id = captured[2].toUInt();

                }else if(captured[1] == "File"){
                    filename = captured[2].trimmed();

                    if(items.size() > 0){
                        processDap(id, acc, revisionTimestamp, items, revisions);
                        acc.clear();
                        container << qMakePair(QFileInfo(filename).fileName(), items);

                        items.clear();
                        revisions.clear();
                        revisionTimestamp = "";
                        delete originalParse;
                        originalParse = NULL;
                        id = 0;
                        filename = "";
                    }
                }
            }
        }
    }

    if(items.size() > 0 || !acc.isEmpty()){
        processDap(id, acc, revisionTimestamp, items, revisions);
        acc.clear();
        container << qMakePair(QFileInfo(filename).fileName(), items);
    }
    delete originalParse;
    return true;
}

void ConllProject::processDap(unsigned id, QString conll, QString &revisionTimestamp,
                              QList<CompleteAnnotationItem *> &items,
                              RevisionAnnotationList &revisions)
{
    if(!conll.isEmpty()){ //I read something
        ConllGraph *g = new ConllGraph();
        generateGraph(conll, g);

        //I need to determine what it is
        if(revisionTimestamp.isEmpty()){ //It's a sentence (so basic AnnotationItem)

            if(!originalParse)
                originalParse = new ConllGraph(*g);
            AnnotationItem *item = new AnnotationItem(id, new ConllGraph(*originalParse),
                                                      new ConllGraph(*g));
            items << new CompleteAnnotationItem(id, item, revisions);

            revisions.clear();
            delete originalParse;
            originalParse = NULL;

        }else{ //It's a revision

            if(revisions.isEmpty()) //No revision, so it's the original parse
                originalParse = new ConllGraph(*g);

            revisions << Revision<AnnotationItem*>(new AnnotationItem(id,new ConllGraph(*originalParse),
                                                                      new ConllGraph(*g)),
                                                   QDateTime::fromString(revisionTimestamp, Qt::ISODate));
            revisionTimestamp = "";
        }
    }
}

/////////////////////////////

QString ConllProject::generateConll(ConllGraph *graph)
{
    unsigned nodeCounter = 0;
    unsigned enodeCounter = 0;

    QMap<unsigned, QString> id2original;
    QStringList lines;

    ConllGraph::ordered_node_iter nb, ne;
    for(boost::tie(nb, ne) = graph->orderedNodes(); nb != ne; ++nb){
        ConllNodeProperties nodeProp = graph->nodeProperties(nb->second);

        if(nodeProp.type() == ConllNodeProperties::Dummy){
            id2original.insert(nodeProp.id, QString::number(nodeProp.id));
            continue;
        }

        QString line;
        QString id = "";

        if(nodeProp.type() == ConllNodeProperties::Unrealised){
            ++enodeCounter;
            id = QString::number(nodeCounter) + "." + QString::number(enodeCounter);
        }else{
            ++nodeCounter;
            id = QString::number(nodeCounter);
        }
        id2original.insert(nodeProp.id, id);

        line += id % "\t" % nodeProp.token()
            %"\t" % nodeProp.lemma() % "\t" % nodeProp.hyperpos()
            % "\t" % nodeProp.pos() % "\t";

        ConllNodeProperties::const_feature_iter fb, fe;
        QStringList features;
        for(boost::tie(fb, fe) = nodeProp.featuresIterator(); fb != fe; ++fb){
            features << fb.key() + "=" + fb.value();
        }

        if(features.size() == 0)
            line += "_\t";
        else
            line += features.join("|") % "\t";               
        lines << line;
    }

    unsigned counter = 0;
    QString string;
    bool first = true;
    for(boost::tie(nb, ne) = graph->orderedNodes(); nb != ne; ++nb){

        ConllNodeProperties nodeProp = graph->nodeProperties(nb->second);
        if(nodeProp.type() == ConllNodeProperties::Dummy)
            continue;

        ConllGraph::in_edge_iter eb, ee;
        QList<QPair<double, QString> > relations;
        double originIndex = -1.0;
        QString originRelation = "";

        for(boost::tie(eb, ee) = graph->inEdges(nb->first); eb != ee; ++eb){
            ConllGraph::Edge e = *eb;
            ConllGraph::Node src = graph->sourceNode(e);
            unsigned src_id = graph->nodeProperties(src).id;
            ConllEdgeProperties edgeProp = graph->edgeProperties(e);

            QString relation;
            if(edgeProp.relFinal() == edgeProp.relInit())
            {
                if(SchemeReader::schemeReader()->doubledRels().contains(edgeProp.relFinal()))
                    relation = edgeProp.relFinal() + ":" + edgeProp.relInit();
                else
                    relation = edgeProp.relFinal();
            }
            else
                relation = edgeProp.relFinal() + ":" + edgeProp.relInit();

            if(edgeProp.type() == ConllEdgeProperties::Surfacic)
                relation = "S:" + relation;
            else if(edgeProp.type() == ConllEdgeProperties::Deep)
                relation = "D:" + relation;
            else if(edgeProp.type() == ConllEdgeProperties::None)
                relation = "I:" + relation;

            if(edgeProp.original())
            {
                originIndex = id2original.value(src_id).toDouble();
                originRelation = relation;
            }
            else
                relations << qMakePair( id2original.value(src_id).toDouble(), relation );
        }

        qSort(relations.begin(), relations.end(), sortByIndexes);

        if(originIndex > -1.0)
            relations.prepend(qMakePair(originIndex, originRelation));

        QString indexes = "", rels = "";
        for(int i = 0; i < relations.size(); i++)
        {
            if(i){
                indexes += "|";
                rels += "|";
            }

            //Keep 24.1 for unrealised node,
            //but put 25 for normal node instead of
            // 25.0
            QString index = "";
            if(relations[i].first - (int)relations[i].first == 0)
                index = QString::number((int)relations[i].first);
            else
                index = QString::number(relations[i].first);

            indexes += index;
            rels += relations[i].second;
        }

        string += lines[counter++];                
        string += indexes % "\t" % rels % "\t" % "_" % "\t" % "_";

        if(first && !graph->nodeProperties(nb->first).comment().isEmpty())
            string += "\t" + ConllProject::normalise(graph->nodeProperties(nb->first).comment()) + "\n";
        else
            string += "\n";
        first = false;
    }

    return string;
}

bool ConllProject::generateGraph(QString conll, ConllGraph *g)
{
    if(!g)
        g = new ConllGraph();

    QStringList lines = conll.split('\n', QString::SkipEmptyParts);
    //unsigned nodeCounter = 0;
    QMap<QString, unsigned> original2id;
    QMultiMap<QString, QPair<QString, ConllEdgeProperties> > edges;

    bool first = true;
    foreach(QString line, lines){
        QStringList items = line.split('\t',QString::KeepEmptyParts);

        qDebug() << items;

        if(items.size() == 6){
            items << "" << "";
        }

        if(items.size() >= 8)
        {
            QMap<QString, QString> features = processFeatures(items[5]);

            //nodeCounter++;

            int idx = -1;
            bool elliptic = false;
            if(items[0].contains(QRegExp("\\.[0-9]+"))){
                elliptic = true;
                float _idx = items[0].toFloat();
                idx = _idx*10;
            }else{
                idx = items[0].toInt();
            }

            original2id.insert(items[0], 10*idx);

            ConllNodeProperties prop(10*idx, items[1],
                    items[2], items[3], items[4], features);

            if(elliptic)
                prop.setType(ConllNodeProperties::Unrealised);


            //Add dummy node for root attachment
            QSet<QString> indexes = items[6].split('|').toSet();
            if(indexes.contains("0"))
            {
                ConllNodeProperties propDummy(0,"__", "__", "X", "X",
                                              QMap<QString, QString>(),
                                              ConllNodeProperties::Dummy);
                g->addNode(propDummy.id, propDummy);
                original2id.insert(0, 0);
            }

            g->addNode(prop.id, prop);

            if(!processEdge(items[0], items[6], items[7], edges)){
                LogConsole::console()->addLine("Unable to process edges for line : " + line, LogConsole::Critical);
                return false;
            }

            /*if(first && items.size() > 10)
                g->nodeProperties(prop.id).setComment(ConllProject::denormalise(items[10]));*/
            first = false;
        }else{
            LogConsole::console()->addLine("Number of items is less than 8 : " + line, LogConsole::Critical);
            return false;
        }
    }


    if(edges.size() > 0){
        typedef QMultiMap<QString, QPair<QString, ConllEdgeProperties> > QEdgesMultiMap;
        for(QEdgesMultiMap::const_iterator it = edges.constBegin(); it != edges.constEnd(); ++it)
        {
            g->addEdge(original2id.value( it.key() ), original2id.value( it.value().first ), it.value().second );
        }
    }
    return true;
}

QMap<QString, QString> ConllProject::processFeatures(QString features)
{
    QMap<QString, QString> featuresHash;
    QStringList feat = features.split("|");
    foreach(QString f, feat){
        QStringList key_value = f.split("=");
        if(key_value.size() != 2)
            continue;
        featuresHash.insert(key_value[0], key_value[1]);
    }
    return featuresHash;
}


bool ConllProject::processEdge(QString id, QString heads, QString rels, QMultiMap<QString,
                 QPair<QString, ConllEdgeProperties> > &edges)
{
    QStringList indexes   = heads.split("|", QString::SkipEmptyParts);
    QStringList relations = rels.split("|", QString::SkipEmptyParts);

    //LogConsole::console()->addLine(QString::number(indexes.size()) + "|" + QString::number(relations.size()),
    //                               LogConsole::Info);

    if(indexes.size() != relations.size()){
        LogConsole::console()->addLine(heads + " and " + rels + " don't have the same size.", LogConsole::Critical);
        return false;
    }

    if(indexes.empty())
        return true;

    for(int i = 0; i < indexes.size(); i++){        
        QString index = indexes[i];

        /*//Root case
        if(index.toUInt() == 0)
            continue;*/

        //Handle initial and final functions here
        ConllEdgeProperties edgeProp(edges.size(), "");
        edgeProp.setOriginal(!i);

        QString relation = relations[i];
        if(relation.startsWith("S:")){ //Surfacic only relation
            relation = relation.remove(0, 2);
            edgeProp.setDependencyType(ConllEdgeProperties::Surfacic);
        }else if(relation.startsWith("D:")){ //Deep only relation
            relation = relation.remove(0, 2);
            edgeProp.setDependencyType(ConllEdgeProperties::Deep);
        }else if(relation.startsWith("I:")){ //None relation
            relation = relation.remove(0, 2);
            edgeProp.setDependencyType(ConllEdgeProperties::None);
        }else{ //Both relation
            edgeProp.setDependencyType(ConllEdgeProperties::Both);
        }

        QStringList final_init = relation.split(":");
        assert(final_init.size() == 1 || final_init.size() == 2);
        edgeProp.setRelFinal(final_init[0]);
        if(final_init.size() == 2)
            edgeProp.setRelInit(final_init[1]);
        else
            edgeProp.setRelInit(final_init[0]);

        edges.insert(index, qMakePair(id, edgeProp));
    }
    return true;
}

////////////////////////////////////

QString ConllProject::normalise(QString string)
{
    string = string.trimmed();
    string = string.replace("\n", "\\n");
    string = string.replace("\t", "\\t");

    return string;
}

QString ConllProject::denormalise(QString string)
{
    string = string.trimmed();
    string = string.replace("\\n", "\n");
    string = string.replace("\\t", "\t");

    return string;
}

