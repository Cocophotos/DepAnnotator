#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setOrganizationName("Cocophotos");
    QApplication::setOrganizationDomain("cocophotos.eu");
    QApplication::setApplicationName("DepAnnotator");

    DependenciesAnnotator w;
    //QObject::connect( &a , SIGNAL( focusChanged( QWidget*, QWidget* ) ) , &w , SLOT( onAppFocusChanged() ) );
    w.show();
    
    return a.exec();
}
