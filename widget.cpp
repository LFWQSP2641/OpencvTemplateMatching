#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    SrcImTip = new QLabel;
    TemplateImTip = new QLabel;
    Result = new QLabel;
    ImOut = new QLabel;
    SrcImPath = new QLineEdit;
    TemplateImPath = new QLineEdit;
    ToMatch = new QPushButton;
    ChooseSrcIm = new QPushButton;
    ChooseTemplateIm = new QPushButton;
    SrcImLayout = new QHBoxLayout;
    TemplateImLayout = new QHBoxLayout;
    ResultLayout = new QHBoxLayout;
    MainLayout = new QVBoxLayout;

    this->setWindowTitle("模板匹配");
    SrcImTip->setText("源图片");
    TemplateImTip->setText("模板图片");
    ChooseSrcIm->setText("选择源图片");
    ChooseTemplateIm->setText("选择模板图片");
    ToMatch->setText("计算");
    //ImOut->resize(500, 500);

    SrcImLayout->addWidget(SrcImTip);
    SrcImLayout->addWidget(SrcImPath);
    SrcImLayout->addWidget(ChooseSrcIm);
    TemplateImLayout->addWidget(TemplateImTip);
    TemplateImLayout->addWidget(TemplateImPath);
    TemplateImLayout->addWidget(ChooseTemplateIm);
    ResultLayout->addWidget(Result);
    ResultLayout->addWidget(ImOut);
    MainLayout->addLayout(SrcImLayout);
    MainLayout->addLayout(TemplateImLayout);
    MainLayout->addLayout(ResultLayout);
    MainLayout->addWidget(ToMatch);
    setLayout(MainLayout);

    connect(ChooseSrcIm, &QPushButton::clicked, [this]()
    {
        StringSrcImPath = QFileDialog::getOpenFileName(this,
                          tr("请选择一个图片"),
                          "./exam_源.png",
                          tr("图片文件(*png *jpg);;"));
        SrcImPath->setText(StringSrcImPath);
    });
    connect(ChooseTemplateIm, &QPushButton::clicked, [this]()
    {
        StringTemplateImPath = QFileDialog::getOpenFileName(this,
                               tr("请选择一个图片"),
                               "./exam_模板.png",
                               tr("图片文件(*png *jpg);;"));
        TemplateImPath->setText(StringTemplateImPath);
    });
    connect(SrcImPath, &QLineEdit::textChanged, [this]()
    {
        StringSrcImPath = SrcImPath->text();
    });
    connect(TemplateImPath, &QLineEdit::textChanged, [this]()
    {
        StringTemplateImPath = TemplateImPath->text();
    });
    connect(ToMatch, &QPushButton::clicked, this, &Widget::SlotTemplateMatching);
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::isDirExist(const QString filename)
{
    QFileInfo mFile(filename);
    qDebug() << mFile.isFile();
    if(mFile.isFile())
    {
        return true;
    }
    else
    {
        return false;
    }
}

int Widget::TemplateMatching(const cv::Mat srcImage, const cv::Mat templateImage)
{
    cv::Mat result;
    int result_cols = srcImage.cols - templateImage.cols + 1;
    int result_rows = srcImage.rows - templateImage.rows + 1;
    if(result_cols < 0 || result_rows < 0)
    {
        qDebug() << "Please input correct image!";
        return -1;
    }
    result.create(result_cols, result_rows, CV_32FC1);
    //enum { TM_SQDIFF=0, TM_SQDIFF_NORMED=1, TM_CCORR=2, TM_CCORR_NORMED=3, TM_CCOEFF=4, TM_CCOEFF_NORMED=5 };
    cv::matchTemplate(srcImage, templateImage, result, cv::TM_CCOEFF_NORMED); //最好匹配为1,值越小匹配越差
    double minVal = -1;
    double maxVal;
    cv::Point minLoc;
    cv::Point maxLoc;
    cv::Point matchLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

    //取大值(视匹配方法而定)
    //matchLoc = minLoc;
    matchLoc = maxLoc;

    //调试
    qDebug() << QString::number(maxVal) << Qt::endl << QString::number(minVal) << Qt::endl;

    //取大值,值越小表示越匹配
    QString str = "Similarity:" + QString::number((maxVal) * 100, 'f', 2) + "%";
    //qDebug(str.toAscii().data());
    qDebug() << str;
    //Result->setText("相似度:%1".arg(QString::number((maxVal) * 100, 'f', 2) + "%"));
    Result->setText(QString("相似度:" + QString::number((maxVal) * 100, 'f', 2) + "%"));

    cv::Mat mask = srcImage.clone(); //绘制最匹配的区域
    cv::rectangle(mask, matchLoc, cv::Point(matchLoc.x + templateImage.cols, matchLoc.y + templateImage.rows), cv::Scalar(0, 255, 0), 2, 8, 0);
    //cv::imshow("mask", mask);

    //cv::Mat的结果转换为QPixmap,并输出
    QImage image = Mat2QImage(mask);
    QPixmap pixmap = QPixmap::fromImage(image);
    int with = ImOut->width();
    int height = ImOut->height();
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ImOut->setPixmap(fitpixmap);
    //ImOut->resize(100, 100);

    return 0;
}

void Widget::SlotTemplateMatching()
{
    if (StringSrcImPath.isEmpty() || StringTemplateImPath.isEmpty())
    {
        Result->setText("未选择文件");
    }
    else if (!isDirExist(StringSrcImPath) || !isDirExist(StringTemplateImPath))
    {
        Result->setText("文件不存在");
    }
    else
    {
        if (TemplateMatching(cv::imread(StringSrcImPath.toLocal8Bit().data()), cv::imread(StringTemplateImPath.toLocal8Bit().data())) == -1)
        {
            Result->setText("模板图片比源图片大");
        }

    }
}

QImage Widget::Mat2QImage(const cv::Mat& InputMat)
{
    QImage Result;
    cv::Mat TmpMat;
    // convert the color space to RGB
    if (InputMat.channels() == 1)
    {
        cv::cvtColor(InputMat, TmpMat, CV_GRAY2RGB);
        Result = QImage((const uchar*)(TmpMat.data), TmpMat.cols, TmpMat.rows, TmpMat.cols * TmpMat.channels(),
                        QImage::Format_Indexed8);
    }
    else
    {
        cv::cvtColor(InputMat, TmpMat, CV_BGR2RGB);
        // construct the QImage using the data of the mat, while do not copy the data
        Result = QImage((const uchar*)(TmpMat.data), TmpMat.cols, TmpMat.rows, TmpMat.cols * TmpMat.channels(),
                        QImage::Format_RGB888);
    }
    // deep copy the data from mat to QImage
    Result.bits();
    return Result;
}
