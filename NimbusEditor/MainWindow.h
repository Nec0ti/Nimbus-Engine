#pragma once

#include <QMainWindow>
#include <QStringList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Navbar
    void on_navbar_currentRowChanged(int currentRow);

    // Yeni Proje Sayfası
    void on_browseButton_clicked();
    void on_createProjectButton_clicked();
    void on_cancelCreateButton_clicked();

    // Home Sayfası
    void on_newProjectButton_clicked(); // Yeni proje sayfasına gider
    void on_openProjectButton_clicked(); // Seçili projeyi açar (şimdilik placeholder)
    void on_projectListWidget_itemSelectionChanged();
    void on_deleteProjectButton_clicked();

private:
    Ui::MainWindow *ui;
    QStringList m_projectPaths;

    // Kurulum ve Yardımcı Fonksiyonlar
    void setupNavbar();
    void setupTemplates();
    void loadProjects();
    void saveProjects();
    void updateDetailsPanel(const QString& projectPath);
};