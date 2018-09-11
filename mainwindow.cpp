#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "checklist.h"
#include "observation.h"

#include <QFile>
#include <QDir>
#include <QtAlgorithms>
#include <QDebug>
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
    QString resourceFile = ":/resources/species.txt";
    QFile taxaFile(resourceFile);
    QString temp;
    if(taxaFile.open(QIODevice::ReadOnly))
    {
        while(taxaFile.canReadLine()) {
            //only use lines which correspond to full species
            temp = taxaFile.readLine();
            if(temp.at(0) == '\t' || temp.at(0) == '[') continue;
            taxa.append(temp.split('\t').first());
        }
    }
    else
    {
        //TODO: handle fileNotFound
        qDebug("couldn't open species.txt!");
    }

    //load data file
    QStringList obs;
    resourceFile = "data.txt";
    QFile dataFile(resourceFile);
    if (dataFile.open(QIODevice::ReadOnly))
    {
        //file opened successfully
        temp = dataFile.readLine();
        header = temp.split(',');
        header.takeFirst();
        header.takeAt(1);
        for(int i=0; i<4; i++) header.takeLast();

        //read entire file
        while(!dataFile.atEnd()) {
        //for(int i=0; i<200; i++) {
            //create new checklist if new cid
            temp = dataFile.readLine();
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
        dataFile.close();

        //TODO: sort lists of species (taxa) and locations (alpha)
        //qSort(dSpecies.begin(), dSpecies.end(), compareSpecies);
        //dLocations.keys().sort();

        //populate UI fields
        ui->exploreStartDate->setDate(database.first().getDate());
        ui->exploreEndDate->setDate(database.last().getDate());
        ui->existingLocEntry->addItems(dLocations.keys());
        ui->existingSpeciesEntry->addItems(dSpecies);
        ui->exploreSpecies->addItem("All");
        ui->exploreSpecies->addItems(dSpecies);
        ui->exploreLocations->addItem("All");
        ui->exploreLocations->addItems(dLocations.keys());
    }
    else
    {
        //TODO: handle fileNotFound
        qDebug("couldn't open data.txt!");
    }

    //load states file
    resourceFile = ":/resources/states.txt";
    QFile statesFile(resourceFile);
    if (statesFile.open(QIODevice::ReadOnly))
    {
        for(int i=0; i<51; i++) {
            states.append(statesFile.readLine().simplified());
        }
        //populate UI fields
        ui->submitStates->addItems(states);
        ui->exploreStates->addItems(states);
    }
    else
    {
        //TODO: handle fileNotFound
        qDebug("couldn't open states.txt!");
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
        init.append(ui->startTime->text());
        init.append(ui->endTime->text());
        init.append(getActiveProtocol());
        init.append(QString::number(ui->distanceTraveled->value()));
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
        else {
            activeList.addSpecies(ui->newSpeciesEntry->text(), ui->speciesCount->value());
            //update UI
            dSpecies.append(ui->newSpeciesEntry->text());
            //TODO: sort dSpecies
            ui->exploreSpecies->clear();
            ui->exploreSpecies->addItems(dSpecies);
            ui->existingSpeciesEntry->clear();
            ui->existingSpeciesEntry->addItems(dSpecies);
        }
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

QString MainWindow::getActiveProtocol()
{
    if(ui->rb_incidental->isChecked()) return QString("Incidental");
    if(ui->rb_stationary->isChecked()) return QString("Stationary");
    return QString("Traveling");
}


void MainWindow::on_pb_cancelChecklist_clicked()
{
    //TODO
}

void MainWindow::on_pb_submitChecklist_clicked()
{
    //add checklist to the database
    database.insert(activeList.getID(), activeList);

    //write new checklist data to the file
    QFile file("data.txt");
    if(file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        file.write(activeList.output().toStdString().c_str());
    }
    else {
        //TODO: handle fileNotFound
        qDebug("couldn't open data.txt!");
    }

    //set active list to empty checklist to mark as inactive
    activeList = Checklist();
    //clear database entry forms
    ui->checklistView->clearContents();
    ui->checklistView->setRowCount(0);
}

void MainWindow::on_pb_resetFilter_clicked()
{
    ui->exploreSpecies->setCurrentIndex(0);
    ui->exploreStates->setCurrentIndex(0);
    ui->exploreLocations->setCurrentIndex(0);
    ui->exploreStartDate->setDate(database.first().getDate());
    ui->exploreEndDate->setDate(database.last().getDate());
}

void MainWindow::on_pb_applyFilter_clicked()
{
    sortDatabase();
    ui->exploreResults->setRowCount(0);
    ui->exploreResults->setColumnCount(5);
    ui->exploreResults->setHorizontalHeaderLabels(header);
    //entire database
    if(ui->rb_allSightings->isChecked()) {
        showEntireDatabase();
    }
    //first/last sightings
    else showEntireFirstLast(ui->rb_firstSightings->isChecked());

    ui->exploreResults->resizeColumnsToContents();
    ui->exploreResults->horizontalHeader()->setStretchLastSection(true);
    ui->exploreResults->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->exploreResults->setSelectionMode(QAbstractItemView::NoSelection);
    ui->exploreResults->setFocusPolicy(Qt::NoFocus);
}

void MainWindow::sortDatabase()
{
    QMap<int, Checklist> db;


    //database = db;
}

void MainWindow::showEntireDatabase() {
    int row = 0;
    int column = 0;
    Checklist c;
    QStringList coreData;
    QMapIterator<int, Checklist> lists(database);
    while (lists.hasNext()) {
        lists.next();
        c = lists.value();
        coreData = c.coreData();

        QMapIterator<QString, int> speciesIter(c.getSpecies());
        while(speciesIter.hasNext()) {
            speciesIter.next();
            column = 0;
            ui->exploreResults->insertRow(row);
            ui->exploreResults->setItem(row, column++, new QTableWidgetItem(speciesIter.key()));
            for(int i=0; i<4; i++) {
                ui->exploreResults->setItem(row, column++, new QTableWidgetItem(coreData.at(i)));
            }
            row++;
        }
    }
}

void MainWindow::showEntireFirstLast(bool first)
{
    int row = 0;
    int column = 0;
    Checklist c;
    QStringList coreData;
    QMapIterator<int, Checklist> lists(database);
    QMap<QString, QStringList> species;
    while (lists.hasNext()) {
        lists.next();
        c = lists.value();

        QMapIterator<QString, int> speciesIter(c.getSpecies());
        while(speciesIter.hasNext()) {
            coreData = c.coreData();
            speciesIter.next();
            //if the current species isn't in the map, add it
            if(!species.contains(speciesIter.key())) {
                coreData.insert(0, speciesIter.key());
                species.insert(speciesIter.key(), coreData);
            }
            //otherwise, check if the current species is from an earlier date
            else {
                QStringList curDateSL = species.value(speciesIter.key()).last().split('-');
                QDate curDate = QDate(curDateSL.at(0).toInt(), curDateSL.at(1).toInt(), curDateSL.at(2).toInt());
                QStringList newDateSL = coreData.last().split('-');
                QDate newDate = QDate(newDateSL.at(0).toInt(), newDateSL.at(1).toInt(), newDateSL.at(2).toInt());
                //update coreData if date is older
                if(first) {
                    if(newDate.operator <(curDate)) {
                        coreData.insert(0, speciesIter.key());
                        species.insert(speciesIter.key(), coreData);
                    }
                }
                //update coreData if date is newer
                else {
                    if(newDate.operator >(curDate)) {
                        coreData.insert(0, speciesIter.key());
                        species.insert(speciesIter.key(), coreData);
                    }
                }
            }
        }
    }
    QList<QStringList> records = sortRecords(species.values());
    for(int i=0; i<records.length(); i++) {
        column = 0;
        ui->exploreResults->insertRow(row);
        for(int j=0; j<5; j++) {
            ui->exploreResults->setItem(row, column++, new QTableWidgetItem(records.at(i).at(j)));
        }
        row++;
    }
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
    //int i1 = taxa.indexOf(s1);
    //int i2 = taxa.indexOf(s2);
    //return i1 < i2;
    return (taxa.indexOf(s1) < taxa.indexOf(s2));
}

//TODO: use quicksort
QList<QStringList> MainWindow::sortRecords(QList<QStringList> records)
{
    QList<QStringList> sortedRecords;
    int index;
    //sortedRecords.append(records.at(0));
    for(int i=0; i<records.length(); i++) {
        index = sortedRecords.length() - 1;
        while(index >= 0 && (records.at(i).last() < sortedRecords.at(index).last())) {
            index--;
        }
        sortedRecords.insert(index+1, records.at(i));
    }
    return sortedRecords;
    //TODO: secondary sort by location and species within date brackets
}
