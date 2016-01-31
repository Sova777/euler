/*
 * File:   main.cpp
 * Author: sova
 */

#include <QApplication>
#include "Form.h"

int main(int argc, char *argv[]) {
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);

    QApplication app(argc, argv);
    Form* form = new Form();
    form->show();
    // create and show your widgets here

    return app.exec();
}
