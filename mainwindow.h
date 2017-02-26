#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

        private slots:
            void on_pushButton_2_released();

        void on_pushButton_released();

        void on_spinBox_valueChanged(int arg1);

        void on_comboBox_activated(const QString &arg1);

        void on_lineEdit_lostFocus();

        void on_actionOtw_rz_triggered();

        void on_actionZapisz_triggered();

        void on_actionZamknij_triggered();

        void on_pushButton_3_released();

        void on_pushButton_4_released();

    private:
        struct krawedz {
            int zbadano; // wartosc 0 - nie ; 1 - tak ; 2 - nie badaj
            int przeplyw;
            int pojemnosc;
            QString cel;
        };

        struct wierzcholek {
            QString nazwa;
            int x, y;
            std::list<krawedz> krawedzie;
        };

        Ui::MainWindow *ui;
        QGraphicsScene *scene;
        std::list <wierzcholek> wierzcholki;

        void get_node(QString name, wierzcholek &w);
        void check_node_position (wierzcholek &w, int nr_of_node, int message);
        void update_node (QString name, krawedz kr);
        void find_node_without_child(std::list<wierzcholek> &tmp);
        void add_outlet(wierzcholek &tmp);
        void open_file();
        void save_file();
        void max_przeplyw();
        int max_get_node(wierzcholek &w, int &max);
        void max_update_przeplyw(int przeplyw);
        void max_empty_way(QString target);
        void add_source();
        void remove_path(QString target);

    protected:
        void paintEvent(QPaintEvent *e);
};

#endif // MAINWINDOW_H
