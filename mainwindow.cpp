#include "mainwindow.h"
#include "simpledocparser.h"
#include "simpletranslator.h"

#include <QTime>
#include <QFileInfo>
#include <QFileDialog>
#include <QRegularExpression>
#include <QMessageBox>
#include <QFont>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QProgressBar>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_stack(nullptr),
    m_homePage(nullptr),
    m_uploadPage(nullptr),
    m_settingsPage(nullptr),
    m_processPage(nullptr),
    m_resultPage(nullptr),
    m_centralWidget(nullptr),
    m_mainLayout(nullptr),
    m_topLayout(nullptr),
    m_selectFileBtn(nullptr),
    m_fileLabel(nullptr),
    m_sourceLangCombo(nullptr),
    m_targetLangCombo(nullptr),
    m_translateBtn(nullptr),
    m_logTextEdit(nullptr),
    m_bottomLayout(nullptr),
    m_progressBar(nullptr),
    m_resultTextEdit(nullptr),
    m_overallProgressLabel(nullptr),
    m_parser(new SimpleDocParser(this)),
    m_translator(new SimpleTranslator(this)),
    m_isTranslating(false)
{
    setupUI();
    setAcceptDrops(true);
    appendLog("Приложение запущено.");

    connect(m_translator, &SimpleTranslator::translationFinished,
            this, &MainWindow::onTranslationFinished);

    connect(m_translator, &SimpleTranslator::progressUpdated,
            this, &MainWindow::onProgressUpdated);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setWindowTitle("DocTrans");
    setMinimumSize(1100, 700);

    m_stack = new QStackedWidget(this);

    const QString pageBg = "background-color: #f3f4f6;";
    const QString titleStyle = "font-size: 46px; font-weight: 700; color: #111111;";
    const QString cardStyle =
        "background-color: #dde2e8;"
        "border-radius: 18px;";
    const QString primaryButtonStyle =
        "QPushButton {"
        "background-color: #6ca4df;"
        "color: white;"
        "border: none;"
        "border-radius: 16px;"
        "font-size: 20px;"
        "font-weight: 600;"
        "padding: 14px 26px;"
        "}"
        "QPushButton:hover { background-color: #5a97d8; }"
        "QPushButton:disabled { background-color: #b7cde6; color: #eef4fb; }";
    const QString blockTitleStyle = "font-size: 22px; font-weight: 500; color: #222222;";
    const QString textStyle = "font-size: 18px; color: #333333;";

    // ================= 1. ГЛАВНАЯ =================
    m_homePage = new QWidget();
    m_homePage->setStyleSheet(pageBg);
    QVBoxLayout *homeLayout = new QVBoxLayout(m_homePage);
    homeLayout->setContentsMargins(36, 28, 36, 28);
    homeLayout->setSpacing(26);

    QLabel *homeTitle = new QLabel("Главная");
    homeTitle->setStyleSheet(titleStyle);

    QPushButton *newBtn = new QPushButton("➕  Новый перевод");
    newBtn->setFixedHeight(74);
    newBtn->setStyleSheet(
        "QPushButton {"
        "background-color: #6a9ad1;"
        "color: white;"
        "border: none;"
        "border-radius: 18px;"
        "font-size: 24px;"
        "font-weight: 600;"
        "padding: 12px 20px;"
        "}"
        "QPushButton:hover { background-color: #5b8fc9; }"
        );

    QFrame *homeCard = new QFrame();
    homeCard->setStyleSheet(cardStyle);
    QVBoxLayout *homeCardLayout = new QVBoxLayout(homeCard);
    homeCardLayout->setContentsMargins(28, 26, 28, 26);
    homeCardLayout->setSpacing(18);

    QLabel *homeRecentTitle = new QLabel("Последние задачи");
    homeRecentTitle->setStyleSheet("font-size: 28px; font-weight: 500; color: #222222;");

    QTextEdit *homeHistoryText = new QTextEdit();
    homeHistoryText->setReadOnly(true);
    homeHistoryText->setPlaceholderText("История переводов пока пуста");
    homeHistoryText->setMinimumHeight(260);
    homeHistoryText->setStyleSheet(
        "QTextEdit {"
        "background: transparent;"
        "border: none;"
        "font-size: 18px;"
        "color: #444444;"
        "padding: 0px;"
        "}"
        );

    homeCardLayout->addWidget(homeRecentTitle);
    homeCardLayout->addWidget(homeHistoryText);

    homeLayout->addWidget(homeTitle);
    homeLayout->addWidget(newBtn);
    homeLayout->addWidget(homeCard);
    homeLayout->addStretch();

    // ================= 2. ЗАГРУЗКА =================
    m_uploadPage = new QWidget();
    m_uploadPage->setStyleSheet(pageBg);
    QVBoxLayout *uploadLayout = new QVBoxLayout(m_uploadPage);
    uploadLayout->setContentsMargins(36, 28, 36, 28);
    uploadLayout->setSpacing(20);

    QLabel *uploadTitle = new QLabel("Загрузка файла");
    uploadTitle->setStyleSheet(titleStyle);

    QFrame *dropCard = new QFrame();
    dropCard->setStyleSheet(
        "background-color: #e9e8f0;"
        "border-radius: 18px;"
        );
    QHBoxLayout *dropLayout = new QHBoxLayout(dropCard);
    dropLayout->setContentsMargins(28, 24, 28, 24);
    dropLayout->setSpacing(24);

    QLabel *dropText = new QLabel("Перетащите файлы сюда или");
    dropText->setStyleSheet("font-size: 26px; color: #2f2f2f;");

    m_selectFileBtn = new QPushButton("Выберите файл");
    m_selectFileBtn->setFixedSize(270, 72);
    m_selectFileBtn->setStyleSheet(primaryButtonStyle);

    dropLayout->addStretch();
    dropLayout->addWidget(dropText);
    dropLayout->addWidget(m_selectFileBtn);
    dropLayout->addStretch();

    QFrame *filesCard = new QFrame();
    filesCard->setStyleSheet(cardStyle);
    QVBoxLayout *filesLayout = new QVBoxLayout(filesCard);
    filesLayout->setContentsMargins(24, 24, 24, 24);

    m_logTextEdit = new QTextEdit();
    m_logTextEdit->setReadOnly(true);
    m_logTextEdit->setMinimumHeight(330);
    m_logTextEdit->setPlaceholderText("Список добавленных файлов появится здесь...");
    m_logTextEdit->setStyleSheet(
        "QTextEdit {"
        "background: transparent;"
        "border: none;"
        "font-size: 20px;"
        "color: #2f2f2f;"
        "padding: 4px;"
        "}"
        );

    filesLayout->addWidget(m_logTextEdit);

    QPushButton *toSettingsBtn = new QPushButton("Далее");
    toSettingsBtn->setFixedSize(220, 74);
    toSettingsBtn->setStyleSheet(primaryButtonStyle);

    QHBoxLayout *uploadBottomLayout = new QHBoxLayout();
    uploadBottomLayout->addStretch();
    uploadBottomLayout->addWidget(toSettingsBtn);

    uploadLayout->addWidget(uploadTitle);
    uploadLayout->addWidget(dropCard);
    uploadLayout->addWidget(filesCard);
    uploadLayout->addLayout(uploadBottomLayout);

    // ================= 3. НАСТРОЙКИ =================
    m_settingsPage = new QWidget();
    m_settingsPage->setStyleSheet(pageBg);
    QVBoxLayout *settingsLayout = new QVBoxLayout(m_settingsPage);
    settingsLayout->setContentsMargins(36, 28, 36, 28);
    settingsLayout->setSpacing(30);

    QLabel *settingsTitle = new QLabel("Настройки перевода");
    settingsTitle->setStyleSheet(titleStyle);

    QFrame *settingsCard = new QFrame();
    settingsCard->setStyleSheet(
        "background-color: #e9e8f0;"
        "border-radius: 18px;"
        );
    QVBoxLayout *settingsCardLayout = new QVBoxLayout(settingsCard);
    settingsCardLayout->setContentsMargins(30, 30, 30, 30);
    settingsCardLayout->setSpacing(34);

    QLabel *settingsInfo = new QLabel("Выберите исходный и целевой языки");
    settingsInfo->setAlignment(Qt::AlignCenter);
    settingsInfo->setStyleSheet("font-size: 28px; color: #2f2f2f;");

    QHBoxLayout *langLayout = new QHBoxLayout();
    langLayout->setSpacing(36);

    m_sourceLangCombo = new QComboBox();
    m_sourceLangCombo->addItem("английский", "en");
    m_sourceLangCombo->addItem("русский", "ru");
    m_sourceLangCombo->setFixedSize(300, 82);
    m_sourceLangCombo->setStyleSheet(
        "QComboBox {"
        "background: white;"
        "border: 2px solid #91bceb;"
        "border-radius: 16px;"
        "padding: 12px 20px;"
        "font-size: 24px;"
        "color: #222222;"
        "}"
        "QComboBox::drop-down { border: none; width: 40px; }"
        );

    QLabel *swapLabel = new QLabel("⇄");
    swapLabel->setAlignment(Qt::AlignCenter);
    swapLabel->setStyleSheet("font-size: 40px; color: #111111;");

    m_targetLangCombo = new QComboBox();
    m_targetLangCombo->addItem("русский", "ru");
    m_targetLangCombo->addItem("английский", "en");
    m_targetLangCombo->setFixedSize(300, 82);
    m_targetLangCombo->setStyleSheet(
        "QComboBox {"
        "background: white;"
        "border: 2px solid #91bceb;"
        "border-radius: 16px;"
        "padding: 12px 20px;"
        "font-size: 24px;"
        "color: #222222;"
        "}"
        "QComboBox::drop-down { border: none; width: 40px; }"
        );

    langLayout->addWidget(m_sourceLangCombo);
    langLayout->addStretch();
    langLayout->addWidget(swapLabel);
    langLayout->addStretch();
    langLayout->addWidget(m_targetLangCombo);

    settingsCardLayout->addWidget(settingsInfo);
    settingsCardLayout->addLayout(langLayout);

    QPushButton *startBtn = new QPushButton("Начать перевод");
    startBtn->setFixedSize(360, 86);
    startBtn->setStyleSheet(primaryButtonStyle);

    QHBoxLayout *settingsBottomLayout = new QHBoxLayout();
    settingsBottomLayout->addStretch();
    settingsBottomLayout->addWidget(startBtn);

    settingsLayout->addWidget(settingsTitle);
    settingsLayout->addWidget(settingsCard);
    settingsLayout->addStretch();
    settingsLayout->addLayout(settingsBottomLayout);

    // ================= 4. ПРОЦЕСС =================
    m_processPage = new QWidget();
    m_processPage->setStyleSheet(pageBg);
    QVBoxLayout *processLayout = new QVBoxLayout(m_processPage);
    processLayout->setContentsMargins(36, 28, 36, 28);
    processLayout->setSpacing(24);

    QLabel *processTitle = new QLabel("Процесс перевода");
    processTitle->setStyleSheet(titleStyle);

    QFrame *processFilesCard = new QFrame();
    processFilesCard->setStyleSheet(cardStyle);
    QVBoxLayout *processFilesLayout = new QVBoxLayout(processFilesCard);
    processFilesLayout->setContentsMargins(24, 24, 24, 24);

    QTextEdit *processInfoText = new QTextEdit();
    processInfoText->setReadOnly(true);
    processInfoText->setMinimumHeight(320);
    processInfoText->setStyleSheet(
        "QTextEdit {"
        "background: transparent;"
        "border: none;"
        "font-size: 20px;"
        "color: #2c2c2c;"
        "}"
        );
    processInfoText->setPlainText(
        "📄 Список файлов в обработке будет отображаться здесь.\n\n"
        );

    processFilesLayout->addWidget(processInfoText);

    QFrame *overallCard = new QFrame();
    overallCard->setStyleSheet(cardStyle);
    QVBoxLayout *overallLayout = new QVBoxLayout(overallCard);
    overallLayout->setContentsMargins(24, 24, 24, 24);
    overallLayout->setSpacing(20);

    m_progressBar = new QProgressBar();
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setFixedHeight(34);
    m_progressBar->setStyleSheet(
        "QProgressBar {"
        "background: #d4d4d4;"
        "border: none;"
        "border-radius: 12px;"
        "text-align: center;"
        "font-size: 18px;"
        "color: transparent;"
        "}"
        "QProgressBar::chunk {"
        "background-color: #6ca4df;"
        "border-radius: 12px;"
        "}"
        );

    m_overallProgressLabel = new QLabel("Общий прогресс: 0%");
    m_overallProgressLabel->setAlignment(Qt::AlignCenter);
    m_overallProgressLabel->setStyleSheet("font-size: 24px; color: #222222;");

    QPushButton *stopBtn = new QPushButton("Стоп");
    stopBtn->setFixedSize(220, 74);
    stopBtn->setStyleSheet(primaryButtonStyle);

    QHBoxLayout *stopLayout = new QHBoxLayout();
    stopLayout->addStretch();
    stopLayout->addWidget(stopBtn);

    overallLayout->addWidget(m_progressBar);
    overallLayout->addWidget(m_overallProgressLabel);
    overallLayout->addLayout(stopLayout);

    processLayout->addWidget(processTitle);
    processLayout->addWidget(processFilesCard);
    processLayout->addWidget(overallCard);

    // ================= 5. РЕЗУЛЬТАТ =================
    m_resultPage = new QWidget();
    m_resultPage->setStyleSheet(pageBg);
    QVBoxLayout *resultLayout = new QVBoxLayout(m_resultPage);
    resultLayout->setContentsMargins(36, 28, 36, 28);
    resultLayout->setSpacing(24);

    QLabel *resultTitle = new QLabel("Результаты");
    resultTitle->setStyleSheet(titleStyle);

    QFrame *resultCard = new QFrame();
    resultCard->setStyleSheet(
        "background-color: #dde2e8;"
        "border: 3px solid #36a2f0;"
        "border-radius: 18px;"
        );
    QVBoxLayout *resultCardLayout = new QVBoxLayout(resultCard);
    resultCardLayout->setContentsMargins(24, 24, 24, 24);

    m_resultTextEdit = new QTextEdit();
    m_resultTextEdit->setReadOnly(true);
    m_resultTextEdit->setMinimumHeight(340);
    m_resultTextEdit->setStyleSheet(
        "QTextEdit {"
        "background: transparent;"
        "border: none;"
        "font-size: 22px;"
        "color: #2c2c2c;"
        "}"
        );
    m_resultTextEdit->setPlaceholderText("Здесь появятся результаты перевода...");

    resultCardLayout->addWidget(m_resultTextEdit);

    QPushButton *backBtn = new QPushButton("Вернуться на главную страницу");
    backBtn->setFixedSize(470, 86);
    backBtn->setStyleSheet(primaryButtonStyle);

    QHBoxLayout *resultBottomLayout = new QHBoxLayout();
    resultBottomLayout->addStretch();
    resultBottomLayout->addWidget(backBtn);

    resultLayout->addWidget(resultTitle);
    resultLayout->addWidget(resultCard);
    resultLayout->addStretch();
    resultLayout->addLayout(resultBottomLayout);

    // ================= СКРЫТЫЕ СЛУЖЕБНЫЕ ЭЛЕМЕНТЫ =================
    m_fileLabel = new QLabel("Файл не выбран");

    m_translateBtn = new QPushButton("Перевести");
    m_translateBtn->setEnabled(false);

    // ================= СТЕК =================
    m_stack->addWidget(m_homePage);
    m_stack->addWidget(m_uploadPage);
    m_stack->addWidget(m_settingsPage);
    m_stack->addWidget(m_processPage);
    m_stack->addWidget(m_resultPage);

    setCentralWidget(m_stack);

    // ================= ПЕРЕХОДЫ =================
    connect(newBtn, &QPushButton::clicked, this, [this]() {
        m_stack->setCurrentWidget(m_uploadPage);
    });

    connect(toSettingsBtn, &QPushButton::clicked, this, [this]() {
        if (m_fileQueue.isEmpty()) {
            appendLog("Добавьте файлы перед продолжением.");
            return;
        }
        m_stack->setCurrentWidget(m_settingsPage);
    });

    connect(startBtn, &QPushButton::clicked, this, [this]() {
        if (m_fileQueue.isEmpty()) {
            QMessageBox::warning(this, "Нет файлов", "Сначала добавьте файлы для перевода.");
            m_stack->setCurrentWidget(m_uploadPage);
            return;
        }

        m_stack->setCurrentWidget(m_processPage);
        onTranslateClicked();
    });

    connect(backBtn, &QPushButton::clicked, this, [this]() {
        m_stack->setCurrentWidget(m_homePage);
    });

    connect(stopBtn, &QPushButton::clicked, this, [this]() {
        appendLog("Остановка перевода пользователем не реализована.");
        QMessageBox::information(this, "Стоп", "Остановка пока не реализована.");
    });

    connect(m_selectFileBtn, &QPushButton::clicked,
            this, &MainWindow::onSelectFileClicked);

    connect(m_translateBtn, &QPushButton::clicked,
            this, &MainWindow::onTranslateClicked);
}

void MainWindow::appendLog(const QString &message)
{
    if (m_logTextEdit) {
        m_logTextEdit->append(
            QString("[%1] %2")
                .arg(QTime::currentTime().toString("hh:mm:ss"), message));
    }

    if (m_resultTextEdit && message.contains("Готово:")) {
        m_resultTextEdit->append("✅ " + message.section("Готово: ", 1));
    }
    if (m_resultTextEdit && message.contains("Ошибка")) {
        m_resultTextEdit->append("❌ " + message);
    }
}

void MainWindow::onSelectFileClicked()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this,
        "Выберите файлы (можно несколько)",
        "",
        "Files (*.txt *.docx *.pdf)");

    if (filePaths.isEmpty())
        return;

    for (const QString &filePath : filePaths) {
        m_fileQueue.enqueue(filePath);
        appendLog("Добавлен в очередь: " + QFileInfo(filePath).fileName());
    }

    if (!m_fileQueue.isEmpty()) {
        m_fileLabel->setText(QString("%1 файлов в очереди").arg(m_fileQueue.size()));
        m_translateBtn->setEnabled(true);
    }
}

void MainWindow::onTranslateClicked()
{
    if (m_fileQueue.isEmpty()) {
        appendLog("Нет файлов в очереди. Добавьте файлы через кнопку или Drag&Drop.");
        return;
    }

    if (m_isTranslating) {
        appendLog("Перевод уже выполняется. Дождитесь окончания.");
        return;
    }

    appendLog(QString("Начинаем перевод %1 файлов...").arg(m_fileQueue.size()));
    m_translateBtn->setEnabled(false);
    m_resultTextEdit->clear();
    processNextFile();
}

void MainWindow::onTranslationFinished(const QString &translatedText, bool success, const QString &error)
{
    if (!success) {
        appendLog("Ошибка перевода: " + error);
        m_progressBar->setVisible(false);
        processNextFile();
        return;
    }

    QFileInfo info(m_currentFilePath);
    QString outputPath;

    if (info.suffix() == "docx") {
        outputPath = info.path() + "/" + info.completeBaseName() + "_translated.docx";
    }
    else if (info.suffix() == "pdf") {
        outputPath = info.path() + "/" + info.completeBaseName() + "_translated.pdf";
    }
    else {
        outputPath = info.path() + "/" + info.completeBaseName() + "_translated.txt";
    }

    if (!m_parser->build(translatedText, outputPath)) {
        appendLog("Ошибка сохранения");
        m_progressBar->setVisible(false);
        processNextFile();
        return;
    }

    appendLog("Готово: " + outputPath);
    m_progressBar->setValue(100);
    if (m_overallProgressLabel)
        m_overallProgressLabel->setText("Общий прогресс: 100%");
    m_progressBar->setVisible(false);


    processNextFile();
}

void MainWindow::onProgressUpdated(int percent)
{
    if (m_progressBar) {
        m_progressBar->setVisible(true);
        m_progressBar->setValue(percent);
    }

    if (m_overallProgressLabel) {
        m_overallProgressLabel->setText(QString("Общий прогресс: %1%").arg(percent));
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();

    if (urls.isEmpty()) {
        appendLog("Drag&Drop: не удалось получить файл");
        return;
    }

    int addedCount = 0;
    for (const QUrl &url : urls) {
        QString filePath = url.toLocalFile();

        if (filePath.endsWith(".docx", Qt::CaseInsensitive) ||
            filePath.endsWith(".pdf", Qt::CaseInsensitive) ||
            filePath.endsWith(".txt", Qt::CaseInsensitive)) {

            m_fileQueue.enqueue(filePath);
            appendLog("Добавлен в очередь (Drag&Drop): " + QFileInfo(filePath).fileName());
            addedCount++;
        } else {
            appendLog("Пропущен (неподдерживаемый формат): " + QFileInfo(filePath).fileName());
        }
    }

    if (addedCount > 0) {
        m_fileLabel->setText(QString("%1 файлов в очереди").arg(m_fileQueue.size()));
        m_translateBtn->setEnabled(true);
    }
}

void MainWindow::processNextFile()
{
    if (m_fileQueue.isEmpty()) {
        m_isTranslating = false;
        m_progressBar->setVisible(false);
        m_translateBtn->setEnabled(true);
        m_fileLabel->setText("Файл не выбран");

        m_stack->setCurrentWidget(m_resultPage);

        return;
    }

    m_isTranslating = true;
    m_currentFilePath = m_fileQueue.dequeue();

    QString fileName = QFileInfo(m_currentFilePath).fileName();
    appendLog(QString("Обработка файла %1 (осталось в очереди: %2)")
                  .arg(fileName)
                  .arg(m_fileQueue.size()));

    m_fileLabel->setText(fileName);
    translateCurrentFile();
}

void MainWindow::translateCurrentFile()
{
    QString text;
    QString fmt;

    if (!m_parser->parse(m_currentFilePath, text, fmt)) {
        appendLog("Ошибка чтения файла");
        processNextFile();
        return;
    }

    // Автоопределение языка
    QString lower = text.toLower();
    if (lower.contains(QRegularExpression("[a-z]"))) {
        m_sourceLangCombo->setCurrentIndex(0); // английский
        m_targetLangCombo->setCurrentIndex(0); // русский
    } else {
        m_sourceLangCombo->setCurrentIndex(1); // русский
        m_targetLangCombo->setCurrentIndex(1); // английский
    }

    QString sourceCode = m_sourceLangCombo->currentData().toString();
    QString targetCode = m_targetLangCombo->currentData().toString();

    m_progressBar->setVisible(true);
    m_progressBar->setValue(10);

    m_translator->translate(text, sourceCode, targetCode);
}