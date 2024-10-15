#include "imageprocess.h"
#include <QDir>
#include <QDateTime>
#include <QThread>
#include "simpleQtLogger.h"


ImageProcess::ImageProcess(Common::CameraHalconData data, TcpSocket* socket, QHalconWindow* hwin, Common::ShareData* sharedata, QObject *parent)
	: QObject(parent), socket(socket), hwin(hwin), sharedata(sharedata), data(data)
{
	
}

ImageProcess::~ImageProcess()
{}

void ImageProcess::updateData(Common::CameraHalconData data)
{
	this->data = data;
	initData();
}

void ImageProcess::initData()
{
	info = QString(u8"pic%1:").arg(picID);
	picdata = data.MyPicDatas[picID - 1];

	showHalconString = picdata.outputname.indexOf(picdata.vectorShowString);
	showHalconValue = picdata.outputname.indexOf(picdata.vectorShowValue);
	OKNG_flag = qMax(0,picdata.OKNG_flag-1);
	auto okQString = picdata.ok_value_list.replace(u8"��", ",");
	auto okList = picdata.ok_value_list.split(",");
	ok_value_list.clear();
	for (auto& str : okList) {
		ok_value_list << str.simplified().toInt();
	}
	for (int i = 0; i < picdata.stringResultNum; i++)
	{
		stringValueIndex.append(picdata.outputname.indexOf(picdata.stringResultNames.at(i)));
	}
	output.clear();
	for (int i = 0; i < picdata.result_num; i++)
	{
		HTuple tem = 0;
		output.append(tem);
	}
	imageResult.clear();
	for (int i = 0; i < picdata.imageResultNum; i++)
	{
		HObject tem;
		imageResult.append(tem);
	}
}

bool ImageProcess::init()
{
	initData();
	return HdevEngineinit();
}

bool ImageProcess::run(HObject& image)
{
	bool ok = false;
	GenEmptyObj(&region);
	for (int i = 0; i < picdata.result_num; i++)
	{
		this->output[i] = 0;
	}
	this->output[OKNG_flag] = 2;
	const QString ccdmes = QString("CCD%1-").arg(data.camID);
	try
	{
		if (picdata.receiveDataNum>0)
		{
			LS_INFO(ccdmes +QString::fromUtf8(info.toLocal8Bit() + u8"��������"));
			receiveValuePrecess();
		}
		m_procedureCall->SetInputIconicParamObject(picdata.imageInputName.toStdString().c_str(), image);
		for (int i = 0; i < picdata.input_num; i++)
		{
			m_procedureCall->SetInputCtrlParamTuple(picdata.inputname[i].toStdString().c_str(), 
				picdata.inputData.singleData.value(picdata.inputname[i]).toDouble());
		}
		m_procedureCall->Execute();
		for (int i = 0; i < picdata.imageResultNum; i++)
		{
			imageResult[i] = m_procedureCall->GetOutputIconicParamObject(picdata.imageResultName[i].toStdString().c_str());
			if (i == 0)
			{
				region = imageResult[i];
			}
		}

		for (int i = 0; i < picdata.result_num; i++)
		{
			this->output[i] = m_procedureCall->GetOutputCtrlParamTuple(picdata.outputname[i].toStdString().c_str());
		}

		//toStdoutput(output);
		ok = true;

		//signal2camera(u8"��ʾͼƬ", &resultImg);
	}
	catch (HException& he)
	{
		HTuple  Exception;
		he.ToHTuple(&Exception);
		QString error = QString(u8"Halcon �㷨�쳣:") + QString(Exception.ToString().Text());
		LS_INFO(ccdmes + error);
		emit emitMessageToCamera(info + error);
	}
	catch (HDevEngineException& m_halconEngineException)
	{
		QString error = m_halconEngineException.Message();
		error = QString(u8"Halcon �����쳣:") + error;
		LS_INFO(ccdmes + error);
		emit emitMessageToCamera(info + error);
	}
	catch (QException& e)
	{
		emit emitMessageToCamera(info + "QT" + e.what());
	}
	catch (const std::exception& e)
	{
		emitMessageToCamera(info + "std" + e.what());
	}


	try
	{
		//Common::m_lock.lockForWrite();
		LS_INFO(ccdmes + QString::fromUtf8(info.toLocal8Bit() + u8"����������ͼ��"));
		showHalconWindow(image);
		//Common::m_lock.unlock();
		auto cur_time = QDateTime::currentDateTime()/*.toString("yyyy-MM-dd-hh-mm-ss.zzz")*/;
		if (picdata.originalImage)
		{
			//LS_INFO(ccdmes + QString::fromUtf8(info.toLocal8Bit() + u8"��ʼ����ԭͼ"));
			saveOriginalImage(cur_time, image);
		}

		//LS_INFO(ccdmes + QString::fromUtf8(info.toLocal8Bit() + u8"��ʼдֵ��PLC"));
		writeValue2PLC();
		
		if (picdata.isEmit && ok_value_list.contains(this->output[OKNG_flag].I()))
		{
			LS_INFO(ccdmes + QString::fromUtf8(info.toLocal8Bit() + u8"��ʼ���ɷ����������"));
			genEmitData();
		}
		
		if (picdata.resultImage)
		{
			//LS_INFO(ccdmes + QString::fromUtf8(info.toLocal8Bit() + u8"��ʼ���ɽ��ͼ"));
			genResultQImage(image);
			LS_INFO(ccdmes + QString::fromUtf8(info.toLocal8Bit() + u8"��ʼ������ͼ"));
			saveResultImage(cur_time);
		}
		
		if (picdata.statics)
		{
			//LS_INFO(ccdmes + QString::fromUtf8(info.toLocal8Bit() + u8"��ʼ����ͳ��"));
			okng = ok_value_list.contains(this->output[OKNG_flag].I()) ? true : false;
			emit emitToUpdataStatistics(picID, okng, picdata.finnalPic);
		}
		LS_INFO(ccdmes + QString::fromUtf8(info.toLocal8Bit() + u8"�������"));
	}
	catch (HException& he)
	{
		HTuple  Exception;
		he.ToHTuple(&Exception);
		QString error = QString(u8"Halcon �����쳣:") + QString(Exception.ToString().Text());
		LS_INFO(ccdmes + error);
		emit emitMessageToCamera(info + error);
	}
	catch (HDevEngineException& m_halconEngineException)
	{
		QString error = m_halconEngineException.Message();
		error = QString(u8"Halcon ���������쳣:") + error;
		LS_INFO(ccdmes + error);
		emit emitMessageToCamera(info + error);
	}
	catch (QException& e)
	{
		emit emitMessageToCamera(info + "QT" + e.what());
	}
	catch (const std::exception& e)
	{
		emitMessageToCamera(info + "std" + e.what());
	}

	return ok;
}

bool ImageProcess::HdevEngineinit()
{
	bool ok = false;
	//�����������1
	m_procedure.reset();
	m_procedureCall.reset();
	try
	{
		m_procedure.reset(new HDevProcedure);
		
		auto ba = picdata.algorithmName.toLatin1();//��QstringתΪchar*
		auto ch = ba.data();//��QstringתΪchar*
		m_procedure->LoadProcedure(ch);
		m_procedureCall.reset(new HDevProcedureCall(*m_procedure.get()));
		ok = true;
		LS_INFO(QString("CCD%1-pic%2").arg(data.camID).arg(picID) + QString::fromUtf8(u8"�㷨��ʼ���ɹ�"));
		emit emitMessageToCamera(info + QString(u8"�㷨��ʼ���ɹ�"));
	}
	catch (HDevEngineException& m_halconEngineException)
	{
		QString error = m_halconEngineException.Message();
		error = QString(u8"�㷨���ش���") + error;
		LS_INFO(QString("CCD%1-pic%2").arg(data.camID).arg(picID) + error);
		emit emitMessageToCamera(info + error);
	}
	catch (HException& he)
	{
		HTuple  Exception;
		he.ToHTuple(&Exception);
		QString error = QString(Exception.ToString().Text());
		error = QString(u8"�㷨���ش���") + error;
		LS_INFO(QString("CCD%1-pic%2").arg(data.camID).arg(picID) + error);
		emit emitMessageToCamera(info + error);
	}
	return ok;
}

void ImageProcess::saveOriginalImage(const QDateTime dtime, HObject& image)
{
	//QString AppPathName = QCoreApplication::applicationDirPath();
	QString AppPathName = data.originalImageRootPath;
	auto d_time = dtime.toString("yyyy-MM-dd");
	AppPathName += "/" + d_time;
	QString  ImageNamePathall, ImageNamePathok, ImageNamePathng;
	QDir dir;
	QString PathName = QString("cam%1/pic%2/").arg(data.camID).arg(picID);
	if (picdata.saveOriginalAll)
	{
		ImageNamePathall = AppPathName + "/" + PathName + "/" + "Saveimageall" + "/";
		if (!dir.exists(ImageNamePathall))
		{
			dir.mkpath(ImageNamePathall);
		}
	}
	if (picdata.saveOriginalOk)
	{
		ImageNamePathok = AppPathName + "/" + PathName + "/" + "Saveimageok" + "/";
		if (!dir.exists(ImageNamePathok))
		{
			dir.mkpath(ImageNamePathok);
		}
	}
	if (picdata.saveOriginalNg)
	{
		ImageNamePathng = AppPathName + "/" + PathName + "/" + "Saveimageng" + "/";
		if (!dir.exists(ImageNamePathng))
		{
			dir.mkpath(ImageNamePathng);
		}
	}
	//������ʱ�������
	auto time = dtime.toString("yyyy-MM-dd-hh-mm-ss.zzz");
	QByteArray name = time.toLocal8Bit();
	char* t = name.data();
	HTuple imageformat;
	HTuple suffix;
	if (picdata.originalImageBmp)
	{
		imageformat = "bmp";
		suffix = ".bmp";
	}
	else
	{
		imageformat = "jpeg 100";
		suffix = ".jpg";
	}
	int imageresult = this->output[OKNG_flag].I();
	if (picdata.saveOriginalAll)
	{
		WriteImage(image, imageformat, 0, HTuple(ImageNamePathall.toStdString().c_str()) + HTuple(t) + suffix);
	}
	if (picdata.saveOriginalOk && ok_value_list.contains(imageresult))
	{
		WriteImage(image, imageformat, 0, HTuple(ImageNamePathok.toStdString().c_str()) + HTuple(t) + suffix);
	}
	if (picdata.saveOriginalNg && !ok_value_list.contains(imageresult))
	{
		WriteImage(image, imageformat, 0, HTuple(ImageNamePathng.toStdString().c_str()) + HTuple(t) + suffix);
	}
}

void ImageProcess::saveResultImage(const QDateTime dtime)
{
	const QString time = dtime.toString("yyyy-MM-dd-hh-mm-ss.zzz");
	auto ccdmes = QString("CCD%1-pic%2: ").arg(data.camID).arg(picID);

	QImage scaleImg;
	if (picdata.resultImageScale != 1)
	{
		//LS_INFO(ccdmes + QString::fromUtf8(u8"����"));
		scaleImg = resultImg.scaled(resultImg.width() * picdata.resultImageScale, resultImg.height() * picdata.resultImageScale, Qt::KeepAspectRatio);
	}
	else
	{
		//LS_INFO(ccdmes + QString::fromUtf8(u8"������"));
		scaleImg = resultImg.copy(resultImg.rect());
	}
	//QString AppPathName = QCoreApplication::applicationDirPath();
	QString AppPathName = data.resultImageRootPath;
	QString fullPath = AppPathName + QString("/cam%1/pic%2/").arg(data.camID).arg(picID);
	QDir dir(fullPath);
	if (!dir.exists())
	{
		dir.mkpath(fullPath);
	}
	//LS_INFO(ccdmes + QString::fromUtf8(u8"����OKNG"));
	QString state = ok_value_list.contains(this->output[OKNG_flag].I()) ? "OK" : "NG";
	if (!picdata.isMes)
	{
		//LS_INFO(ccdmes + QString::fromUtf8(u8"����·��"));
		QString name = QString("%1_%2.jpg").arg(time, state);
		QString path = fullPath + name;
		//LS_INFO(ccdmes + QString::fromUtf8(u8"������ͼ"));
		scaleImg.save(path);
		//LS_INFO(ccdmes + QString::fromUtf8(u8"������ͼ���"));
		return;
	}
	auto currenttime = time;
	currenttime = currenttime.remove("-").remove(".");
	QString code = "";
		
	for (int var_code = 0; var_code < picdata.QR_num; ++var_code) {
		QByteArray command2, receive2, commandLog2;
		command2 = SanLing_PLC::MC_Bin3E_read_message("D", picdata.QR_plc_address[var_code], picdata.QR_len[var_code], &commandLog2);
		receive2 = socket->Socket_Write_Read_Data1(command2);
		//if (receive2.length() < 11)
		//{
		//	emit emitMessageToCamera(info + QString(u8"��%1���ݶ�ȡPLCʧ��").arg(var_code + 1));
		//	continue;
		//}
		auto mmm = receive2.mid(11);
		LS_INFO(ccdmes +QString::fromUtf8(u8"��ʼ��ȡ��%1����ά��").arg(var_code+1));
		bool mes_code = true;
		for (int ii = 0; ii < mmm.size(); ++ii) {
			if (int(mmm.at(ii)) == 0)
			{
				mes_code = false;
				break;
			}
		}
		if (!mes_code)
		{
			QString codeError = "";
			for (int ii = 0; ii < mmm.size(); ++ii) {
				auto ss = QChar(mmm.at(ii));
				codeError.append(ss);
			}
			emit emitMessageToCamera(ccdmes + QString(u8"PLC��%1����ά������MESͼƬ���ƽ�ȱ�ٵ�%2����ά�룺%3").arg(var_code + 1).arg(var_code + 1).arg(codeError));
			LS_INFO(ccdmes + QString::fromUtf8(u8"PLC��%1����ά������MESͼƬ���ƽ�ȱ�ٵ�%2����ά�룺%3").arg(var_code + 1).arg(var_code + 1).arg(codeError));
			continue;
		}
		for (int ii = 0; ii < mmm.size(); ++ii) {
			auto ss = QChar(mmm.at(ii));
			code.append(ss);
		}
		//if (var_code < QR_num - 1)
		code.append("_");

	}
	QString name = QString("%1_%2%3.jpg").arg(currenttime, code, state);
	QString path = fullPath + name;

	for (int var = 0; var < picdata.mes_num; ++var) {
		QByteArray message;
		message.append(ok_value_list.contains(this->output[OKNG_flag].I()) ? "0," : "1,");
		message.append(QString("%1,").arg(picID).toLocal8Bit());//ͼ��
		if (this->output[showHalconValue].TupleLength()<=var)
		{
			this->output[showHalconValue][var] = 0;
		}
		message.append(QString::number(this->output[showHalconValue][var].D(), 'f', 6).toLocal8Bit() + ",");
		message.append(name.toLocal8Bit());
		int s[400];
		int length = message.length();
		for (int var1 = 0; var1 < length; ++var1) {
			s[var1] = message.at(var1);
		}
		QByteArray command1, receive1, commandLog1;
		command1 = SanLing_PLC::MC_Bin3E_write_message_bit32("D", picdata.picPath2PLCaddress[var], length, s, &commandLog1);
		receive1 = socket->Socket_Write_Read_Data(command1);

		if (receive1 == "")
		{
			//ͨ��ʧ��
			emit emitMessageToCamera(info + u8"�����ϴ�mesд��PLCʧ��");
		}
		auto logstr = QString("CCD%1-pic%2: ").arg(data.camID).arg(picID) + QString::fromUtf8(u8"д���%1��mes���ݣ�%2  ��ַ��%3")
			.arg(var + 1).arg(QString::fromLocal8Bit(message)).arg(picdata.picPath2PLCaddress[var]);
		LS_INFO(logstr);
	}
	if (!scaleImg.save(path))
	{			
		auto logstr = info + QString::fromUtf8(u8"mesͼƬ����ʧ�ܣ�·�����Ϸ�:")+path;
		emit emitMessageToCamera(logstr);
		LS_INFO(logstr);
		return;
	}
}

void ImageProcess::writeValue2PLC()
{
	const QString ccdmes = QString("CCD%1-").arg(data.camID);
	//plc���ͽ��
	if (picdata.valueResultNum==0)
	{
		return;
	}
	for (int i = 0; i < picdata.valueResultNum; i++)
	{
		pic_result[i] = this->output[i].D();
	}
	QByteArray command, receive, commandLog;
	command = SanLing_PLC::MC_Bin3E_write_message_bit32("D", picdata.picAddress, picdata.valueResultNum, pic_result, &commandLog);
	receive = socket->Socket_Write_Read_Data(command);
	if (receive == "")
	{
		//ͨ��ʧ��
		emit emitMessageToCamera(info + u8"����PLC����ʧ��");
		//�ڽ���д������
	}
	else {
		QString valueInfo;
		for (int i = 0; i < picdata.valueResultNum; i++)
		{
			valueInfo += QString("%1,").arg(pic_result[i]);
		}
		LS_INFO(ccdmes+info + u8"д����ֵ��PLC��" + valueInfo + QString(u8" �׵�ַ��%1").arg(picdata.picAddress));
	}
	for (int i = 0; i < picdata.stringResultNum; i++)
	{
		int s[400];
		QString message_QString = this->output[stringValueIndex[i]].ToString().Text();
		QByteArray message = message_QString.remove("\"").toLocal8Bit();
		int length = message.length();
		for (int var1 = 0; var1 < length; ++var1) {
			s[var1] = message.at(var1);
		}
		QByteArray command1, receive1, commandLog1;
		command1 = SanLing_PLC::write_D_register_bit16(picdata.stringResult2PLCAddress[i], length, s);
		//command1 = SanLing_PLC::MC_Bin3E_write_message_bit32("D", picdata.stringResult2PLCAddress[i], length, s, &commandLog1);
		receive1 = socket->Socket_Write_Read_Data(command1);
		LS_INFO(ccdmes+info + QString(u8"д���ַ���%1��PLC��").arg(i) + message_QString + QString(u8", ��ַ��%1").arg(picdata.stringResult2PLCAddress[i]));
		if (receive1 == "")
		{
			//ͨ��ʧ��
			emit emitMessageToCamera(info + QString(u8"�ַ�������%1д��PLCʧ��").arg(i));
		}
	}
	//*******************֪ͨplc���ճɹ�*****************
	{
		QByteArray command, receive, commandLog;
		int data1[1] = { 1 };
		command = SanLing_PLC::MC_Bin3E_write_message("D", this->data.CamplcGrabok, 1, data1, &commandLog);
		receive = socket->Socket_Write_Read_Data(command);
		if (receive == "")
		{
			LS_INFO(ccdmes+info + QString::fromUtf8(info.toLocal8Bit() + u8"֪ͨPLC���ճɹ�����ʧ��"));
			emit emitMessageToCamera(info + u8"֪ͨPLC���ճɹ�����ʧ��");
		}
	}
}

bool ImageProcess::HImage2QImage(HalconCpp::HImage& from, QImage& to)
{
	Hlong width;
	Hlong height;
	from.GetImageSize(&width, &height);

	HTuple channels = from.CountChannels();
	HTuple type = from.GetImageType();

	if (strcmp(type[0].S(), "byte")) // ������� byte ���ͣ���ʧ��
	{
		return false;
	}

	QImage::Format format;
	switch (channels[0].I())
	{
	case 1:
		format = QImage::Format_Grayscale8;
		break;
	case 3:
		format = QImage::Format_RGB32;
		break;
	default:
		return false;
	}

	if (to.width() != width || to.height() != height || to.format() != format)
	{
		to = QImage(static_cast<int>(width),
			static_cast<int>(height),
			format);
	}
	HString Type;
	if (channels[0].I() == 1)
	{
		unsigned char* pSrc = reinterpret_cast<unsigned char*>(from.GetImagePointer1(&Type, &width, &height));
		memcpy(to.bits(), pSrc, static_cast<int>(width) * static_cast<int>(height));
		return true;
	}
	else if (channels[0].I() == 3)
	{
		uchar* R, * G, * B;
		from.GetImagePointer3(reinterpret_cast<void**>(&R),
			reinterpret_cast<void**>(&G),
			reinterpret_cast<void**>(&B), &Type, &width, &height);

		for (int row = 0; row < height; row++)
		{
			QRgb* line = reinterpret_cast<QRgb*>(to.scanLine(row));
			for (int col = 0; col < width; col++)
			{
				line[col] = qRgb(*R++, *G++, *B++);
			}
		}
		return true;
	}

	return false;
}

void ImageProcess::genResultQImage(HObject& image)
{
	QPen pen = QPen(Qt::green, picdata.lineWidth);
	//QPainter pp(&resultImg);
	//pp.setPen(pen);

	try
	{
		HImage  ho_MultiChannelImage1;
		Compose3(image, image, image, &ho_MultiChannelImage1);
		HImage2QImage(ho_MultiChannelImage1, resultImg);
		QPainter pp(&resultImg);
		pp.setPen(pen);
		if (region.IsInitialized()) {
			const int size = region.CountObj();

			for (int var = 0; var < size; ++var) {

				HObject hh = region.SelectObj(var + 1);
				if (!m_draw.drawHObject(resultImg, hh, pp)) {
					HTuple className = hh.GetObjClass();
					emit emitMessageToCamera(info + QString(u8"ͼ����Ʋ�֧�ֵ����ͣ�%1").arg(className.ToString().Text()));
				}
			}
		}
	}
	catch (HException& he)
	{
		HTuple  Exception;
		he.ToHTuple(&Exception);
		QString error = QString(Exception.ToString().Text());
		error = QString(u8"ͼ����ƴ���region��") + error;
		emit emitMessageToCamera(info + error);
	}
	catch (...)
	{
		emit emitMessageToCamera(info + u8"δ֪����1");
	}
	QFont font;//�������壬����������������
	font.setFamily("Microsoft YaHei");//������ʽ
	font.setPointSize(picdata.frontSize);//������С
//    font.setItalic(true);//�����Ƿ�б��
	QPainter pp(&resultImg);
	pp.setPen(pen);
	pp.setFont(font);
	QBrush brush = QBrush(Qt::green);
	pp.setBrush(brush);
	const int length = this->output[showHalconString].TupleLength();
	QVector<QString> r7;
	QVector<QString> r8;
	for (int i = 0; i < length; i++) {
		auto str = QString::fromLocal8Bit(this->output[showHalconString][i].ToTuple().ToString().Text());
		r7.append(str.remove("\""));

		QString value = this->output[showHalconValue][i].ToTuple().ToString().Text();
		r8.append(value);
	}

	const int row = 200;
	const int bias = 200;
	for (int i = 0; i < length; i++) {
		auto str = r7[i] + r8[i];
		pp.drawText(QPointF(100, row + bias * i), str);
	}
	// ��ʾ����
	if (picdata.showInput)
	{
		HTuple w, h;
		GetImageSize(image, &w, &h);
		int xpos = w.D() * 0.8;
		for (int i = 0; i < picdata.input_num; i++)
		{
			auto str = picdata.inputname[i] + QString(": %1").arg(picdata.inputData.singleData.value(picdata.inputname[i]).toDouble());
			pp.drawText(QPointF(xpos, row + bias * i), str);
		}
	}

}

void ImageProcess::showHalconWindow(HObject& image)
{
	hwin->GetHalconBuffer()->ClearWindow();//���֮ǰ�Ĵ�������
	hwin->GetHalconBuffer()->DispObj(image);
	hwin->GetHalconBuffer()->SetTposition(100, 100);
	hwin->GetHalconBuffer()->DispObj(region);
	if (ok_value_list.contains(this->output[OKNG_flag].I()))
	{
		hwin->GetHalconBuffer()->SetColor("green");
		hwin->GetHalconBuffer()->WriteString(QString("ok%1").arg(picID).toStdString().c_str());
	}
	else
	{
		hwin->GetHalconBuffer()->SetColor("red");
		hwin->GetHalconBuffer()->WriteString(QString("ng%1").arg(picID).toStdString().c_str());
	}

	const int length = this->output[showHalconString].TupleLength();
	if (length!= this->output[showHalconValue].TupleLength())
	{
		emitMessageToCamera(info + u8"��ʾ��ǩ������������ƥ��");
		hwin->GetHalconBuffer()->SetTposition(iniHeight + bias, 100);
		hwin->GetHalconBuffer()->WriteString(u8"��ʾ��ǩ������������ƥ��");
		hwin->GetHalconBuffer()->FlushBuffer();
		return;
	}
	for (int i = 0; i < length; i++) {
		hwin->GetHalconBuffer()->SetTposition(iniHeight + bias * i, 100);
		hwin->GetHalconBuffer()->WriteString(HTuple(this->output[showHalconString][i] + this->output[showHalconValue][i]));
	}
	Common::m_lock.lockForWrite();
	hwin->GetHalconBuffer()->FlushBuffer();
	Common::m_lock.unlock();
}

void ImageProcess::genEmitData()
{
	//emit emitMessageToCamera(info + QString(u8"%1").arg(QString::number((unsigned int)sharedata, 16)));
	Common::m_lock.lockForWrite();
	QString emitID = QString("%1-%2").arg(data.camID).arg(picID);
	//sharedata->emitSingleData.remove(emitID);
	QHash<QString, QVariant> tem_data;
	for (int i = 0; i < picdata.emitResultNum; i++)
	{
		const int index = picdata.outputname.indexOf(picdata.emitParamNames.at(i));
		if (index != -1)
		{
			tem_data.insert(picdata.receiveParamNames.at(i), this->output[index].D());
		}
	}
	sharedata->emitSingleData.insert(emitID, tem_data);
	Common::m_lock.unlock();
}

void ImageProcess::receiveValuePrecess()
{
	//emit emitMessageToCamera(info + QString(u8"%1").arg(QString::number((unsigned int)sharedata, 16)));
	bool isReceive = false;
	int maxTime = 1500;
	int t = 100;
	while (maxTime > 0)
	{
		for (auto& receiveCamPioc : picdata.receiveDataCamPic)
		{
			if (sharedata->emitSingleData.keys().contains(receiveCamPioc))
			{
				isReceive = true;
				//Common::m_lock.lockForRead();
				QHash<QString, QVariant> receive_data = sharedata->emitSingleData.value(receiveCamPioc);
				QHash<QString, QVariant>::const_iterator it = receive_data.begin();
				while (it != receive_data.end()) {
					picdata.inputData.singleData.insert(it.key(), it.value());
					++it;
				}
				//Common::m_lock.unlock();
				Common::m_lock.lockForWrite();
				sharedata->emitSingleData.remove(receiveCamPioc);
				Common::m_lock.unlock();
				break;
			}
		}
		if (isReceive)
		{
			break;
		}
		QThread::msleep(t);
		maxTime -= t;
	}

	if (!isReceive)
	{
		LS_INFO(info + QString::fromUtf8(u8"�������ݲ�����"));
		for (int i = 0; i < picdata.input_num; i++)
		{
			picdata.inputData.singleData.insert(picdata.inputname[i], 0);		// �������Ĭ��ֵ
		}
		for (auto& receiveCamPioc : picdata.receiveDataCamPic)
		{
			QString mes1 = info + QString(u8"���������в�����%1������,��鷢�����ݵ�����Ƿ����ա�����NG�������ļ�").arg(receiveCamPioc);
			emit emitMessageToCamera(mes1);
		}
	}
}