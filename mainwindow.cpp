#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostInfo>
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QUrl url;
    m_Manager = new QNetworkAccessManager(this);
    url.setUrl("http://ip.taobao.com/service/getIpInfo2.php");
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1SslV3);

    m_Request.setSslConfiguration(config);
    m_Request.setUrl(url);
    m_Request.setRawHeader("Accept", "*/*");
    m_Request.setRawHeader("Accept-Language", "zh-CN,zh;q=0->9");
    m_Request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    m_Request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36");
    m_Request.setRawHeader("X-Requested-With:", "XMLHttpRequest");
    m_Request.setRawHeader("Referer", "http://ip.taobao.com/ipSearch.html");

    connect(m_Manager, &QNetworkAccessManager::finished, this, &MainWindow::ReplyReadFunc);
    connect(ui->postPushButton, SIGNAL(clicked()), this, SLOT(handle_postPushButton_clicked()));
    connect(ui->postLineEdit, SIGNAL(returnPressed()), this, SLOT(handle_postPushButton_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::handle_postPushButton_clicked()
{
    QHostInfo info = QHostInfo::fromName(ui->postLineEdit->text().toLocal8Bit());
    if ( !info.addresses().isEmpty())
    {
        QString ipAddress = info.addresses().first().toString();
        QByteArray sendData;
        sendData = "ip=" +   ipAddress.toLocal8Bit();
        m_Reply = m_Manager->post(m_Request, sendData);
    }
    else
    {
        qDebug() << "Hostname unavaliable~~";
    }
}
void MainWindow::ReplyReadFunc(QNetworkReply* Reply)
{
    int NetworkErrors = -1;
    NetworkErrors = Reply->error();
    QString ip;
    QString country;
    QString region;
    QString isp;

    if(NetworkErrors == QNetworkReply::NoError)
    {
        QByteArray databuff = Reply->readAll();
        QString result = QString::fromStdString(databuff.toStdString()).toUtf8();
        QJsonDocument document;
        QJsonParseError ParseError;
        document = QJsonDocument::fromJson(result.toUtf8(), &ParseError);
        if(!document.isNull() && ParseError.error == QJsonParseError::NoError)
        {
            if(document.isObject())
            {
                QJsonObject Object = document.object(); //获取文档对象
                if(!Object.isEmpty())
                {
                    ui->replyPlaintTextEdit->appendPlainText(QString(QJsonDocument(Object).toJson()));
                    if(Object.contains("data")) //先判断有没有
                    {
                        QJsonValue dataObj = Object.value("data");
                        if(dataObj.isObject())  //判断是否是对象
                        {
                            QJsonObject ObjectData = dataObj.toObject();
                            if(ObjectData.contains("ip"))
                            {
                                QJsonValue ipVal = ObjectData.value("ip");
                                if(ipVal.isString())
                                {
                                    ip = ipVal.toString();
                                }
                            }
                            if(ObjectData.contains("region"))
                            {
                                QJsonValue regionVal = ObjectData.value("region");
                                if(regionVal.isString())
                                {
                                    region = regionVal.toString();
                                }
                            }
                            if(ObjectData.contains("isp"))
                            {
                                QJsonValue ispVal = ObjectData.value("isp");
                                if(ispVal.isString())
                                {
                                    isp = ispVal.toString();
                                }
                            }
                        }
                    }
                }
            }

            ui->replyPlaintTextEdit->appendPlainText("您的ip:" + ip);

            ui->replyPlaintTextEdit->appendPlainText("所在城市:" + region);

            ui->replyPlaintTextEdit->appendPlainText( "运营商:" + isp);

            qDebug() << country.toStdString().c_str();
        }
    }
}
