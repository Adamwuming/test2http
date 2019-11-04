#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    QNetworkAccessManager* m_Manager ;
    QNetworkRequest m_Request;
    QNetworkReply* m_Reply;

private slots:
    void ReplyReadFunc(QNetworkReply* Reply);
    void handle_postPushButton_clicked();



};

#endif // MAINWINDOW_H
