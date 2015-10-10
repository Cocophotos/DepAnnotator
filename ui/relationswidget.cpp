#include "relationswidget.h"

#include <QtGui>
#include <QVBoxLayout>


RelationsWidget::RelationsWidget(QWidget *parent) :
    QWidget(parent)
{
    relationsTree = new QTreeWidget(this);
    reader = SchemeReader::schemeReader();

    QVBoxLayout *main = new QVBoxLayout(this);
    main->addWidget(relationsTree);
}

void RelationsWidget::populate(QString filepath)
{
    relationsTree->clear();

/*    relationsBoth << "_"
                  << "suj"
                  << "obj"
                  << "de_obj"
                  << "a_obj"
                  << "p_obj_loc"
                  << "p_obj_dloc"
                  << "p_obj"
                  << "ats"
                  << "ato"
                  << "mod_loc"
                  << "mod_cleft"
                  << "mod"
                  << "ponct"
                  << "dep_coord"
                  << "coord"
                  << "mod_rel"
                  << "arg"
                  << "det"
                  << "dep"
                  << "arg_comp"
                  << "arg_cons";
*/

//    relationsInit << "arg_caus";
/*    relationsFinal << "aux_tps"
                   << "aux_pass"
                   << "aux_caus"
                   << "aff_moyen"
                   << "aff_neutre"
                   << "aff_secaus"
                   << "aff"
                   << "dis"
                   << "p_obj_agt";
*/

/*    relationsFinal << "_"
                   << "a_obj"
                   << "aff"
                   << "aff_moyen"
                   << "aff_neutre"
                   << "aff_secaus"
                   << "arg"
                   << "arg_comp"
                   << "arg_cons"
                   << "ato"
                   << "ats"
                   << "aux_caus"
                   << "aux_pass"
                   << "aux_tps"
                   << "coord"
                   << "de_obj"
                   << "dep"
                   << "dep_coord"
                   << "det"
                   << "dis"
                   << "mod"
                   << "mod_cleft"
                   << "mod_loc"
                   << "mod_rel"
                   << "obj"
                   << "p_obj"
                   << "p_obj_agt"
                   << "p_obj_dloc"
                   << "p_obj_loc"
                   << "ponct"
                   << "suj";

    relationsInit << "_"
                  << "a_obj"
                  << "aff"
                  << "aff_moyen"
                  << "aff_neutre"
                  << "aff_secaus"
                  << "arg"
                  << "arg_caus"
                  << "arg_comp"
                  << "arg_cons"
                  << "ato"
                  << "ats"
                  << "aux_caus"
                  << "aux_pass"
                  << "aux_tps"
                  << "coord"
                  << "de_obj"
                  << "dep"
                  << "dep_coord"
                  << "det"
                  << "dis"
                  << "mod"
                  << "mod_cleft"
                  << "mod_loc"
                  << "mod_rel"
                  << "obj"
                  << "p_obj"
                  << "p_obj_agt"
                  << "p_obj_dloc"
                  << "p_obj_loc"
                  << "ponct"
                  << "suj";   */

    reader->read(filepath);


    QMap<QString, QString> fins = reader->finals();

    if(!reader->initials().isEmpty()){
        QMap<QString, QString> inits = reader->initials();

        QTreeWidgetItem *parentInit = new QTreeWidgetItem(QStringList() << "Canonique");
        QMapIterator<QString, QString> iit(inits);
        while(iit.hasNext()){
            iit.next();
            QTreeWidgetItem *i = new QTreeWidgetItem(QStringList() << iit.key());
            i->setToolTip(0, iit.value());
            parentInit->addChild(i);
        }

        QTreeWidgetItem *parentFinal = new QTreeWidgetItem(QStringList() << "Final");
        QMapIterator<QString, QString> fit(fins);
        while(fit.hasNext()){
            fit.next();
            QTreeWidgetItem *i = new QTreeWidgetItem(QStringList() << fit.key());
            i->setToolTip(0, fit.value());
            parentFinal->addChild(i);
        }
        relationsTree->insertTopLevelItems(0, QList<QTreeWidgetItem*>() << parentInit
                                           << parentFinal);
        parentInit->setExpanded(true);
        parentFinal->setExpanded(true);
    }else{
        QList<QTreeWidgetItem*> items;
        QMapIterator<QString, QString> fit(fins);
        while(fit.hasNext()){
            fit.next();
            QTreeWidgetItem *i = new QTreeWidgetItem(QStringList() << fit.key());
            i->setToolTip(0, fit.value());
            items << i;
        }
        relationsTree->insertTopLevelItems(0, items);
    }
}
