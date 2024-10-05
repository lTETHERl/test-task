#include <iostream>
#include <locale>
#include <vector>

typedef unsigned char BYTE;

BYTE buffer_data[184] = {
0xab, 0xcd, 0x00, 0xb3, 0x4a, 0x42, 0x00, 0x19, 0x01, 0x00,
0x01, 0x02, 0x03, 0x14, 0x0f, 0x01, 0x04, 0x1c, 0x08, 0x04,
0x00, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x1d, 0x4d,
0x42, 0x00, 0x75, 0xe2, 0x02, 0x61, 0xff, 0xc2, 0xff, 0x04,
0x00, 0x03, 0x04, 0x3e, 0x00, 0x2a, 0xff, 0xfb, 0x46, 0xc3,
0x00, 0xff, 0xff, 0xff, 0xc2, 0x00, 0x00, 0x02, 0x61, 0x00,
0x2d, 0xe3, 0x03, 0x85, 0xff, 0xa5, 0xfe, 0xd2, 0x00, 0x02,
0x05, 0x03, 0x00, 0x2a, 0xff, 0xf6, 0x47, 0x1a, 0x00, 0xff,
0xff, 0xff, 0xa5, 0x00, 0x00, 0x03, 0x85, 0x00, 0x2d, 0xe4,
0x05, 0xee, 0xff, 0xe0, 0xfe, 0xd3, 0x00, 0x04, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x46, 0x62, 0x00, 0xff, 0xff, 0xff,
0xe0, 0x00, 0x00, 0x05, 0xee, 0x00, 0x2d, 0xe1, 0x06, 0x6d,
0x00, 0x82, 0x00, 0xf7, 0x00, 0x08, 0x11, 0x63, 0x00, 0xb5,
0x00, 0x0b, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x82, 0x00,
0x00, 0x06, 0x6d, 0x00, 0x2f, 0xea, 0x44, 0x4c, 0x00, 0x25,
0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00,
0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x08, 0x00,
0x00, 0x00, 0xd9, 0xc0 };

int countBits(BYTE byte)
{
	int cnt = 0;
	for (size_t i = 0; i < 8; ++i)
	{
		if (((byte >> i) & 0b00000001) != 0) ++cnt;
	}

	return cnt;
}

std::vector<std::vector<BYTE>> splitDataToThreeArrays(BYTE buff[], size_t size)
{
	std::vector<std::vector<BYTE>> res;
	std::vector<BYTE> temp;

	bool firstBlock = false, secondBlock = false;

	for (size_t i = 0; i < size; ++i)
	{
		if (i + 1 < 184 && buff[i] == 0x4A && buff[i+1] == 0x42 && !firstBlock)
		{
			firstBlock = true;
			res.push_back(temp);
			temp.clear();
		}

		if (i + 1 < 184 && buff[i] == 0x4D && buff[i+1] == 0x42 && !secondBlock)
		{
			secondBlock = true;
			res.push_back(temp);
			temp.clear();
		}
		temp.push_back(buff[i]);
	}

	res.push_back(temp);
	return res;
	
}

void dissasembleFirstBlock(const std::vector<BYTE>& buff)
{
	std::cout << "Заголовок фрейма: " << std::hex << ((static_cast<int>(buff[0]) << 8) | buff[1]) << std::endl
		<< "Общий объём данных: " << std::dec << ((static_cast<int>(buff[2]) << 8) | buff[3]) << " байтов" << std::endl;
}

void dissasembleSecondBlock(const std::vector<BYTE>& buff)
{
	size_t i = 0;
	std::cout << "Тип информационного модуля: " << std::hex << ((static_cast<int>(buff[i++]) << 8) | buff[i++]) << std::endl;
	std::cout << "Длина данных модуля: " << std::dec << ((static_cast<int>(buff[i++]) << 8) | buff[i++]) << " байтов" << std::endl;
	std::cout << "CRC: " << (static_cast<int>(buff[buff.size() - 1])) << std::endl;
}

void dissasembleThirdBlock(const std::vector<BYTE>& buff)
{
	int i = 0;
	std::cout << "Тип информационного модуля: " << std::hex << ((static_cast<int>(buff[i++]) << 8) | buff[i++]) << std::endl;
	int length = ((static_cast<int>(buff[i++]) << 8) | buff[i++]);
	std::cout << "Длина данных модуля: " << std::dec << length << " байтов" << std::endl;

	int numOfTargets = (length - 1) / 29; //29 - колличество байт в информации об объекте

	int countTargets = numOfTargets;


	while (countTargets > 0)
	{
		std::cout << std::endl;

		std::cout << "Номер " << (numOfTargets - countTargets + 1) << " цели: " << std::dec << (static_cast<unsigned int>(buff[i++])) << std::endl;
		std::cout << "Продольное расстояние: " << (0.1 * ((static_cast<unsigned int>(buff[i++]) << 8) | buff[i++])) << " метров" << std::endl;
		std::cout << "Горизонтальное расстояние: " << (0.1 * ((static_cast<unsigned int>(buff[i++]) << 8) | buff[i++])) << " метров" << std::endl;
		std::cout << "Скорость (Y направление): " << (0.1 * ((static_cast<unsigned int>(buff[i++]) << 8) | buff[i++])) << " метров" << std::endl;

		unsigned int type = static_cast<unsigned int>(buff[i++]);
		std::string typeStr = "";
		switch (type) {
		case 0: typeStr = "Маленький автомобиль"; break;
		case 1: typeStr = "Пешеход"; break;
		case 2: typeStr = "Не моторное транспортное средство"; break;
		case 3: typeStr = "Транспортное средство среднего размера"; break;
		case 4: typeStr = "Большой автомобиль"; break;
		default: typeStr = "Неизвестный тип"; break;
		}
		std::cout << "Тип цели: " << typeStr << std::endl;

		std::cout << "Номер полосы движения: " << (static_cast<unsigned int>(buff[i++])) << std::endl;
		std::cout << "Переднее расстояние: " << (0.1 * ((static_cast<unsigned int>(buff[i++]) << 8) | buff[i++])) << " метров" << std::endl;
		std::cout << "Передний временной интервал: " << (0.1 * ((static_cast<unsigned int>(buff[i++]) << 8) | buff[i++])) << " секунд" << std::endl;
		std::cout << "Скорость (X направление): " << (0.1 * ((static_cast<unsigned int>(buff[i++]) << 8) | buff[i++])) << " метров" << std::endl;
		std::cout << "Направляющий угол: " << (0.01 * ((static_cast<unsigned int>(buff[i++]) << 8) | buff[i++])) << " градусов" << std::endl;

		BYTE events = buff[i++];
		std::string eventsStr = "";
		std::vector<std::string> eventsArr = {
			"Разблокированная парковка", "Перегруженная парковка", "Превышение скорости транспортным средством", "Парковка в гавани",
			"Медленно движущееся транспортное средство", "Пешеход", "Ретроградное транспортное средство", "Смена полосы движения"
		};
		for (size_t i = 0; i < 8; i++) {
			if (((events >> i) & 0b00000001) != 0) {
				eventsStr += eventsArr[i] + " ";
			}
		}
		std::cout << "Произошло событие: " << (eventsStr.empty() ? "никаких событий не произошло" : eventsStr) << std::endl;

		std::cout << "Координата X радиолокационной сети: " << (0.1 * ((static_cast<int>(buff[i++]) << 24) | (static_cast<int>(buff[i++]) << 16) |
			(static_cast<int>(buff[i++]) << 8) | buff[i++])) << " метров" << std::endl;
		std::cout << "Координата Y радиолокационной сети: " << (0.1 * ((static_cast<int>(buff[i++]) << 24) | (static_cast<int>(buff[i++]) << 16) |
			(static_cast<int>(buff[i++]) << 8) | buff[i++])) << " метров" << std::endl;

		std::cout << "Заполните слепую отметку: " << ((static_cast<unsigned int>(buff[i]) == 0) ? "Отчёт главного радара" : "Заполните отчёт о слепом радаре")
			<< ' ' << static_cast<unsigned int>(buff[i++]) << std::endl;

		std::cout << "Длина автомобиля: " << (0.1 * static_cast<unsigned int>(buff[i++])) << " метров" << std::endl;
		std::cout << "Ширина автомобиля: " << (0.1 * static_cast<unsigned int>(buff[i++])) << " метров" << std::endl;


		--countTargets;
	}
	std::cout << "CRC: " << static_cast<int>(buff[i]) << std::endl;
	return;
}

int main()
{
	std::setlocale(LC_ALL, "rus");
	std::vector<std::vector<BYTE>> res = splitDataToThreeArrays(buffer_data, sizeof(buffer_data)/sizeof(BYTE));

	std::cout << "Начало первого блока: " << "0x" << std::hex << ((static_cast<int>(res[0][0])<<8)|res[0][1]) << '\t' << "Размер первого блока: " << std::dec << res[0].size() << std::endl
		<< "Начало второго блока: " << "0x" << std::hex << ((static_cast<int>(res[1][0]) << 8) | res[1][1]) << '\t' << "Размер второго блока: " << std::dec << res[1].size() << std::endl
		<< "Начало третьего блока: " << "0x" << std::hex << ((static_cast<int>(res[2][0]) << 8) | res[2][1]) << '\t' << "Размер третьего блока: " << std::dec << res[2].size() << std::endl << std::endl;

	dissasembleFirstBlock(res[0]);
	std::cout << std::endl;
	dissasembleSecondBlock(res[1]);
	std::cout << std::endl;
	dissasembleThirdBlock(res[2]);

	system("pause");

	return 0;
}