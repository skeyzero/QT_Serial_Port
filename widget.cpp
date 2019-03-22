#include "widget.h"


void StringToHex(QString str, QByteArray &senddata);

//串口

Widget::Widget(QWidget *parent)
	: QWidget(parent)
{

	//参数初始化
	flag_serial_open = 0;
	send_type = HEX_TYPE;
	receive_type = HEX_TYPE;

	//按键初始化
	bt_send = new QPushButton("Send");
	bt_open_port = new QPushButton("Open");
	bt_scan_port = new QPushButton("Scan");
	bt_clear_receive_buf = new QPushButton("Clear");
	bt_send_type = new QPushButton("HEX");
	bt_receive_type = new QPushButton("HEX");

	//选项初始化
	cbb_baud = new QComboBox();
	cbb_baud->addItem("9600");
	cbb_baud->addItem("19200");
	cbb_baud->addItem("115200");
	cbb_baud->addItem("4800");
	cbb_baud->addItem("38400");
	cbb_baud->addItem("57600");

	cbb_parity = new QComboBox();
	cbb_parity->addItem("NoParity");
	cbb_parity->addItem("EvenParity");
	cbb_parity->addItem("OddParity");

	cbb_port =  new QComboBox();


	//文本编辑初始化
	te_send_buf = new QTextEdit();
	//te_receive_buff = new QTextEdit();
	te_receive_buff = new QPlainTextEdit();
	te_receive_buff->setBaseSize(500,200);
	te_send_buf->setBaseSize(500,100);

	//串口初始化
	serial =  new QSerialPort;

	foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())//搜索可用串口号，并将串口号记录在comboBox中
	{
	   serial->setPort(info);
		cbb_port->addItem(serial->portName());
	}

	//信号槽处理
	connect(bt_scan_port,SIGNAL(clicked(bool)),this,SLOT(bt_handle()));
	connect(bt_open_port,SIGNAL(clicked(bool)),this,SLOT(bt_handle()));
	connect(bt_clear_receive_buf,SIGNAL(clicked(bool)),this,SLOT(bt_handle()));
	connect(bt_send,SIGNAL(clicked(bool)),this,SLOT(bt_handle()));
	connect(bt_send_type,SIGNAL(clicked(bool)),this,SLOT(bt_handle()));
	connect(bt_receive_type,SIGNAL(clicked(bool)),this,SLOT(bt_handle()));
	connect(serial,SIGNAL(readyRead()),this,SLOT(serial_read()));				//串口接收
	//布局
	vb_body = new QVBoxLayout();
	hb_set = new QHBoxLayout();
	hb_send = new QHBoxLayout();
	hb_receive = new QHBoxLayout();

	hb_set->addWidget(cbb_baud);
	hb_set->addWidget(cbb_parity);
	hb_set->addWidget(cbb_port);
	hb_set->addWidget(bt_scan_port);
	hb_set->addWidget(bt_open_port);

	hb_send->addWidget(bt_send_type);
	hb_send->addWidget(bt_send);

	hb_receive->addWidget(bt_receive_type);
	hb_receive->addWidget(bt_clear_receive_buf);

	vb_body->addWidget(te_receive_buff);
	vb_body->addLayout(hb_receive);
	vb_body->addWidget(te_send_buf);
	vb_body->addLayout(hb_send);
	vb_body->addLayout(hb_set);

	setLayout(vb_body);

}


Widget::~Widget()
{

}

void Widget::bt_handle()
{
	if (QPushButton* btn = dynamic_cast<QPushButton*>(sender()))
	{
		//扫描串口
		if(btn == bt_scan_port)
		{
			if(flag_serial_open == 0)
			{
				cbb_port->clear();
				foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())//搜索可用串口号，并将串口号记录在comboBox中
				{
					serial->setPort(info);
					cbb_port->addItem(serial->portName());
				}
			}
		}
		else if(btn == bt_open_port)
		{
			if(flag_serial_open == 0)
			{
				serial->setPortName(cbb_port->currentText());
				if(serial->open(QIODevice::ReadWrite))
				{
					te_receive_buff->appendPlainText("Open Success \n");
					//te_receive_buff->append("Open Success \n");
					bt_open_port->setText("Close");
					flag_serial_open = 1;

					//波特率选择
					if(cbb_baud->currentText() == "9600")
					{
						serial->setBaudRate(QSerialPort::Baud9600);
					}
					else if(cbb_baud->currentText() == "19200")
					{
						serial->setBaudRate(QSerialPort::Baud19200);
					}
					else if(cbb_baud->currentText() == "38400")
					{
						serial->setBaudRate(QSerialPort::Baud38400);
					}
					else if(cbb_baud->currentText() == "57600")
					{
						serial->setBaudRate(QSerialPort::Baud57600);
					}
					else if(cbb_baud->currentText() == "115200")
					{
						serial->setBaudRate(QSerialPort::Baud115200);
					}
					else if(cbb_baud->currentText() == "4800")
					{
						serial->setBaudRate(QSerialPort::Baud4800);
					}

					//校验位选择
					if(cbb_parity->currentText() == "NoParity")
					{
						serial->setParity(QSerialPort::NoParity);
					}
					else if(cbb_parity->currentText() == "EvenParity")
					{
						serial->setParity(QSerialPort::EvenParity);
					}
					else if(cbb_parity->currentText() == "OddParity")
					{
						serial->setParity(QSerialPort::OddParity);
					}

					serial->setDataBits(QSerialPort::Data8);
					serial->setStopBits(QSerialPort::OneStop);
					serial->setFlowControl(QSerialPort::NoFlowControl);
					serial->clearError();
					serial->clear();
				}
				else
				{
					te_receive_buff->appendPlainText("Open Failed\n");
					//te_receive_buff->append("Open Failed\n");
				}
			}
			else
			{
				bt_open_port->setText("Open");
				flag_serial_open = 0;
				serial->close();
			}
		}
		else if(btn == bt_send)
		{
			if(send_type == HEX_TYPE)
			{
				u16 num;
				u16 i;
				QByteArray send_data;
				QString str = te_send_buf->toPlainText();//Read the data to send
				int len = str.length();
				QString send_str;
				num = 0;
				for(i=0;i<len;i++)
				{
					if(str.at(i)!= ' ')
					{
						send_str[num] = str.at(i);
						num ++;
					}
				}
				StringToHex(str,send_data);//将str字符串转换为16进制的形式
				send_data.resize(num/2);
				serial->write(send_data);
			}
			else
			{
				QByteArray send_data;
				QString str = te_send_buf->toPlainText();//Read the data to send
				send_data = str.toUtf8();
				serial->write(send_data);
			}
		}
		else if(btn == bt_clear_receive_buf)
		{
			te_receive_buff->clear();
		}
		else if(btn == bt_send_type)
		{
			if(send_type == HEX_TYPE)
			{
				send_type = ASCII_TYPE;
				bt_send_type->setText("ASCII");
			}
			else
			{
				send_type = HEX_TYPE;
				bt_send_type->setText("HEX");
			}
		}
		else if(btn == bt_receive_type)
		{
			if(receive_type == HEX_TYPE)
			{
				receive_type = ASCII_TYPE;
				bt_receive_type->setText("ASCII");
			}
			else
			{
				receive_type = HEX_TYPE;
				bt_receive_type->setText("HEX");
			}
		}

	}
}


void Widget::serial_read()
{
	QByteArray buf;
	buf = serial->readAll();//读出的数据未必完整
	if(receive_type == HEX_TYPE)
	{
		QDataStream out(&buf,QIODevice::ReadWrite);
		qint8 outChar = 0;
		while(!out.atEnd())
		{
			out>>outChar;   //每字节填充一次，直到结束
			//十六进制的转换
			QString str = QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));
			te_receive_buff->moveCursor(QTextCursor::End);
			te_receive_buff->insertPlainText(str.toUpper());//大写
			te_receive_buff->moveCursor(QTextCursor::End);
			te_receive_buff->insertPlainText(" ");//每发送两个字符后添加一个空格
		}
	}
	else
	{
		QString str;
		str = buf;
		te_receive_buff->insertPlainText(str);//大写
		QTextCursor cursor = te_receive_buff->textCursor();
		cursor.movePosition(QTextCursor::End);
		te_receive_buff->setTextCursor(cursor);
	}
}




/*将十六进制转换为字符*/
char ConvertHexChar(char ch)
{
	if((ch >= '0') && (ch <= '9'))
		return ch-0x30;
	else if((ch >= 'A') && (ch <= 'F'))
		return ch-'A'+10;
	else if((ch >= 'a') && (ch <= 'f'))
		return ch-'a'+10;
	else return ch-ch;//不在0-f范围内的会发送成0
}


/*
将字符串转换为hex格式
*/
void StringToHex(QString str, QByteArray &senddata)
{
	int hexdata,lowhexdata;
	int hexdatalen = 0;
	int len = str.length();
	senddata.resize(len/2);
	char lstr,hstr;

	for(int i=0; i<len; )
	{
		//char lstr,
		hstr=str[i].toLatin1();
		if(hstr == ' ')
		{
			i++;
			continue;
		}
		i++;
		if(i >= len)
			break;
		lstr = str[i].toLatin1();
		hexdata = ConvertHexChar(hstr);
		lowhexdata = ConvertHexChar(lstr);
		if((hexdata == 16) || (lowhexdata == 16))
			break;
		else
			hexdata = hexdata*16+lowhexdata;
		i++;
		senddata[hexdatalen] = (char)hexdata;
		hexdatalen++;
	}
	senddata.resize(hexdatalen);
}





