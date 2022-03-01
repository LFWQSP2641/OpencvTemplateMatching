#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2\imgproc\types_c.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    QLabel *SrcImTip;
    QLabel *TemplateImTip;
    QLabel *Result;
    QLabel *ImOut;
    QLineEdit *SrcImPath;
    QLineEdit *TemplateImPath;
    QPushButton *ChooseSrcIm;
    QPushButton *ChooseTemplateIm;
    QPushButton *ToMatch;
    QHBoxLayout *SrcImLayout;
    QHBoxLayout *TemplateImLayout;
    QHBoxLayout *ResultLayout;
    QVBoxLayout *MainLayout;
    QString StringSrcImPath;
    QString StringTemplateImPath;

    bool isDirExist(const QString filename);
    int TemplateMatching(const cv::Mat srcImage, const cv::Mat templateImage);
    QImage Mat2QImage(const cv::Mat& InputMat);

private slots:
    void SlotTemplateMatching();
};
#endif // WIDGET_H
