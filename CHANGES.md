# CHANGES.md

Хронология изменений по датам. Подробности по сборке и диагностике — в
[`docs/BUILD_NOTES.md`](docs/BUILD_NOTES.md).

---

## 2026-10-09 — Отключены диагностические логи сетевого пакета `[NETLOG]`

Три лога, добавленных при разборе ошибки подключения (клиент получал `ERR_PT_NETEXCP`,
код 522), выключены по причине размера: `receiver_debug.log` разрастался примерно до 3 МБ
за одну сессию клиента, остальные — до сотен килобайт. Файлы открывались в рабочем
каталоге процесса при загрузке.

**Код не удалён.** Все вызовы закомментированы и помечены тегом `[NETLOG]`, конструкция
`std::ofstream` убрана за `#if`, который по умолчанию выключен.

| Файл | Флаг (было → стало) | Было | Стало |
|------|--------------------|------|-------|
| `sources/Libraries/Server/sdk/src/Packet.cpp` | `DEBUG_WPACKET` → `DIAG_WPACKET_LOG` | `1` | `0` |
| `sources/Libraries/Server/sdk/src/Sender.cpp` | `DEBUG_SENDER` → `DIAG_SENDER_LOG` | `1` | `0` |
| `sources/Libraries/Server/sdk/src/Receiver.cpp` | `DEBUG_RECEIVER` → `DIAG_RECEIVER_LOG` | `1` | `0` |

Закомментировано 21 строку вызовов: 8 в `Packet.cpp`, 2 в `Sender.cpp`, 11 в `Receiver.cpp`.

Дополнительно в `Packet.cpp` и `Sender.cpp` добавлен `#include <iostream>` — без него ветка
`#else` (`std::cerr`) не собирается, в исходном варианте заголовка там не было.

Возврат логов: поставить `1` в нужный `DIAG_*_LOG` и снять комментарий со строк,
помеченных `[NETLOG]`. Подробности в [`docs/BUILD_NOTES.md`](docs/BUILD_NOTES.md).

Причина проблемы, ради которой логи и добавлялись, устранена ранее — см. ниже.

---

## 2026-10-08 — ANSI → Unicode ODBC Fix for Wine + Microsoft ODBC Driver 17
Сессия от 2026-10-08, ветка `arena/4b0016bf-talesofpiratedx9` (PR #2 → base `recompile_linux`)

## Проблема
При запуске 32-битных Windows-серверов Tales of Pirates под Wine в Docker-контейнере подключение к MSSQL Server 2022 падало, потому что Microsoft ODBC Driver 17 for SQL Server экспортирует только Unicode-версии ODBC функций (с суффиксом W). Оригинальный ANSI код использовал не-W версии функций — Wine не может их эмулировать.

## Измененные файлы (все правки ODBC):
| Файл | Что изменено |
|------|--------------|
| `sources/Libraries/Util/include/db.h` | Добавлен `#include <windows.h>` для доступа к WinAPI типам. В инлайн-хелпере `exec_direct_w()` кодировка изменена с `CP_UTF8` на `CP_ACP` (корректно для ANSI Win32 приложения). Буфер `wsql[]` использует константу `SQL_MAXLEN`. |
| `sources/Libraries/Util/src/db.cpp` | - Удалено дублирующее static определение `exec_direct_w` (бралось из заголовка)<br>- Заменены все вызовы устаревшей ODBC 2.x функции `SQLSetStmtOption(...SQL_QUERY_TIMEOUT...)` на современную ODBC 3.x `SQLSetStmtAttr(...SQL_ATTR_QUERY_TIMEOUT...)` — **это была ключевая причина падения всех запросов**: Driver 17 не поддерживает SQLSetStmtOption, возвращал SQL_ERROR(-1) до отправки запроса на сервер<br>- Добавлен принудительный `printf()` ODBC ошибок в stdout (обходит игровые логгеры, помогает при диагностике под Wine, когда буфер stdout не сбрасывается)<br>- Конвертация строк MB2WC/WC2MB переведена с CP_UTF8 на CP_ACP |
| `sources/Libraries/Util/src/db3.cpp` | - Исправлены ошибки компиляции в блоках `SQLGetDiagRecW`: заменен несуществующий `*pfNativeError` на правильную локальную переменную `nNativeError`<br>- Исправлена несовместимость типов `SQLCHAR* (unsigned char*) → LPSTR (char*)` в вызовах `WideCharToMultiByte` — добавлено явное приведение `(LPSTR)`<br>- Добавлена пропущенная конвертация сообщения ошибки `wmsg` в обоих ветках `CSQLRecordset::ThrowError()` (до этого szErrorMsg оставался неинициализированным)<br>- Исправлен вызов `WideCharToMultiByte`: убран лишний 9-й аргумент, длина берется из возвращаемого значения функции<br>- Размер буферов wstate/wmsg заменен с магических констант (7/256) на ODBC-константы `SQL_SQLSTATE_SIZE +1` и `SQL_MAX_MESSAGE_LENGTH +1`<br>- Перенесено определение `static exec_direct_w_db3()` из конца файла в начало (после инклюдов) для исправления ошибки "identifier not found" — функция вызывалась раньше чем была объявлена<br>- Убран повторяющийся блок инклюдов в конце, добавлен обратно `using namespace std;`<br>- Заменены вызовы `SQLSetStmtOption` на `SQLSetStmtAttr`<br>- Кодировка MB2WC исправлена на CP_ACP |

## Текущий статус работы серверов
✅ **Все три сервера успешно подключаются к MSSQL и работают:**
- AccountServer — поднимается, слушает порт
- GroupServer — подключается к БД, создает триггеры в таблицах, подключается к AccountServer (`172.21.0.3:1978 come!`) и GateServer (`172.21.0.5 login success`)
- GateServer — слушает порт 1973 на 0.0.0.0, принимает TCP-соединения

⚠️ **Ошибка DROP TRIGGER "не существует или нет прав" при старте — это НОРМАЛЬНО**, код ожидает эту ошибку и игнорирует (при первом запуске триггеры еще не существуют), CREATE TRIGGER после этого проходит успешно.

## Не решенная проблема (не связана с ODBC!)
**Клиент Game.exe падает при получении первого пакета от GateServer после подключения:**
```
10-08 14:36:11UnhandledException
sources/Libraries/Server/sdk/src/Receiver.cpp(74) : Game.exe at dbc::Receiver::Process()
```
Строка 74 — это стандартный вызов Winsock `recv()`. Выглядит как access violation при записи в буфер пакета m_rpk. Происходит через ~40 секунд после установки TCP соединения с GateServer.

Проверено при настройках GateServer.cfg:
```ini
[ToClient]
CommEncrypt = 0
RSAAES = 0
WpeProtection = 0
DDoSProtection = 0
```
Падение повторяется даже при полностью выключенном шифровании — значит проблема **НЕ в шифровании**, а в чем-то другом в клиентском коде обработки сетевых пакетов или сервере под Wine. Сетевой код клиента и сервера никак не изменялся в рамках этого патча.

## Важные заметки для Wine/Docker
1. `setvbuf(stdout, NULL, _IOLBF, 0)` в main() сервисов решит проблему буферизации логов в Docker логах (Wine/Windows libc использует полную блочную буферизацию при выводе в pipe)
2. Ошибка `ZwLoadDriver winebth` — безвредное предупреждение Wine про Bluetooth, можно игнорировать.
3. Для подключения к БД используется connection string:
   ```
   DRIVER={ODBC Driver 17 for SQL Server};SERVER=%s;UID=%s;PWD=%s;DATABASE=%s;Encrypt=no;TrustServerCertificate=yes
   ```
   и в AccountServer она аналогичная.

## Ссылка на PR
https://github.com/zeleniyToster/TalesOfPirateDX9/pull/2
