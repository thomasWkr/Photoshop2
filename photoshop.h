#ifndef PHOTOSHOP_H
#define PHOTOSHOP_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QImage>
#include <QVector>
#include <QQueue>
#include <QLabel>
#include <QtCharts>

QT_BEGIN_NAMESPACE
namespace Ui { class Window; }
QT_END_NAMESPACE

class Photoshop : public QMainWindow
{
    Q_OBJECT

public:
    Photoshop(QWidget *parent = nullptr);
    ~Photoshop();

private slots:

    void openLabel(QPixmap pix);

    void on_actionCarregar_Imagem_2_triggered();

    void on_actionSalvar_Imagem_triggered();

    void on_copy_clicked();

    void on_quant_clicked();

    void on_vMirror_clicked();

    void on_hMirror_clicked();

    void on_grayScale_clicked();

    void luminanceCalc();

    void on_toneValue_valueChanged(int arg1);

    void histogramCalc(unsigned int*, QImage img);

    void on_histogram_clicked();

    void on_bright_clicked();

    void on_brightValue_valueChanged(int arg1);

    void on_contrastValue_valueChanged(double arg1);

    void on_contrast_clicked();

    void on_negative_clicked();

    bool isGray();

    void cumulativeCalc(unsigned int histogram_cum[], unsigned int histogram[], QImage img);

    void on_equalize_clicked();

    void on_zoomOut_clicked();

    void on_sxValue_valueChanged(int arg1);

    void on_syValue_valueChanged(int arg1);

    void on_zoomIn_clicked();

    void on_rotateR_clicked();

    void on_rotateL_clicked();

    bool compare_matrix(double matrix[3][3]);

    void on_convolution_clicked();

    void on_matching_clicked();

private:
    Ui::Window *ui;
    QLabel secondWnd;
    QImage img1;
    QImage img2;
    QImage img_aux;
    bool activeOperations = false;
    unsigned int mint = 256;
    unsigned int maxt = 0;
    unsigned int tones = 1;
    unsigned int w;
    unsigned int h;
    QMainWindow histogramWindow;
    QMainWindow histogramWindow2;
    int bright=0;
    double contrast=0;
    unsigned int sx=1;
    unsigned int sy=1;
    double kernel[3][3] = {0};
    double passa_baixas[3][3] =  {  {0.0625, 0.125, 0.0625},
                                    {0.125,  0.25,  0.125},
                                    {0.0625, 0.125, 0.0625} };
    double passa_altas[3][3] =  {   {0, -1, 0},
                                    {-1, 4, -1},
                                    {0, -1, 0} };
    double passa_altas_gen[3][3] =  {   {-1, -1, -1},
                                        {-1,  8, -1},
                                        {-1, -1, -1} };
};
#endif // PHOTOSHOP_H
