
#include <librsvg/rsvg.h>
#include <cairo.h>
#include "mainwindow.h"
#include "CairoCompilerLib.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QSvgRenderer>
#include <QPixmap>
#include <QPainter>
#include <QKeyEvent>
#include <QTextCursor>
#include <QTextBlock>



#include <QImage>

extern "C"{

QImage renderSVGWithLibrsvg(const QByteArray &svgData, int width, int height) {
    GError *error = nullptr;
    // 1. Charger le flux SVG en mémoire
    RsvgHandle *handle = rsvg_handle_new_from_data(
        reinterpret_cast<const guint8*>(svgData.constData()),
        svgData.size(),
        &error
    );

    if (!handle) {
        // Gérer l'erreur si besoin
        return QImage();
    }

    // 2. Créer la surface Cairo destination
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *cr = cairo_create(surface);

    // 3. Rendu du SVG via librsvg (gestion complète des clipPath)
    RsvgRectangle viewport = { 0.0, 0.0, static_cast<double>(width), static_cast<double>(height) };
    rsvg_handle_render_document(handle, cr, &viewport, &error);

    cairo_surface_flush(surface);

    // 4. Conversion vers QImage (copie propre pour détruire la surface Cairo)
    QImage result(
        cairo_image_surface_get_data(surface),
        width,
        height,
        cairo_image_surface_get_stride(surface),
        QImage::Format_ARGB32_Premultiplied
    );
    QImage finalImage = result.copy(); // Copie indépendante du buffer C

    // Nettoyage
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    g_object_unref(handle);

    return finalImage;
}
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
    applyDarkStyle();
    clearOutput();
}

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);

    // =========================================================
    // PANNEAU GAUCHE : ÉDITEUR
    // =========================================================
    QWidget *editorPanel = new QWidget(this);
    QVBoxLayout *editorLayout = new QVBoxLayout(editorPanel);
    editorLayout->setContentsMargins(0, 0, 0, 0);
    editorLayout->setSpacing(0);

    // Header Éditeur
    QWidget *editorHeader = new QWidget(this);
    editorHeader->setObjectName("editorHeader");
    editorHeader->setFixedHeight(45);
    QHBoxLayout *editorHeaderLayout = new QHBoxLayout(editorHeader);

    QLabel *titleLabel = new QLabel("Cairo DSL", this);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin-right: 10px;");

    filenameLabel = new QLabel("Aucun fichier", this);
    filenameLabel->setStyleSheet("color: #aaa; font-family: monospace; font-size: 12px;");

    openBtn = new QPushButton("Ouvrir", this);
    saveBtn = new QPushButton("Enregistrer", this);
    saveasBtn = new QPushButton("Enregistrer Sous", this);
    sendBtn = new QPushButton("Envoyer", this);
    sendBtn->setObjectName("sendButton");
    sendBtn->setEnabled(false);

    editorHeaderLayout->addWidget(titleLabel);
    editorHeaderLayout->addWidget(filenameLabel, 1);
    editorHeaderLayout->addWidget(openBtn);
    editorHeaderLayout->addWidget(saveBtn);
    editorHeaderLayout->addWidget(saveasBtn);
    editorHeaderLayout->addWidget(sendBtn);

    // Zone d'édition
    editor = new QPlainTextEdit(this);
    editor->setPlaceholderText("Entrez votre code ici...");
    editor->setTabStopDistance(4 * fontMetrics().horizontalAdvance(' '));

    editorLayout->addWidget(editorHeader);
    editorLayout->addWidget(editor);

    // =========================================================
    // PANNEAU DROIT
    // =========================================================
    QWidget *rightPanel = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    QSplitter *rightSplitter = new QSplitter(Qt::Vertical, this);

    // Panel Résultat
    QWidget *outputPanel = new QWidget(this);
    QVBoxLayout *outputLayout = new QVBoxLayout(outputPanel);
    outputLayout->setContentsMargins(0, 0, 0, 0);
    outputLayout->setSpacing(0);

    QWidget *outputHeader = new QWidget(this);
    outputHeader->setObjectName("panelHeader");
    outputHeader->setFixedHeight(40);
    QHBoxLayout *outputHeaderLayout = new QHBoxLayout(outputHeader);

    QLabel *outputTitle = new QLabel("Résultat", this);
    outputTitle->setStyleSheet("font-weight: bold;");
    saveOutputBtn = new QPushButton("Enregistrer", this);
    saveOutputBtn->setEnabled(false);

    outputHeaderLayout->addWidget(outputTitle, 1);
    outputHeaderLayout->addWidget(saveOutputBtn);

    outputDisplay = new QLabel(this);
    outputDisplay->setAlignment(Qt::AlignCenter);
    outputDisplay->setStyleSheet("background-color: #ffffff; color: #777777;");

    outputScrollArea = new QScrollArea(this);
    outputScrollArea->setWidget(outputDisplay);
    outputScrollArea->setWidgetResizable(true);

    outputLayout->addWidget(outputHeader);
    outputLayout->addWidget(outputScrollArea);

    // Panel Messages
    QWidget *messagesPanel = new QWidget(this);
    QVBoxLayout *messagesLayout = new QVBoxLayout(messagesPanel);
    messagesLayout->setContentsMargins(0, 0, 0, 0);
    messagesLayout->setSpacing(0);

    QWidget *messagesHeader = new QWidget(this);
    messagesHeader->setObjectName("panelHeader");
    messagesHeader->setFixedHeight(40);
    QHBoxLayout *messagesHeaderLayout = new QHBoxLayout(messagesHeader);
    QLabel *messagesTitle = new QLabel("Messages", this);
    messagesTitle->setStyleSheet("font-weight: bold;");
    messagesHeaderLayout->addWidget(messagesTitle);

    messagesList = new QListWidget(this);

    messagesLayout->addWidget(messagesHeader);
    messagesLayout->addWidget(messagesList);

    rightSplitter->addWidget(outputPanel);
    rightSplitter->addWidget(messagesPanel);
    rightSplitter->setStretchFactor(0, 65);
    rightSplitter->setStretchFactor(1, 35);

    rightLayout->addWidget(rightSplitter);

    mainSplitter->addWidget(editorPanel);
    mainSplitter->addWidget(rightPanel);
    mainLayout->addWidget(mainSplitter);

    // Signal / Slots Connects
    connect(openBtn, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::saveSource);
    connect(saveasBtn, &QPushButton::clicked, this, &MainWindow::saveSourceAs);
    connect(sendBtn, &QPushButton::clicked, this, &MainWindow::processCode);
    connect(saveOutputBtn, &QPushButton::clicked, this, &MainWindow::saveOutput);
    connect(messagesList, &QListWidget::itemClicked, this, &MainWindow::onMessageClicked);
    connect(editor, &QPlainTextEdit::textChanged, this, &MainWindow::onTextChanged);

    resize(1100, 700);
}

void MainWindow::applyDarkStyle() {
    QString style = R"(
        QMainWindow { background-color: #202124; color: #eee; }
        QWidget { color: #eee; font-family: sans-serif; }
        
        #editorHeader, #panelHeader {
            background-color: #292a2d;
            border-bottom: 1px solid #444;
            padding: 0 5px;
        }

        QPlainTextEdit {
            background-color: #1e1f22;
            color: #eee;
            font-family: monospace;
            font-size: 14px;
            border: none;
        }

        QPushButton {
            background-color: #555;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 5px 12px;
            margin-left: 4px;
        }

		QPushButton:pressed {
			background-color: #3d3d3d;
			padding-top: 6px;
			padding-bottom: 4px;
		}

        QPushButton:hover { background-color: #666; }
        QPushButton:disabled { background-color: #444; color: #888; }

        #sendButton { background-color: #4caf50; font-weight: bold; }
        #sendButton:hover { background-color: #5bc660; }

        QListWidget {
            background-color: #1e1f22;
            border: none;
            font-family: monospace;
            font-size: 13px;
        }
        QListWidget::item { padding: 6px; margin-bottom: 2px; }
        QListWidget::item:hover { background-color: #383a40; }

        QSplitter::handle { background-color: #444; }
    )";
    this->setStyleSheet(style);
}



// Raccourcis clavier (Ctrl+O, Ctrl+S, Ctrl+Enter)
void MainWindow::onTextChanged() {
    if(editor->toPlainText().isEmpty() == false)
    	sendBtn->setEnabled(true);
    else
    	sendBtn->setEnabled(false);
}

// Raccourcis clavier (Ctrl+O, Ctrl+S, Ctrl+Enter)
void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() & Qt::ControlModifier || event->modifiers() & Qt::MetaModifier) {
        if (event->key() == Qt::Key_O) {
            openFile();
            return;
        } else if (event->key() == Qt::Key_S) {
            saveSource();
            return;
        } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        	if(editor->toPlainText().isEmpty() == false)
        		processCode();
            return;
        }
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::openFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", "", "Cairo Files (*.cairo);;Text Files (*.txt)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        editor->setPlainText(in.readAll());
        file.close();

        currentFilePath = filePath;
        qDebug() << "Current Filepath : " << currentFilePath;
        filenameLabel->setText(QFileInfo(filePath).fileName());
        clearOutput();
    } else {
        addLocalMessage("Error", "Impossible de lire le fichier.");
    }
}

void MainWindow::saveSource() {
    QString filePath = currentFilePath;
    if (filePath.isEmpty()) {
        filePath = QFileDialog::getSaveFileName(this, "Enregistrer sous", "untitled.cairo", "Cairo Files (*.cairo)");
        if (filePath.isEmpty()) return;
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << editor->toPlainText();
        file.close();

        currentFilePath = filePath;
        filenameLabel->setText(QFileInfo(filePath).fileName());
    } else {
        addLocalMessage("Error", "Impossible d'enregistrer le fichier.");
    }
}

void MainWindow::saveSourceAs() {
	QString filePath = QFileDialog::getSaveFileName(this, "Enregistrer sous", "untitled.cairo", "Cairo Files (*.cairo)");
	if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << editor->toPlainText();
        file.close();

        currentFilePath = filePath;
        filenameLabel->setText(QFileInfo(filePath).fileName());
    } else {
        addLocalMessage("Error", "Impossible d'enregistrer le fichier.");
    }
}

// Remplacement du Fetch CGI par l'appel Bibliothèque
void MainWindow::processCode() {
    clearMessages();
    addLocalMessage("Run", "Traitement en cours par la bibliothèque C++...");

    // Appel direct à la lib C++ au lieu de la requête HTTP
    QString msgs;
    QByteArray out;
    bool is_xml = true;
    int ret = processlib(editor->toPlainText(), currentFilePath, msgs, out, is_xml, &width, &height);
    
    processXmlResponse(msgs, out, is_xml, ret);
}

void MainWindow::processXmlResponse(const QString &xmlText, const QByteArray &out, bool is_xml, int ret) {
    QDomDocument doc;
    if (!doc.setContent(xmlText)) {
        clearOutput();
        addLocalMessage("ParseError", "XML invalide retourné par la bibliothèque.");
        return;
    }

    if(ret == 0)
    {
		if (is_xml) {
			displayOutputSvg(out);
		} else {
			displayOutputPng(out);
		}
    }

    // Traitement des messages (<msgs><msg>...)
    clearMessages();
    QDomNodeList msgNodes = doc.elementsByTagName("msg");
    for (int i = 0; i < msgNodes.count(); ++i) {
        QDomElement msgElem = msgNodes.at(i).toElement();
        MessageItem item;
        item.state = msgElem.firstChildElement("running-state").text();
        item.text = msgElem.firstChildElement("text").text();

        bool ok;
        int val = msgElem.firstChildElement("pos").firstChildElement("first_line").text().toInt(&ok);
        if (ok) item.firstLine = val;
        val = msgElem.firstChildElement("pos").firstChildElement("first_column").text().toInt(&ok);
        if (ok) item.firstColumn = val;
        val = msgElem.firstChildElement("pos").firstChildElement("last_line").text().toInt(&ok);
        if (ok) item.lastLine = val;
        val = msgElem.firstChildElement("pos").firstChildElement("last_column").text().toInt(&ok);
        if (ok) item.lastColumn = val;

        addMessage(item);
    }
}

void MainWindow::displayOutputSvg(const QByteArray &svgData) {

	QImage image = renderSVGWithLibrsvg(svgData, width, height);

    /*QSvgRenderer renderer(svgData);
    if (!renderer.isValid()) {
        addLocalMessage("ParseError", "Le contenu SVG est invalide.");
        return;
    }

    QImage img(width, height, QImage::Format_ARGB32);
    img.fill(Qt::white);
    QPainter painter(&img);
    renderer.render(&painter);

    outputDisplay->setPixmap(QPixmap::fromImage(img));*/

    outputDisplay->setPixmap(QPixmap::fromImage(image));
    currentOutputData = svgData;
    currentOutputType = "svg";
    saveOutputBtn->setEnabled(true);
}

void MainWindow::displayOutputPng(const QByteArray &pngData) {
    QPixmap pixmap;
    if (!pixmap.loadFromData(pngData, "PNG")) {
        addLocalMessage("Error", "Impossible de lire le rendu PNG.");
        saveOutputBtn->setEnabled(false);
        return;
    }

    outputDisplay->setPixmap(pixmap);
    currentOutputData = pngData;
    currentOutputType = "png";
    saveOutputBtn->setEnabled(true);
}

void MainWindow::saveOutput() {
    if (currentOutputData.isEmpty()) return;

    QString filter = (currentOutputType == "svg") ? "SVG Image (*.svg)" : "PNG Image (*.png)";
    QString defaultName = QFileInfo(currentFilePath).completeBaseName();
    if (defaultName.isEmpty()) defaultName = "output";
    defaultName += "." + currentOutputType;

    QString filePath = QFileDialog::getSaveFileName(this, "Enregistrer le résultat", defaultName, filter);
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(currentOutputData);
        file.close();
    }
}

void MainWindow::clearMessages() {
    messagesList->clear();
}

void MainWindow::addMessage(const MessageItem &msg) {
    QString stateIcon = "•";
    QString borderStyle = "border-left: 3px solid #777;";

    QString normState = msg.state.toLower();
    if (normState == "ready" || normState == "finished") {
        stateIcon = "✓";
        borderStyle = "border-left: 3px solid #4caf50;";
    } else if (normState == "error" || normState == "parseerror") {
        stateIcon = "✗";
        borderStyle = "border-left: 3px solid #f44336; background-color: #351f20;";
    } else if (normState == "run") {
        stateIcon = "▶";
    }

    QString posStr;
    if (msg.firstLine != -1) {
        if (msg.lastLine == -1) {
            posStr = QString("ligne %1:%2").arg(msg.firstLine).arg(msg.firstColumn);
        } else {
            posStr = QString("L%1:%2 → L%3:%4").arg(msg.firstLine).arg(msg.firstColumn).arg(msg.lastLine).arg(msg.lastColumn);
        }
    }

    QListWidgetItem *item = new QListWidgetItem(messagesList);
    item->setText(QString("%1  %2  %3\n%4").arg(stateIcon, msg.state, posStr, msg.text));
    
    // Sauvegarde des données de position dans l'élément de liste
    item->setData(Qt::UserRole + 1, msg.firstLine);
    item->setData(Qt::UserRole + 2, msg.firstColumn);
    item->setData(Qt::UserRole + 3, msg.lastLine);
    item->setData(Qt::UserRole + 4, msg.lastColumn);

    messagesList->addItem(item);
}

void MainWindow::addLocalMessage(const QString &state, const QString &text) {
    MessageItem item;
    item.state = state;
    item.text = text;
    addMessage(item);
}

// Clic sur un message -> Selectionne le code correspondant dans l'éditeur
void MainWindow::onMessageClicked(QListWidgetItem *item) {
    int firstLine = item->data(Qt::UserRole + 1).toInt();
    int firstColumn = item->data(Qt::UserRole + 2).toInt();
    int lastLine = item->data(Qt::UserRole + 3).toInt();
    int lastColumn = item->data(Qt::UserRole + 4).toInt();

    if (firstLine > 0) {
        selectEditorPosition(firstLine, firstColumn, lastLine, lastColumn + 1);//last column est le début de la dernière colonne. +1 est la fin de la colonne, début de la suivante. Pour entourer la sélection, besoin de la fin !
    }
}

void MainWindow::selectEditorPosition(int firstLine, int firstColumn, int lastLine, int lastColumn) {
    QTextDocument *doc = editor->document();
    
    QTextBlock startBlock = doc->findBlockByLineNumber(firstLine - 1);
    int startPos = startBlock.position() + qMax(0, firstColumn - 1);

    int endPos = startPos;
    if (lastLine > 0 && lastColumn > 0) {
        QTextBlock endBlock = doc->findBlockByLineNumber(lastLine - 1);
        endPos = endBlock.position() + qMax(0, lastColumn - 1);
    }

    QTextCursor cursor(doc);
    cursor.setPosition(startPos);
    cursor.setPosition(endPos, QTextCursor::KeepAnchor);
    
    editor->setTextCursor(cursor);
    editor->setFocus();
}

// Vide le conteneur de résultat et réinitialise l'état
void MainWindow::clearOutput() {
    // 1. Vide le contenu de la zone d'affichage
    outputDisplay->clear();

    // 2. Affiche le placeholder
    showOutputPlaceholder("Aucun résultat");

    // 3. Réinitialise les variables de sortie
    currentOutputData.clear();
    currentOutputType.clear();

    // 4. Désactive le bouton de sauvegarde
    saveOutputBtn->setEnabled(false);
}

// Affiche un texte indicatif dans le composant de rendu
void MainWindow::showOutputPlaceholder(const QString &text) {
    outputDisplay->setText(text);
    outputDisplay->setStyleSheet("color: #777777; font-style: italic; background-color: #ffffff;");
    outputDisplay->setAlignment(Qt::AlignCenter);
}

// Met à jour le label affichant le nom du fichier ouvert
void MainWindow::updateFilename() {
    if (currentFilePath.isEmpty()) {
        filenameLabel->setText("Aucun fichier");
    } else {
        filenameLabel->setText(QFileInfo(currentFilePath).fileName());
    }
}
