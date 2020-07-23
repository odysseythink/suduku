#include <QApplication>
#include "ui/main_win.hh"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>

QSqlDatabase g_DB;

static QSqlError Init_Db()
{
    g_DB = QSqlDatabase::addDatabase("QSQLITE");
    g_DB.setDatabaseName("./suduku.db");

    if (!g_DB.open())
        return g_DB.lastError();

    QStringList tables = g_DB.tables();
    QSqlQuery q;
    QString sql;
    if (!tables.contains("tb_puzzle", Qt::CaseInsensitive)){
        sql = "CREATE TABLE tb_puzzle (\
                   id      INTEGER       PRIMARY KEY ASC AUTOINCREMENT\
                                         UNIQUE,\
                   level   INTEGER       DEFAULT (3),\
                   content VARCHAR (512),\
                   solved  BOOLEAN       DEFAULT (0) \
               );";
        if (!q.exec(sql)){
            return q.lastError();
        }
    }

    return QSqlError();
}


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    if (!QSqlDatabase::drivers().contains("QSQLITE")){
        QMessageBox::critical(nullptr, "Unable to load database", "This demo needs the SQLITE driver");
        return -1;
    }

    // initialize the database
    QSqlError err = Init_Db();
    if (err.type() != QSqlError::NoError) {
        QMessageBox::critical(nullptr, "Unable to initialize tables",
                    "Error initializing tables: " + err.text());
        return -1;
    }

    CMainWin* window = new CMainWin;
	window->show();
	return a.exec();
}

