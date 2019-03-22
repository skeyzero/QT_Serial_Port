#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>

#include <QTextEdit>
#include <QByteArray>
#include <QIODevice>
#include <QLineEdit>
#include <QtDebug>
#include <QTimer>
#include <QMessageBox>
#include <QPalette>
#include <QIntValidator>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>



typedef char               s8;
typedef short int          s16;			/* int is 32bit date */
typedef int                s32;

typedef unsigned char               u8;
typedef unsigned short int          u16;			/* int is 32bit date */
typedef unsigned int                u32;


typedef enum _DATA_TYPE{
	HEX_TYPE,
	ASCII_TYPE,
}DATA_TYPE;



class Widget : public QWidget
{
	Q_OBJECT


public:

	u8 flag_serial_open;
	u8 Receive_Buf[1024];
	u16 Rp;
	DATA_TYPE send_type;
	DATA_TYPE receive_type;

	//文本编辑
	QTextEdit*	te_receive_buff;
	QTextEdit*	te_send_buf;

	//按键
	QPushButton*	bt_clear_receive_buf;
	QPushButton*	bt_send;
	QPushButton*	bt_scan_port;
	QPushButton*	bt_open_port;
	QPushButton*	bt_send_type;
	QPushButton*	bt_receive_type;

	//选项
	QComboBox*		cbb_port;
	QComboBox*		cbb_baud;
	QComboBox*		cbb_parity;

	//串口
	QSerialPort*	serial;

	//布局
	QVBoxLayout*	vb_body;
	QHBoxLayout*	hb_set;
	QHBoxLayout*	hb_send;
	QHBoxLayout*	hb_receive;

public slots:
	void bt_handle(void);
	void serial_read(void);
	void Disp_Rx_Data(QByteArray* rx_buf);

public:
	Widget(QWidget *parent = 0);
	~Widget();
};

#endif // WIDGET_H
