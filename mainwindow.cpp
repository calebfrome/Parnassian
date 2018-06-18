#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "checklist.h"
#include "observation.h"

#include <QFile>
#include <QtAlgorithms>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    char version[128];
    sprintf(version, "Keeping up with the Parnassians v%d.%d", V_MAJOR, V_MINOR);
    this->setWindowTitle(version);

    //load taxonomy file
    QString myResourceFile = ":/resources/taxa.txt";
    QFile f1(myResourceFile);
    QString temp;
    if(f1.open(QIODevice::ReadOnly))
    {
        while(f1.canReadLine()) {
            //only use lines which correspond to full species
            temp = f1.readLine();
            if(temp.at(0) == '\t' || temp.at(0) == '[') continue;
            taxa.append(temp.split('\t').first());
        }
    }
    else
    {
        //TODO: handle fileNotFound
    }

    //load data file
    QStringList obs;
    myResourceFile = ":/resources/data.csv";
    QFile f2(myResourceFile);
    if (f2.open(QIODevice::ReadOnly))
    {
        //file opened successfully
        temp = f2.readLine();
        header = temp.split(',');
        header.takeFirst();
        header.takeAt(1);
        for(int i=0; i<4; i++) header.takeLast();

        //read entire file
        while(f2.canReadLine()) {
            //create new checklist if new cid
            temp = f2.readLine();
            obs = temp.split(',');
            if(obs.at(0).toInt() > currentID) {
                currentID = obs.at(0).toInt();
                Checklist checklist = Checklist(obs);
                database.insert(currentID, checklist);
            }
            //otherwise add to existing checklist
            else {
                database.last().addSpecies(obs.at(1), obs.at(2).toInt());
            }

            //add location to location list
            if(!dLocations.keys().contains(obs.at(3))) dLocations.insert(obs.at(3), QStringList(QStringList() << obs.at(4) << obs.at(5)));
            //add species to species list
            if(!dSpecies.contains(obs.at(1))) dSpecies.append(obs.at(1));
        }
        f2.close();

        //TODO: sort lists of species (taxa) and locations (alpha)
        //qSort(dSpecies.begin(), dSpecies.end(), compareSpecies);
        //dLocations.keys().sort();

        //populate UI fields
        ui->existingLocEntry->addItems(dLocations.keys());
        ui->existingSpeciesEntry->addItems(dSpecies);
        ui->exploreSpecies->addItems(dSpecies);
        ui->exploreLocations->addItems(dLocations.keys());
    }
    else
    {
        //TODO: handle fileNotFound
    }

    //load states file
    myResourceFile = ":/resources/states.txt";
    QFile f3(myResourceFile);
    if (f3.open(QIODevice::ReadOnly))
    {
        for(int i=0; i<50; i++) {
            states.append(f3.readLine().simplified());
        }
        //populate UI fields
        ui->submitStates->addItems(states);
        ui->exploreStates->addItems(states);
    }
    else
    {
        //TODO: handle fileNotFound
    }

    //setup
    activeList = Checklist();


    //ui setup
    //ui->exploreResults->verticalHeader()->setVisible(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_addSpecies_clicked()
{
    //create checklist if there is no active list
    if(activeList.getID() == -1) {
        QStringList init = QStringList();
        init.append(QString::number(++currentID)); //ID
        //species name
        if(ui->rb_existingSpecies->isChecked()) {
            init.append(ui->existingSpeciesEntry->currentText());
        }
        else {
            init.append(ui->newSpeciesEntry->text());
            //update UI
            dSpecies.append(ui->newSpeciesEntry->text());
            //TODO: sort dSpecies
            ui->exploreSpecies->clear();
            ui->exploreSpecies->addItems(dSpecies);
            ui->existingSpeciesEntry->clear();
            ui->existingSpeciesEntry->addItems(dSpecies);
        }
        init.append(QString::number(ui->speciesCount->value())); //species count
        if(ui->rb_existingLoc->isChecked()) {
            init.append(ui->existingLocEntry->currentText());
            init.append(dLocations.value(ui->existingLocEntry->currentText()).at(0)); //county
            init.append(dLocations.value(ui->existingLocEntry->currentText()).at(1)); //state
        }
        else {
            init.append(ui->newLocEntry->text());
            init.append(ui->submitCounties->text());
            init.append(ui->submitStates->currentText());
            dLocations.insert(ui->newLocEntry->text(), QStringList(QStringList() << ui->submitCounties->text() << ui->submitStates->currentText()));
            //update existing location entry values
            ui->existingLocEntry->clear();
            ui->existingLocEntry->addItems(dLocations.keys());
            //update explore location values
            ui->exploreLocations->clear();
            ui->exploreLocations->addItems(dLocations.keys());
        }
        init.append(ui->calendarWidget->selectedDate().toString("MM/dd/yyyy"));
        activeList = Checklist(init);
        //setup checklistView
        ui->checklistView->setRowCount(0);
        ui->checklistView->setColumnCount(2);
        ui->checklistView->setHorizontalHeaderLabels(QStringList(QStringList() << "Species" << "Count"));
    }
    //add species to checklist set
    else {
        if(ui->rb_existingSpecies->isChecked()) {
            activeList.addSpecies(ui->existingSpeciesEntry->currentText(), ui->speciesCount->value());
        }
        else activeList.addSpecies(ui->newSpeciesEntry->text(), ui->speciesCount->value());
    }
    //update UI
    int row = activeList.numSpecies() - 1;
    ui->checklistView->insertRow(row);
    if(ui->rb_existingSpecies->isChecked()) {
        ui->checklistView->setItem(row, 0, new QTableWidgetItem(ui->existingSpeciesEntry->currentText()));
    }
    else {
        ui->checklistView->setItem(row, 0, new QTableWidgetItem(ui->newSpeciesEntry->text()));
    }
    ui->checklistView->setItem(row, 1, new QTableWidgetItem(QString::number(ui->speciesCount->value())));
    ui->checklistView->resizeColumnsToContents();
    //ui->checklistView->horizontalHeader()->setStretchLastSection(true);
    ui->checklistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->checklistView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->checklistView->setFocusPolicy(Qt::NoFocus);
}

void MainWindow::on_pb_submitChecklist_clicked()
{
    //add checklist to the database
    database.insert(activeList.getID(), activeList);
    //set active list to empty checklist to mark as inactive
    activeList = Checklist();
    //clear database entry forms
    //ui->checklistView->clear();
    ui->checklistView->clearContents();
    //TODO: write new checklist data to the file
}

void MainWindow::on_pb_search_clicked()
{

}

void MainWindow::on_pb_showAll_clicked()
{
    int row = 0;
    int column = 0;
    Checklist c;
    QStringList coreData;
    QMapIterator<int, Checklist> lists(database);
    ui->exploreResults->setRowCount(0);
    ui->exploreResults->setColumnCount(5);
    ui->exploreResults->setHorizontalHeaderLabels(header);
    while (lists.hasNext()) {
        lists.next();
        c = lists.value();
        coreData = c.coreData();

        QMapIterator<QString, int> species(c.getSpecies());
        while(species.hasNext()) {
            species.next();
            column = 0;
            ui->exploreResults->insertRow(row);
            ui->exploreResults->setItem(row, column++, new QTableWidgetItem(species.key()));
            for(int i=0; i<4; i++) {
                ui->exploreResults->setItem(row, column++, new QTableWidgetItem(coreData.at(i)));
            }
            row++;
        }
    }
    ui->exploreResults->resizeColumnsToContents();
    ui->exploreResults->horizontalHeader()->setStretchLastSection(true);
    ui->exploreResults->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->exploreResults->setSelectionMode(QAbstractItemView::NoSelection);
    ui->exploreResults->setFocusPolicy(Qt::NoFocus);
}

void MainWindow::printDatabase()
{
    QMapIterator<int, Checklist> i(database);
    while (i.hasNext()) {
        i.next();
        ((Checklist)i.value()).printSpecies();
        std::cout << std::endl;
    }
}

bool MainWindow::compareSpecies(const QString &s1, const QString &s2)
{
    int i1 = taxa.indexOf(s1);
    int i2 = taxa.indexOf(s2);
    return i1 < i2;
    //return (taxa.indexOf(s1) < taxa.indexOf(s2));
}
