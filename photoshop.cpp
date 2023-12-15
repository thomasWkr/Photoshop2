#include "photoshop.h"
#include "./ui_photoshop.h"

Photoshop::Photoshop(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Window)
{
    ui->setupUi(this);
}

Photoshop::~Photoshop()
{
    delete ui;
}

//Function to shorten image opening
void Photoshop::openLabel(QPixmap pix){
    secondWnd.show();
    secondWnd.setPixmap(pix);
    secondWnd.adjustSize();
}

//Open image file and print on both labels
void Photoshop::on_actionCarregar_Imagem_2_triggered()
{

    QString file_name = QFileDialog::getOpenFileName(this,
                                                     tr("Open File"),
                                                     QDir::homePath(),
                                                     tr("Images (*.png *.xpm *.jpg)"));
    if (!file_name.isEmpty()){
        activeOperations = true; //flag to enable operations
        QImage img(file_name);
        img1 = img; //Original Image
        img2 = img; //Modified Image
        w = img.width();
        h = img.height();
        QPixmap pix = QPixmap::fromImage(img);
        int wi = ui->label_img1->width();
        int hi = ui->label_img1->height();
        ui->label_img1->setPixmap(pix.scaled(wi,hi,Qt::KeepAspectRatio)); //prints image in both labels
        Photoshop::openLabel(pix);
        secondWnd.setWindowTitle("Modified Image");
        mint = 256; maxt = 0; //reset mint and maxt when image is changed
    }
}

//Save Image
void Photoshop::on_actionSalvar_Imagem_triggered()
{
    if (activeOperations){
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        tr("Save Image"),
                                                        "",
                                                        tr("Images (*.png *.xpm *.jpg)"));
        if (!fileName.isEmpty()){
            img2.save(fileName);
        }
    }
}

//Copy Image
void Photoshop::on_copy_clicked()
{
    if(activeOperations){
        img2 = img1;
        w = img2.width();
        h = img2.height();
        QPixmap pix = QPixmap::fromImage(img2);
        Photoshop::openLabel(pix);
    }
}

//Function that mirrors the image vertically
void Photoshop::on_vMirror_clicked()
{
    if(activeOperations){
        img_aux = img2;
        for (unsigned int i = 0; i < h; i++){
            for (unsigned int j = 0; j < w; j++){
                img2.setPixel(j,i,img_aux.pixel(w - j-1, i));
            }
        }
        QPixmap pix = QPixmap::fromImage(img2);
        Photoshop::openLabel(pix);
    }
}

//Function that mirrors the image horizontally
void Photoshop::on_hMirror_clicked()
{
    if(activeOperations){
        img_aux = img2;
        for (unsigned int i = 0; i < h; i++){
            for (unsigned int j = 0; j < w; j++){
                img2.setPixel(j,i,img_aux.pixel(j, h -i-1));
            }
        }
        QPixmap pix = QPixmap::fromImage(img2);
        Photoshop::openLabel(pix);
    }
}

//Calculates image gray scale
void Photoshop::luminanceCalc(){
    if(!isGray()){
        for (unsigned int i = 0; i < h; i++){
            for (unsigned int j = 0; j < w; j++){
                QColor newcolor;
                int gray = 0.299*img2.pixelColor(j,i).red() + 0.587*img2.pixelColor(j,i).green() + 0.114*img2.pixelColor(j,i).blue();
                if (maxt < gray){
                    maxt = gray;
                }
                if (mint > gray){
                    mint = gray;
                }
                newcolor.setRed(gray); //altera cada cor para a luminância calculada
                newcolor.setGreen(gray);
                newcolor.setBlue(gray);
                img2.setPixelColor(j,i,newcolor);
            }
        }
    }
}

// Turn image into gray scale
void Photoshop::on_grayScale_clicked()
{
    if(activeOperations){
        Photoshop::luminanceCalc();
        QPixmap pix = QPixmap::fromImage(img2);
        Photoshop::openLabel(pix);
    }
}

//Get tone from input
void Photoshop::on_toneValue_valueChanged(int arg1)
{
    tones = arg1;
}

//Quantization
void Photoshop::on_quant_clicked()
{
    if(activeOperations){
        Photoshop::luminanceCalc();
        int gapSize = maxt-mint+1; //distância entre os tones da imagem
        if (tones<gapSize && tones>0){
            QColor newcolor;
            unsigned int originalTone, luminance;
            float bin = (float) gapSize / tones;
            for (unsigned int i = 0; i < h; i++){
                for (unsigned int j = 0; j < w; j++){
                    originalTone = img2.pixelColor(j,i).red();
                    for (float t = mint; t < maxt; t+=bin){ //encontra o intervalo do tom atual
                        if (originalTone <= t+bin){
                            luminance = round(t+(bin/2)); //seta a nova luminãncia
                            newcolor.setRed(luminance);
                            newcolor.setGreen(luminance);
                            newcolor.setBlue(luminance);
                            img2.setPixelColor(j,i,newcolor);
                            t = 256; //sai do for
                        }
                    }
                }
            }
            mint = round(mint+(bin/2)); //atualiza mint e maxt
            maxt = round(maxt-(bin/2));
            QPixmap pix = QPixmap::fromImage(img2);
            Photoshop::openLabel(pix);
        }
    }
}

//Function to calculate histogram
void Photoshop::histogramCalc(unsigned int histogram[], QImage img){
    for (unsigned int i = 0; i < 256; i++){ //set histogram with 0
        histogram[i] = 0;
    }
    for (unsigned int i = 0; i < img.height(); i++){
        for (unsigned int j = 0; j < img.width(); j++){
            histogram[img.pixelColor(j,i).red()]+=1; //count ocurrences of each tone
        }
    }
}

//Histogram
void Photoshop::on_histogram_clicked()
{
    if(activeOperations){
        Photoshop::on_grayScale_clicked(); //calls function to turn image into gray scale
        unsigned int histogram[256];
        Photoshop::histogramCalc(histogram, img2);
        auto set0 = new QBarSet("");
        int max = 0;
        for (unsigned int i = 0; i < 256; i++){ //find tone with maximum ocurrences
            if (max < histogram[i])
                max = histogram[i];
        }
        for (unsigned int i = 0; i < 256; i++){
            *set0 << (float) 255*histogram[i]/(max); //pass each column of the graph
        }
        QBarSeries *series = new QBarSeries;
        series->append(set0);
        auto chart = new QChart;
        chart->legend()->setVisible(false);
        chart->addSeries(series);
        chart->createDefaultAxes();
        QChartView *chartView = new QChartView(chart);
        histogramWindow.setCentralWidget(chartView);
        histogramWindow.setWindowTitle("Histogram");
        histogramWindow.resize(1280,700);
        histogramWindow.show();
    }
}

//get bright value
void Photoshop::on_brightValue_valueChanged(int arg1)
{
    bright = arg1;
}

//Brightness
void Photoshop::on_bright_clicked()
{
    if(activeOperations){
        for (unsigned int i = 0; i < h; i++){
            for (unsigned int j = 0; j < w; j++){
                QColor newcolor;

                if (img2.pixelColor(j,i).red()+bright > 255) //check if tone is out of the 255 limit
                    newcolor.setRed(255);
                else if (img2.pixelColor(j,i).red()+bright < 0) //check if tone is lower than the 0 limit
                    newcolor.setRed(0);
                else
                    newcolor.setRed(img2.pixelColor(j,i).red()+bright);


                if (img2.pixelColor(j,i).green()+bright > 255)
                    newcolor.setGreen(255);
                else if (img2.pixelColor(j,i).green()+bright < 0)
                    newcolor.setGreen(0);
                else
                    newcolor.setGreen(img2.pixelColor(j,i).green()+bright);


                if (img2.pixelColor(j,i).blue()+bright > 255)
                    newcolor.setBlue(255);
                else if (img2.pixelColor(j,i).blue()+bright < 0)
                    newcolor.setBlue(0);
                else
                    newcolor.setBlue(img2.pixelColor(j,i).blue()+bright);

                img2.setPixelColor(j,i,newcolor);
            }
        }
        QPixmap pix = QPixmap::fromImage(img2);
        Photoshop::openLabel(pix);
    }
}

//get contrast value
void Photoshop::on_contrastValue_valueChanged(double arg1)
{
    contrast = arg1;
}

//Contrast
void Photoshop::on_contrast_clicked()
{
    if(activeOperations){
        for (unsigned int i = 0; i < h; i++){
            for (unsigned int j = 0; j < w; j++){
                QColor newcolor;

                if (img2.pixelColor(j,i).red()*contrast > 255) //check if tone is out of the 255 limit
                    newcolor.setRed(255);
                else
                    newcolor.setRed(img2.pixelColor(j,i).red()*contrast);


                if (img2.pixelColor(j,i).green()*contrast > 255)
                    newcolor.setGreen(255);
                else
                    newcolor.setGreen(img2.pixelColor(j,i).green()*contrast);


                if (img2.pixelColor(j,i).blue()*contrast > 255)
                    newcolor.setBlue(255);
                else
                    newcolor.setBlue(img2.pixelColor(j,i).blue()*contrast);

                img2.setPixelColor(j,i,newcolor);
            }
        }
        QPixmap pix = QPixmap::fromImage(img2);
        Photoshop::openLabel(pix);
    }
}

//Negative
void Photoshop::on_negative_clicked()
{
    if(activeOperations){
        for (unsigned int i = 0; i < h; i++){
            for (unsigned int j = 0; j < w; j++){
                QColor newcolor;

                newcolor.setRed(255-img2.pixelColor(j,i).red());
                newcolor.setGreen(255-img2.pixelColor(j,i).green());
                newcolor.setBlue(255-img2.pixelColor(j,i).blue());

                img2.setPixelColor(j,i,newcolor);
            }
        }
        QPixmap pix = QPixmap::fromImage(img2);
        Photoshop::openLabel(pix);
    }
}

//test if image is in gray scale
bool Photoshop::isGray(){
    for (unsigned int i = 0; i < h; i++){
        for (unsigned int j = 0; j < w; j++){
            if (img2.pixelColor(j,i).red()!=img2.pixelColor(j,i).blue() ||
                img2.pixelColor(j,i).red()!=img2.pixelColor(j,i).green() ||
                img2.pixelColor(j,i).blue()!=img2.pixelColor(j,i).green()){
                return false;
            }
        }
    }
    return true;
}

//Function to calculate cumulative histogram
void Photoshop::cumulativeCalc(unsigned int histogram_cum[], unsigned int histogram[], QImage img){
    histogram_cum[0] = histogram[0];
    for (int i = 1; i < 256; i++) {
        histogram_cum[i] = histogram_cum[i-1] + histogram[i]; //sum of cumulative histogram
    }
    for (int i = 0; i < 256; i++) {
        histogram_cum[i] = (255*histogram_cum[i])/(img.width()*img.height()); //normalization of cumulative histogram
    }
}

//Equalization
void Photoshop::on_equalize_clicked()
{
    if(activeOperations){
        unsigned int histogram[256]; //histogram of old image
        unsigned int histogram_cum[256]; //cumulative histogram
        bool is_gray = Photoshop::isGray();
        img_aux = img2;
        Photoshop::luminanceCalc();
        Photoshop::histogramCalc(histogram, img2);
        img2 = img_aux;
        Photoshop::cumulativeCalc(histogram_cum, histogram, img2);
        for (unsigned int i = 0; i < h; i++){
            for (unsigned int j = 0; j < w; j++){
                QColor newcolor;
                newcolor.setRed(histogram_cum[img2.pixelColor(j,i).red()]); //sets each RGB tone to the histogram_cum's tone associated
                newcolor.setBlue(histogram_cum[img2.pixelColor(j,i).blue()]);
                newcolor.setGreen(histogram_cum[img2.pixelColor(j,i).green()]);
                img2.setPixelColor(j,i,newcolor);
            }
        }
        QPixmap pix = QPixmap::fromImage(img2);
        Photoshop::openLabel(pix);

        if (is_gray){ //prints histogram if image is in gray scale
            auto set0 = new QBarSet("Number of Pixels");
            int max = 0;
            for (unsigned int i = 0; i < 256; i++){
                if (max < histogram[i])
                    max = histogram[i];
            }
            for (unsigned int i = 0; i < 256; i++){
                *set0 << (float) 255*histogram[i]/(max);
            }
            QBarSeries *series = new QBarSeries;
            series->append(set0);
            auto chart = new QChart;
            chart->addSeries(series);
            chart->createDefaultAxes();
            chart->legend()->setVisible(false);
            QChartView *chartView = new QChartView(chart);
            histogramWindow2.setCentralWidget(chartView); //shows old image histogram
            histogramWindow2.setWindowTitle("Histogram of Last Image");
            histogramWindow2.resize(1280,700);
            histogramWindow2.show();
            Photoshop::on_histogram_clicked(); //uses histogram function to show new histogram
        }
    }
}

//Matching
void Photoshop::on_matching_clicked()
{
    if(activeOperations){
        QString file_name = QFileDialog::getOpenFileName(this,
                                                         tr("Open File"),
                                                         QDir::homePath(),
                                                         tr("Images (*.png *.xpm *.jpg)"));
        if (!file_name.isEmpty()){
            unsigned int histogram[256], histogram2[256];
            unsigned int histogram_cum[256], histogram_cum2[256];
            QImage img(file_name);
            for (unsigned int i = 0; i < img.height(); i++){ //transform img into luminance
                for (unsigned int j = 0; j < img.width(); j++){
                    QColor newcolor;
                    int gray = 0.299*img.pixelColor(j,i).red() + 0.587*img.pixelColor(j,i).green() + 0.114*img.pixelColor(j,i).blue();
                    newcolor.setRed(gray); //change to luminance color calculated
                    newcolor.setGreen(gray);
                    newcolor.setBlue(gray);
                    img.setPixelColor(j,i,newcolor);
                }
            }
            Photoshop::luminanceCalc(); //transform modified image into luminance
            Photoshop::histogramCalc(histogram, img2); //calculates all necessary histograms
            Photoshop::histogramCalc(histogram2, img);
            Photoshop::cumulativeCalc(histogram_cum, histogram, img2);
            Photoshop::cumulativeCalc(histogram_cum2, histogram2, img);
            for (unsigned int i = 0; i < h; i++){
                for (unsigned int j = 0; j < w; j++){
                    int min_x = 0;
                    int dif = fabs(histogram_cum[img2.pixelColor(j,i).red()] - histogram_cum2[min_x]);
                    for (unsigned int x = 1; x < 256; x++){ //finds target cumulative histogram closest value
                        if (fabs(histogram_cum[img2.pixelColor(j,i).red()] - histogram_cum2[x]) < dif){
                            dif = fabs(histogram_cum[img2.pixelColor(j,i).red()] - histogram_cum2[x]);
                            min_x = x;
                        }
                    }
                    QColor newcolor;
                    newcolor.setRed(min_x); //sets each RGB tone to histogram_cum's tone associated
                    newcolor.setBlue(min_x);
                    newcolor.setGreen(min_x);
                    img2.setPixelColor(j,i,newcolor);
                }
            }
            QPixmap pix = QPixmap::fromImage(img2);
            Photoshop::openLabel(pix);
        }
    }
}

//Zoom Out
void Photoshop::on_zoomOut_clicked()
{
    if(activeOperations){
        if(sx < img2.width() && sy < img2.height()){
            int newHeight = ceil((float) img2.height() / sy); //creates image with smaller size
            int newWidth = ceil((float) img2.width() / sx);

            QImage newImage(newWidth, newHeight, img2.format());

            for (int i = 0; i < h; i+=sy){ //finds the average value of each sx,sy matrix
                for (int j = 0; j < w; j+=sx){
                    int pixels = 0;
                    QColor newcolor;
                    unsigned int red = 0, green = 0, blue = 0;
                    for (unsigned int x = 0; (x < sx && j+x < w) ; x++){ //check if
                        for (unsigned int y = 0; (y < sy && i+y < h); y++){
                            red+=img2.pixelColor(j+x,i+y).red();
                            blue+=img2.pixelColor(j+x,i+y).blue();
                            green+=img2.pixelColor(j+x,i+y).green();
                            pixels++;
                        }
                    }
                    newcolor.setRed((int) red/pixels);
                    newcolor.setBlue((int) blue/pixels);
                    newcolor.setGreen((int) green/pixels);
                    newImage.setPixelColor((int) (j/sx), (int) (i/sy), newcolor);
                }
            }
            img2 = newImage;
            w = newImage.width();
            h = newImage.height();
            QPixmap pix = QPixmap::fromImage(img2);
            Photoshop::openLabel(pix);
        }

    }

}

//get sx value
void Photoshop::on_sxValue_valueChanged(int arg1)
{
    sx = arg1;
}

//get sy value
void Photoshop::on_syValue_valueChanged(int arg1)
{
    sy = arg1;
}

//Zoom In
void Photoshop::on_zoomIn_clicked()
{
    if(activeOperations){

        QImage newImage((img2.width()*2)-1, (img2.height()*2)-1, img2.format());
        for (int i = 0; i < h; i++){
            for (int j = 0; j < w; j++){
                newImage.setPixelColor(j*2, i*2, img2.pixelColor(j,i));

                QColor newcolor;
                if (j>0){
                    newcolor.setRed((int) (img2.pixelColor(j,i).red() + img2.pixelColor(j-1,i).red())/2);
                    newcolor.setBlue((int) (img2.pixelColor(j,i).blue() + img2.pixelColor(j-1,i).blue())/2);
                    newcolor.setGreen((int) (img2.pixelColor(j,i).green() + img2.pixelColor(j-1,i).green())/2);

                    newImage.setPixelColor(j*2-1,i*2,newcolor);
                }

                if (i>0){
                    newcolor.setRed((int) (img2.pixelColor(j,i).red() + img2.pixelColor(j,i-1).red())/2);
                    newcolor.setBlue((int) (img2.pixelColor(j,i).blue() + img2.pixelColor(j,i-1).blue())/2);
                    newcolor.setGreen((int) (img2.pixelColor(j,i).green() + img2.pixelColor(j,i-1).green())/2);

                    newImage.setPixelColor(j*2,i*2-1,newcolor);
                }

                if (i>0 && j>0){
                    newcolor.setRed((int) (newImage.pixelColor(j*2-2,i*2-1).red()
                                           + newImage.pixelColor(j*2,i*2-1).red())/2);
                    newcolor.setBlue((int) (newImage.pixelColor(j*2-2,i*2-1).blue()
                                            + newImage.pixelColor(j*2,i*2-1).blue())/2);
                    newcolor.setGreen((int) (newImage.pixelColor(j*2-2,i*2-1).green()
                                             + newImage.pixelColor(j*2,i*2-1).green())/2);

                    newImage.setPixelColor(j*2-1,i*2-1,newcolor);
                }
            }
        }
        img2 = newImage;
        w = newImage.width();
        h = newImage.height();
        QPixmap pix = QPixmap::fromImage(img2);
        Photoshop::openLabel(pix);
    }
}

//Rotate Right
void Photoshop::on_rotateR_clicked()
{
    if(activeOperations){

        QImage newImage(img2.height(), img2.width(), img2.format());
        for (int i = 0; i < h; i++){
            for (int j = 0; j < w; j++){
                newImage.setPixel(h-1-i, j, img2.pixel(j,i));
            }
        }
        img2 = newImage;
        w = newImage.width();
        h = newImage.height();
        QPixmap pix = QPixmap::fromImage(img2);
        Photoshop::openLabel(pix);
    }
}

//Rotate Left
void Photoshop::on_rotateL_clicked()
{
    if(activeOperations){

        QImage newImage(img2.height(), img2.width(), img2.format());
        for (int i = 0; i < h; i++){
            for (int j = 0; j < w; j++){
                newImage.setPixel(i, w-1-j, img2.pixel(j,i));
            }
        }
        img2 = newImage;
        w = newImage.width();
        h = newImage.height();
        QPixmap pix = QPixmap::fromImage(img2);
        Photoshop::openLabel(pix);
    }
}

//function to compare matrixes
bool Photoshop::compare_matrix(double matrix[3][3])
{
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            if(kernel[j][i]!=matrix[j][i])
                return false;
        }
    }
    return true;
}

//Convolution
void Photoshop::on_convolution_clicked()
{
    if(activeOperations){

        kernel[0][0] = ui->kernelValue_1->value();
        kernel[1][0] = ui->kernelValue_2->value();
        kernel[2][0] = ui->kernelValue_3->value();
        kernel[0][1] = ui->kernelValue_4->value();
        kernel[1][1] = ui->kernelValue_5->value();
        kernel[2][1] = ui->kernelValue_6->value();
        kernel[0][2] = ui->kernelValue_7->value();
        kernel[1][2] = ui->kernelValue_8->value();
        kernel[2][2] = ui->kernelValue_9->value();

        if (!Photoshop::compare_matrix(passa_baixas)){
            Photoshop::luminanceCalc();
        }

        QImage newImage(w, h, img2.format());

        for (int i = 1; i < h-1; i++){
            for (int j = 1; j < w-1; j++){
                QColor newcolor;
                int red=0, green=0, blue=0;

                for (int x = 0; x < 3; x++){
                    for (int y = 0; y < 3; y++){
                        red += img2.pixelColor(j-1+x,i-1+y).red()*kernel[2-x][2-y];
                        green += img2.pixelColor(j-1+x,i-1+y).green()*kernel[2-x][2-y];
                        blue += img2.pixelColor(j-1+x,i-1+y).blue()*kernel[2-x][2-y];
                    }
                }
                if (!Photoshop::compare_matrix(passa_baixas) &&
                    !Photoshop::compare_matrix(passa_altas) &&
                    !Photoshop::compare_matrix(passa_altas_gen)){
                    red+=127;
                    green+=127;
                    blue+=127;
                }

                if(red>255)
                    newcolor.setRed(255);
                else if (red<0)
                    newcolor.setRed(0);
                else
                    newcolor.setRed(red);

                if(green>255)
                    newcolor.setGreen(255);
                else if (green<0)
                    newcolor.setGreen(0);
                else
                    newcolor.setGreen(green);

                if(blue>255)
                    newcolor.setBlue(255);
                else if (blue<0)
                    newcolor.setBlue(0);
                else
                    newcolor.setBlue(blue);

                newImage.setPixelColor(j, i, newcolor);
            }
        }
        img2 = newImage;
        w = newImage.width();
        h = newImage.height();
        QPixmap pix = QPixmap::fromImage(img2);
        Photoshop::openLabel(pix);
    }
}

