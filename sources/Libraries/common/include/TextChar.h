#pragma once

/*	Работа с символами строки в зависимости от кодировки.

	Движок написан под китайскую сборку и почти везде считает так:
	байт со старшим битом — первая половина двухбайтового символа GBK,
	следующий байт пропускается. Для cp1251 это неверно: русская буква
	тоже байт 0xC0..0xFF, поэтому «съедалась» соседняя буква, а длина
	строки считалась вдвое больше реальной.

	Определить по одному байту, GBK это или cp1251, нельзя: диапазоны
	пересекаются. Значит решение принимается один раз для всей игры, а
	все проверки идут через эти функции.

	При ARBAH_TEXT_CP == 1256 русская раскладка: байт 0x80 и выше —
	обычный одиночный символ. Китайский при этом перестаёт быть
	двухбайтовым и рисуется по одному байту, то есть едет вёрстка под
	иероглифы. Это осознанный размен: игра русская, китайских строк
	в таблицах почти нет (в клиентской StringSet их две, обе — следы
	ошибок компилятора, см. docs).
*/

#ifndef ARBAH_TEXT_CP
#define ARBAH_TEXT_CP 1251
#endif

/*	Двухбайтовый ли символ начинается с этого байта.

	GBK: ведущий байт 0x81..0xFE, хвостовой 0x40..0xFE кроме 0x7F.
	При другой кодировке считаем любой байт одиночным.
*/
inline bool IsLeadByte(unsigned char c)
{
#if ARBAH_TEXT_CP == 936
	return c >= 0x81 && c <= 0xFE;
#else
	(void)c;
	return false;
#endif
}

/*	Хвостовой ли это байт. Нужен там, где код проверяет второй байт
	пары сам, минуя IsLeadByte. */
inline bool IsTrailByte(unsigned char c)
{
#if ARBAH_TEXT_CP == 936
	return (c >= 0x40 && c <= 0xFE && c != 0x7F);
#else
	(void)c;
	return false;
#endif
}

/*	Сколько байт занимает символ, начинающийся в позиции i. */
inline int CharBytes(const char* s, int i)
{
	unsigned char c = static_cast<unsigned char>(s[i]);
	if (!IsLeadByte(c))
		return 1;
	/*	Ведущий байт без хвостового — это одиночный символ, иначе
		следующий же начнёт считаться ошибочным хвостом и каскад
		разъедется дальше. */
	unsigned char next = static_cast<unsigned char>(s[i + 1]);
	return IsTrailByte(next) ? 2 : 1;
}

/*	Сколько символов (а не байт) в строке. */
inline int CharCount(const char* s, int len)
{
	int n = 0;
	for (int i = 0; i < len && s[i]; i += CharBytes(s, i))
		++n;
	return n;
}

/*	Позиция начала символа номер ordinal (нумерация с нуля). */
inline int CharOffset(const char* s, int len, int ordinal)
{
	int i = 0, n = 0;
	while (i < len && s[i] && n < ordinal)
	{
		i += CharBytes(s, i);
		++n;
	}
	return i < len ? i : len;
}