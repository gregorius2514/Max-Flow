#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QTextStream"
#include "QString"
#include "QtCore"
#include "QtGui"
#include "QPainter"
#include "QPaintEvent"
#include "list"
#include "string"
#include "QtXml"
#include "QFileDialog"

const int ECLIPSE_WIDTH = 50;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  scene = new QGraphicsScene();
  ui->graphicsView->setScene(scene);

  add_source();
}

MainWindow::~MainWindow()
{
  delete ui;
}

// add node button
void MainWindow::on_pushButton_2_released()
{
  wierzcholek wK, tmpW;
  krawedz krawedz_w;
  QString text = ui->lineEdit->text();



  if ( text.length() > 0) {
	//sprawdzanie obecnego wierzcholka
	get_node(text, wK);
	if(wK.nazwa.length() != 0){
	  krawedz_w.pojemnosc = ui->spinBox->value();
	  krawedz_w.przeplyw = 0;
	  krawedz_w.zbadano = 0;
	  krawedz_w.cel = text;
	  update_node(ui->comboBox->currentText(), krawedz_w);
	  return;
	} else {

	  //dodanie wierzcholka do listy
	  ui->comboBox->addItem(text);

	  wK.nazwa = text;
	  krawedz_w.pojemnosc = ui->spinBox->value();
	  krawedz_w.przeplyw = 0;
	  krawedz_w.zbadano = 0;
	  krawedz_w.cel = text;

	  //jesli poprzednikiem jest zrodlo
	  if( ui->comboBox->count() == 1 ) {
		wK.x = 0;
		wK.y = 0;
	  } else {
		get_node(ui->comboBox->currentText(), tmpW);
		wK.x = tmpW.x;
		wK.y = tmpW.y;
		check_node_position(wK, tmpW.krawedzie.size(), 1);
	  }
	  update_node(ui->comboBox->currentText(), krawedz_w);
	  wierzcholki.push_back(wK);
	}
  }
}


// calculating button
void MainWindow::on_pushButton_released()
{
  wierzcholek outlet;
  add_outlet(outlet);
  max_przeplyw();
}

void MainWindow::on_spinBox_valueChanged(int arg1){}
void MainWindow::on_comboBox_activated(const QString &arg1){}
void MainWindow::on_lineEdit_lostFocus(){}

void MainWindow::get_node (QString name, wierzcholek &w)
{
  for (std::list<wierzcholek>::iterator itr = wierzcholki.begin(); itr != wierzcholki.end(); itr++){
	if( itr->nazwa == name ) {
	  w = *itr;
	  break;
	}
  }
}

void MainWindow::update_node (QString name, krawedz kr)
{
  for (std::list<wierzcholek>::iterator itr = wierzcholki.begin(); itr != wierzcholki.end(); itr++){
	if( itr->nazwa == name ) {
	  itr->krawedzie.push_back(kr);
	  break;
	}
  }
}

void MainWindow::check_node_position (wierzcholek &w, int nr_of_node, int message)
{
  if( message == 1) {
	// wierzcholek ma 1 potomka -- obecnie dodawany/badany wierzcholek
	w.x += 2*ECLIPSE_WIDTH;
  } else if( message == 1){
    //gorny element
    w.x += 2*ECLIPSE_WIDTH;
    w.y += 2*ECLIPSE_WIDTH;
  } else if( message == 1) {
    //dolny element: 2,4,6,...
    w.x += 2*ECLIPSE_WIDTH;
    w.y -= 2*ECLIPSE_WIDTH;
  }

  message++;

  for (std::list<wierzcholek>::iterator itr = wierzcholki.begin(); itr != wierzcholki.end(); itr++){
	if( itr->x == w.x && itr->y == w.y ) {
	  if(message > 1){
		if(nr_of_node % 2 == 0) {
		  w.y -= 2*ECLIPSE_WIDTH;
		} else if(nr_of_node % 1 == 0) {
		  w.y += 2*ECLIPSE_WIDTH;
		} else {
		  QTextStream (stdout) << "Blad przydzielania wierzcholkowi miejsca" << endl;
		}
		check_node_position(w, nr_of_node, message);
	  }
	}


  }
}

//zapisuje do referencji (parametru) liste wierzcholkow bez krawedzi
void MainWindow::find_node_without_child(std::list<wierzcholek> &tmp)
{
  for (std::list<wierzcholek>::iterator itr = wierzcholki.begin(); itr != wierzcholki.end(); itr++){
	// moze byc == rowne zeru '0'
	if(itr->krawedzie.size() <= 0){
	  tmp.push_back(*itr);
	}
  }
}

void MainWindow::add_outlet(wierzcholek &tmp)
{
  // wspolrzednie x, y ujscia
  // count jest uzywane do obliczenia sredniej odleglosci miedzy punktami w pionie
  std::list<wierzcholek> tt;
  wierzcholek nastepnik;
  krawedz krawedz_w;
  int valueY = 0, valueX = 0, count = 0;

  find_node_without_child(tt);

  for (std::list<wierzcholek>::iterator itr = tt.begin(); itr != tt.end(); itr++){
	if( itr->x > valueX ) {
	  valueX = itr->x;
	}
	valueY += itr->y;
	count++;
  }

  valueY = valueY / count;

  if(valueX == 0) {
	valueX = 50;
  }
  tmp.nazwa = "ujscie";
  tmp.x = valueX + valueX;
  tmp.y = valueY;  
  wierzcholki.push_back(tmp);

  krawedz_w.cel = tmp.nazwa;
  krawedz_w.pojemnosc = INT_MAX;
  krawedz_w.przeplyw = -1;
  krawedz_w.zbadano = 2;

  for (std::list<wierzcholek>::iterator itr2 = tt.begin(); itr2 != tt.end(); itr2++){
	update_node(itr2->nazwa, krawedz_w);
  }
}

void MainWindow::paintEvent(QPaintEvent *e)
{
  wierzcholek tmp;
  QPen blackPen(Qt::black);
  blackPen.setWidth(1);

  int x_src, x_dst, y_src, y_dst;
  x_src = x_dst = y_src = y_dst = 0;

  for(std::list<wierzcholek>::iterator itr = wierzcholki.begin(); itr != wierzcholki.end(); itr++) {

	scene->addEllipse(itr->x, itr->y, ECLIPSE_WIDTH, ECLIPSE_WIDTH, blackPen);

	QGraphicsTextItem *io = new QGraphicsTextItem;
	io->setPos(itr->x + ECLIPSE_WIDTH/4 - itr->nazwa.length(), itr->y + ECLIPSE_WIDTH/3);
	io->setPlainText(itr->nazwa);
	scene->addItem(io);

	for(std::list<krawedz>::iterator itr2 = itr->krawedzie.begin(); itr2 != itr->krawedzie.end(); itr2++){
	  get_node(itr2->cel, tmp);
	  x_src = itr->x+ECLIPSE_WIDTH;
	  y_src = itr->y+ECLIPSE_WIDTH/2;
	  x_dst = tmp.x;
	  y_dst = tmp.y+ECLIPSE_WIDTH/2;

      if(itr2->cel != "ujscie") {
          QGraphicsTextItem *number = new QGraphicsTextItem;
          number->setPos((x_src + x_dst)/2, (y_src + y_dst)/2);
          number->setPlainText(QString::number(itr2->pojemnosc));
          scene->addItem(number);
      }

	  scene->addLine(x_src, y_src, x_dst, y_dst,  blackPen);
	}
  }
}

void MainWindow::open_file()
{
  //czyszczenie listy
  wierzcholki.clear();
  ui->comboBox->clear();


  //istnieje prawdopodobienstwo przejmowania wartosci struktury
  // po poprzednim elemencie gdy wartosc obecnego elementu jest nieznana
  wierzcholek tmp_w;
  QDomDocument xml_document;
  QString filename = "";

  filename = QFileDialog::getOpenFileName(this, tr("Open File"),
	  ".",
	  tr("XML files (*.xml)"));

  QFile file(filename);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
	QTextStream(stdout) << "Blad otwarcia pliku";
	return;

  } else {
	if(!xml_document.setContent(&file)) {
	  QTextStream(stdout) << "Blad odczytu";
	}
	file.close();
  }

  QDomElement root = xml_document.firstChildElement();

  QDomNodeList items = root.elementsByTagName("wierzcholek");

  for(int i = 0; i < items.count(); i++) {
	QDomElement item_element = items.at(i).toElement();

	if(item_element.tagName() == "wierzcholek") {

	  QDomNodeList iitems = item_element.childNodes();
	  for( int j=0; j < iitems.count(); j++){

		QDomElement child_element = iitems.at(j).toElement();

		if( !child_element.isNull() && child_element.tagName() == "nazwa") {
		  tmp_w.nazwa = child_element.text();
		}                  
		if(!child_element.isNull() && child_element.tagName() == "x" ) {
		  tmp_w.x = child_element.text().toInt();
		}
		if( !child_element.isNull() &&child_element.tagName() == "y" ) {
		  tmp_w.y = child_element.text().toInt();
		}                 

		if( !child_element.isNull() &&child_element.tagName() == "krawedzie" ) {

		  QDomNodeList l_krawedzie = child_element.childNodes();
		  for(int k=0; k < l_krawedzie.count(); k++) {
			krawedz krawedz_w;

			QDomElement krawedzie_element = l_krawedzie.at(k).toElement();
			if(krawedzie_element.tagName() == "krawedz"){

			  QDomNodeList k_krawedz = krawedzie_element.childNodes();
			  for(int kk = 0; kk < k_krawedz.count(); kk++) {
				QDomElement kk_krawedz = k_krawedz.at(kk).toElement();

				if( !kk_krawedz.isNull() && kk_krawedz.tagName() == "pojemnosc" ) {
				  krawedz_w.pojemnosc = kk_krawedz.text().toInt();
				}
				if( !kk_krawedz.isNull() && kk_krawedz.tagName() == "przeplyw" ) {
				  krawedz_w.przeplyw = kk_krawedz.text().toInt();
				}
				if( !kk_krawedz.isNull() &&kk_krawedz.tagName() == "zbadano" ) {
				  krawedz_w.zbadano = kk_krawedz.text().toInt();
				}
				if( !kk_krawedz.isNull() &&kk_krawedz.tagName() == "cel" ) {
				  krawedz_w.cel = kk_krawedz.text();
				}                             
			  }
			  tmp_w.krawedzie.push_back(krawedz_w);
			}
		  }
		}
	  }
	  //dodanie wierzcholka do listy
	  wierzcholki.push_back(tmp_w);
	  ui->comboBox->addItem(tmp_w.nazwa);
	  tmp_w.krawedzie.clear();
	}
  }
}


void MainWindow::save_file()
{
  QString filename = "";
  QDomDocument xml_document;
  QDomElement root = xml_document.createElement("Baza");
  xml_document.appendChild(root);

  for(std::list<wierzcholek>::iterator itr = wierzcholki.begin(); itr != wierzcholki.end(); itr++) {
	//wierzcholek
	QDomElement wierzcholek = xml_document.createElement("wierzcholek");
	root.appendChild(wierzcholek);

	//nazwa
	QDomElement nazwa = xml_document.createElement("nazwa");
	wierzcholek.appendChild(nazwa);
	QDomText t_nazwa = xml_document.createTextNode(itr->nazwa);
	nazwa.appendChild(t_nazwa);           

	//x
	QDomElement x = xml_document.createElement("x");
	wierzcholek.appendChild(x);
	QDomText t_x = xml_document.createTextNode(QString::number(itr->x));
	x.appendChild(t_x);

	//y
	QDomElement y = xml_document.createElement("y");
	wierzcholek.appendChild(y);
	QDomText t_y = xml_document.createTextNode(QString::number(itr->y));
	y.appendChild(t_y);

	//krawedzie
	QDomElement krawedzie = xml_document.createElement("krawedzie");
	wierzcholek.appendChild(krawedzie);

	for(std::list<krawedz>::iterator itr2 = itr->krawedzie.begin(); itr2 != itr->krawedzie.end(); itr2++) {
	  //krawedz
	  QDomElement krawedz = xml_document.createElement("krawedz");
	  krawedzie.appendChild(krawedz);

	  //pojemnosc
	  QDomElement pojemnosc = xml_document.createElement("pojemnosc");
	  krawedz.appendChild(pojemnosc);
	  QDomText t_pojemnosc = xml_document.createTextNode(QString::number(itr2->pojemnosc));
	  pojemnosc.appendChild(t_pojemnosc);


	  //przeplyw
	  QDomElement przeplyw = xml_document.createElement("przeplyw");
	  krawedz.appendChild(przeplyw);
	  QDomText t_przeplyw = xml_document.createTextNode(QString::number(itr2->przeplyw));
	  przeplyw.appendChild(t_przeplyw);

	  //zbadano
	  QDomElement zbadano = xml_document.createElement("zbadano");
	  krawedz.appendChild(zbadano);
	  QDomText t_zbadano = xml_document.createTextNode(QString::number(itr2->zbadano));
	  zbadano.appendChild(t_zbadano);

	  //cel
	  QDomElement cel = xml_document.createElement("cel");
	  krawedz.appendChild(cel);
	  QDomText t_cel = xml_document.createTextNode(itr2->cel);
	  cel.appendChild(t_cel);
	}
  }

  filename = QFileDialog::getSaveFileName(this, tr("Save File"),
	  "./db.xml",
	  tr("XML files (*.xml)"));

  QFile file(filename);
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
	QTextStream(stdout) << "Blad zapisu";

  } else {
	QTextStream stream(&file);
	stream << xml_document.toString();
	file.close();
  }

}

void MainWindow::on_actionOtw_rz_triggered()
{
  open_file();
}

void MainWindow::on_actionZapisz_triggered()
{
  save_file();
}

void MainWindow::on_actionZamknij_triggered()
{
  this->close();
}

void MainWindow::max_przeplyw()
{
  int max_P = 0, max = INT_MAX;
  wierzcholek tmp, zrodlo;
//  get_node("zrodlo", zrodlo);
  zrodlo = wierzcholki.front();
  tmp = zrodlo;
  int stop = 0;


  /*
##### ALGORYTM
   */


  do{

	stop = max_get_node(tmp, max);
	if(tmp.nazwa == zrodlo.nazwa && stop == 1){
	  break;
    } else if(stop == 1){
        max_empty_way(tmp.nazwa);
        tmp = zrodlo;
    }

	if(stop == 2){
	  max_P += max;
	  max_update_przeplyw(max);
	  max = INT_MAX;
	  tmp = zrodlo;
	}

	/*
####### KONIEC
	 */

  } while(true);

  scene->clear();
  ui->label_5->setText(QString::number(max_P));

}
// 0 - znaleziono wierzcholek; 1 - nie znaleziono wierzcholka; 2 - znaleziono ujscie
int MainWindow::max_get_node(wierzcholek &w, int &max){
  for(std::list<wierzcholek>::iterator itt = wierzcholki.begin(); itt != wierzcholki.end(); itt++){
	if(itt->nazwa == w.nazwa){
	  for(std::list<krawedz>::iterator itr=itt->krawedzie.begin(); itr != itt->krawedzie.end(); itr++) {
        if(itr->cel == "ujscie"){
		  return 2;
		}
		if( (itr->przeplyw <= itr->pojemnosc) && itr->zbadano != 2) {
		  itr->zbadano = 1;
		  max = (max > itr->pojemnosc) ? itr->pojemnosc : max ;
		  get_node(itr->cel, w);
		  return 0;
		}
	  }
	}
  }
  return 1;
}

void MainWindow::max_update_przeplyw(int przeplyw){
  for(std::list<wierzcholek>::iterator itr=wierzcholki.begin(); itr != wierzcholki.end(); itr++) {
	for(std::list<krawedz>::iterator itr2 = itr->krawedzie.begin(); itr2 != itr->krawedzie.end(); itr2++){

	  if(itr->nazwa != "zrodlo" || itr->nazwa != "ujscie"){
		if(itr2->zbadano == 1){
		  itr2->przeplyw = przeplyw;
		  itr2->pojemnosc -= przeplyw;
		  if(itr2->pojemnosc <= 0){
			itr2->pojemnosc = 0;
			itr2->zbadano = 2;
		  } else {
			itr2->zbadano = 0;
		  }
		}
	  }
	}
  }
}

void MainWindow::max_empty_way(QString target) {
for(std::list<wierzcholek>::iterator itr=wierzcholki.begin(); itr != wierzcholki.end(); itr++) {
        for(std::list<krawedz>::iterator itr2 = itr->krawedzie.begin(); itr2 != itr->krawedzie.end(); itr2++){
            if(itr2->cel == target){
                itr2->zbadano = 2;
            }
        }
    }
}

void MainWindow::add_source() {
    wierzcholek zrodlo;
    zrodlo.nazwa = "zrodlo";
    zrodlo.x = 0;
    zrodlo.y = 0;

    wierzcholki.push_back(zrodlo);
    ui->comboBox->addItem(zrodlo.nazwa);
}

//cofnij
void MainWindow::on_pushButton_3_released()
{
    if(wierzcholki.size() > 1){
        wierzcholki.pop_back();
        ui->comboBox->removeItem(ui->comboBox->count()-1);
        scene->clear();
    }
}

//usun
void MainWindow::on_pushButton_4_released()
{
    wierzcholki.clear();
    ui->comboBox->clear();
    scene->clear();
    add_source();

}
