#include "MainWindow.h"
#include "ui_MainWindow.h"

// Gerekli tüm Qt başlık dosyaları
#include <QStyle>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <QImage>
#include <QPainter>
#include <QColor>
#include <QFont>
#include <QDateTime>
#include <QIcon>

// C++ standard kütüphanesi
#include <filesystem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowTitle("Nimbus Hub");

    setupNavbar();
    setupTemplates();
    loadProjects();
    
    ui->mainContent->setCurrentIndex(0);
    ui->detailsPanel->setVisible(false);
    ui->openProjectButton->setEnabled(false); // Başlangıçta devre dışı
}

MainWindow::~MainWindow() {
    delete ui;
}

// === KURULUM FONKSİYONLARI ===

void MainWindow::setupNavbar() {
    QListWidgetItem *homeItem = new QListWidgetItem(this->style()->standardIcon(QStyle::SP_DirHomeIcon), "Home");
    ui->navbar->addItem(homeItem);
    QListWidgetItem *newsItem = new QListWidgetItem(this->style()->standardIcon(QStyle::SP_FileIcon), "Haberler");
    ui->navbar->addItem(newsItem);
    QListWidgetItem *notesItem = new QListWidgetItem(this->style()->standardIcon(QStyle::SP_FileDialogDetailedView), "Sürüm Notları");
    ui->navbar->addItem(notesItem);
    QListWidgetItem *docsItem = new QListWidgetItem(this->style()->standardIcon(QStyle::SP_DialogHelpButton), "Dokümantasyon");
    ui->navbar->addItem(docsItem);
    ui->navbar->setCurrentRow(0);
}

void MainWindow::setupTemplates() {
    QListWidgetItem* empty = new QListWidgetItem(this->style()->standardIcon(QStyle::SP_FileIcon), "Boş Proje");
    empty->setToolTip("Her şeyin sıfırdan başladığı boş bir proje.");
    ui->templateListWidget->addItem(empty);

    QListWidgetItem* tps = new QListWidgetItem(this->style()->standardIcon(QStyle::SP_ComputerIcon), "Third Person Player");
    tps->setToolTip("Basit bir üçüncü şahıs karakter kontrolcüsü içeren proje.");
    ui->templateListWidget->addItem(tps);
    
    QListWidgetItem* fps = new QListWidgetItem(this->style()->standardIcon(QStyle::SP_DesktopIcon), "First Person Player");
    fps->setToolTip("Basit bir birinci şahıs karakter kontrolcüsü içeren proje.");
    ui->templateListWidget->addItem(fps);

    ui->templateListWidget->setCurrentRow(0);
}

// === PROJE YÖNETİM FONKSİYONLARI ===

void MainWindow::loadProjects() {
    ui->detailsPanel->setVisible(false);
    ui->projectListWidget->clear();
    m_projectPaths.clear();

    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString manifestPath = configPath + "/manifest.json";

    QFile manifestFile(manifestPath);
    if (!manifestFile.exists() || !manifestFile.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(manifestFile.readAll());
    manifestFile.close();

    if (doc.isObject()) {
        QJsonArray projectsArray = doc.object()["projects"].toArray();
        for (const auto& val : projectsArray) {
            QString path = val.toString();
            if (path.isEmpty() || !QDir(path).exists()) continue;

            m_projectPaths.append(path);
            
            QIcon icon;
            QString thumbPath = path + "/.data/thumbnail.png";
            if (QFile::exists(thumbPath)) {
                icon = QIcon(thumbPath);
            } else {
                icon = this->style()->standardIcon(QStyle::SP_DirIcon);
            }

            QListWidgetItem* item = new QListWidgetItem(icon, QFileInfo(path).baseName());
            item->setData(Qt::UserRole, path);
            item->setSizeHint(QSize(item->sizeHint().width(), 80));
            ui->projectListWidget->addItem(item);
        }
    }
}

void MainWindow::saveProjects() {
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir(configPath).mkpath(".");
    QString manifestPath = configPath + "/manifest.json";

    QJsonArray projectsArray;
    for (const QString& path : m_projectPaths) {
        projectsArray.append(path);
    }
    
    QJsonObject rootObj;
    rootObj["projects"] = projectsArray;
    QJsonDocument doc(rootObj);

    QFile manifestFile(manifestPath);
    if (manifestFile.open(QIODevice::WriteOnly)) {
        manifestFile.write(doc.toJson());
        manifestFile.close();
    }
}

void MainWindow::updateDetailsPanel(const QString& projectPath) {
    QString manifestPath = projectPath + "/.data/manifest.json";
    QFile manifestFile(manifestPath);
    if (!manifestFile.open(QIODevice::ReadOnly)) {
        ui->projectNameLabel->setText("Veri Yok");
        ui->engineVersionLabel->setText("N/A");
        ui->revisionLabel->setText("N/A");
        ui->creationDateLabel->setText("N/A");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(manifestFile.readAll());
    manifestFile.close();

    QJsonObject manifest = doc.object();
    ui->projectNameLabel->setText(manifest["projectName"].toString());
    ui->engineVersionLabel->setText(manifest["engineVersion"].toString());
    ui->revisionLabel->setText(QString::number(manifest["revision"].toInt()));
    
    QDateTime date = QDateTime::fromString(manifest["creationDate"].toString(), Qt::ISODate);
    ui->creationDateLabel->setText(date.toString("dd MMMM yyyy"));
}

// === QT SLOTLARI ===

void MainWindow::on_navbar_currentRowChanged(int currentRow) {
    if(currentRow == 0) ui->mainContent->setCurrentIndex(0); // Home
    if(currentRow == 1) ui->mainContent->setCurrentIndex(2); // News
    if(currentRow == 2) ui->mainContent->setCurrentIndex(3); // Notes
    if(currentRow == 3) ui->mainContent->setCurrentIndex(4); // Docs
}

void MainWindow::on_newProjectButton_clicked() {
    ui->mainContent->setCurrentIndex(1);
}

void MainWindow::on_openProjectButton_clicked() {
    QListWidgetItem* selectedItem = ui->projectListWidget->currentItem();
    if (!selectedItem) return;

    QString projectPath = selectedItem->data(Qt::UserRole).toString();
    QString projectName = QFileInfo(projectPath).baseName();

    QMessageBox::information(this, "Motor Başlatılıyor...", 
        QString("'%1' projesi Nimbus Engine ile açılacak.\n\n(Bu özellik henüz geliştirilmedi.)").arg(projectName));
}

void MainWindow::on_cancelCreateButton_clicked() {
    ui->mainContent->setCurrentIndex(0);
}

void MainWindow::on_browseButton_clicked() {
    QString directory = QFileDialog::getExistingDirectory(this, "Proje Konumunu Seçin");
    if (!directory.isEmpty()) {
        ui->projectPathInput->setText(directory);
    }
}

void MainWindow::on_createProjectButton_clicked() {
    QString projectName = ui->projectNameInput->text();
    QString projectPath = ui->projectPathInput->text();
    int selectedTemplateIndex = ui->templateListWidget->currentRow();

    if (projectName.isEmpty() || projectPath.isEmpty() || selectedTemplateIndex < 0) {
        QMessageBox::warning(this, "Eksik Bilgi", "Proje adı, konumu ve şablonu seçilmelidir.");
        return;
    }

    std::filesystem::path fullPath = projectPath.toStdString();
    fullPath /= projectName.toStdString(); // <-- HATA BURADA DÜZELTİLDİ
    QString fullPathStr = QString::fromStdString(fullPath.string());

    try {
        if (std::filesystem::exists(fullPath)) {
             QMessageBox::warning(this, "Hata", "Bu konumda zaten aynı isimde bir klasör var.");
             return;
        }

        std::filesystem::create_directories(fullPath / "Dev" / "Scripts");
        std::filesystem::create_directory(fullPath / "Suite");
        std::filesystem::create_directory(fullPath / ".data");
        std::filesystem::create_directory(fullPath / "Logs");
        std::filesystem::create_directory(fullPath / "Settings");
        std::filesystem::create_directory(fullPath / "Plugins");
        
        { // Proje manifest.json oluşturma bloğu
            QJsonObject projectManifest;
            projectManifest["projectName"] = projectName;
            projectManifest["engineVersion"] = "0.1.0-alpha";
            projectManifest["revision"] = 1;
            QDateTime now = QDateTime::currentDateTime();
            projectManifest["creationDate"] = now.toString(Qt::ISODate);
            projectManifest["lastOpened"] = now.toString(Qt::ISODate);

            QJsonDocument doc(projectManifest);
            QFile manifestFile(fullPathStr + "/.data/manifest.json");
            if (manifestFile.open(QIODevice::WriteOnly)) {
                manifestFile.write(doc.toJson(QJsonDocument::Indented));
                manifestFile.close();
            }
        }

        { // thumbnail.png oluşturma bloğu
            QImage thumbnail(128, 128, QImage::Format_RGB32);
            thumbnail.fill(QColor("#3c4250"));
            QPainter painter(&thumbnail);
            painter.setPen(QColor("#e0e0e0"));
            painter.setFont(QFont("Noto Sans", 48, QFont::Bold));
            painter.drawText(thumbnail.rect(), Qt::AlignCenter, projectName.left(1).toUpper());
            painter.end();
            thumbnail.save(fullPathStr + "/.data/thumbnail.png", "PNG");
        }
        
        switch (selectedTemplateIndex) {
            case 1: {
                QFile scriptFile(fullPathStr + "/Dev/Scripts/ThirdPersonController.cs");
                if (scriptFile.open(QIODevice::WriteOnly)) {
                    QTextStream(&scriptFile) << "// Third Person Controller Script\n";
                    scriptFile.close();
                }
                break;
            }
            case 2: {
                 QFile scriptFile(fullPathStr + "/Dev/Scripts/FirstPersonController.cs");
                if (scriptFile.open(QIODevice::WriteOnly)) {
                    QTextStream(&scriptFile) << "// First Person Controller Script\n";
                    scriptFile.close();
                }
                break;
            }
            default: break;
        }

        m_projectPaths.append(fullPathStr);
        saveProjects();
        loadProjects();
        
        ui->projectNameInput->clear();
        ui->projectPathInput->clear();
        ui->mainContent->setCurrentIndex(0);
        
    } catch (const std::filesystem::filesystem_error& e) {
        QMessageBox::critical(this, "Hata", QString("Proje oluşturulamadı: %1").arg(e.what()));
    }
}

void MainWindow::on_projectListWidget_itemSelectionChanged() {
    QListWidgetItem* selectedItem = ui->projectListWidget->currentItem();
    if (!selectedItem) {
        ui->detailsPanel->setVisible(false);
        ui->openProjectButton->setEnabled(false);
        return;
    }
    ui->openProjectButton->setEnabled(true);
    QString projectPath = selectedItem->data(Qt::UserRole).toString();
    updateDetailsPanel(projectPath);
    ui->detailsPanel->setVisible(true);
}

void MainWindow::on_deleteProjectButton_clicked() {
    QListWidgetItem* selectedItem = ui->projectListWidget->currentItem();
    if (!selectedItem) return;

    QString projectPath = selectedItem->data(Qt::UserRole).toString();
    QString projectName = QFileInfo(projectPath).baseName();

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Projeyi Sil", 
        QString("'%1' projesini kalıcı olarak silmek istediğinizden emin misiniz?\nTüm proje dosyaları diskten silinecektir. Bu işlem geri alınamaz!").arg(projectName),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (QDir(projectPath).removeRecursively()) {
            m_projectPaths.removeOne(projectPath);
            saveProjects();
            loadProjects(); // UI'ı yenile
        } else {
            QMessageBox::critical(this, "Hata", "Proje dosyaları silinirken bir hata oluştu.");
        }
    }
}