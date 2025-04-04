# freeBSD_lab4
## Завдання №1
Скільки пам’яті може виділити `malloc(3)` за один виклик? Параметр `malloc(3)` є цілим числом типу даних `size_t`, тому логічно максимальне число, яке можна передати як параметр `malloc(3)`, — це максимальне значення `size_t` на платформі `(sizeof(size_t))`. У 64-бітній Linux `size_t` становить 8 байтів, тобто 8 * 8 = 64 біти. Відповідно, максимальний обсяг пам’яті, який може бути виділений за один виклик `malloc(3)`, дорівнює 2^64. Спробуйте запустити код на x86_64 та x86. Чому теоретично максимальний обсяг складає 8 ексабайт, а не 16?
## Реалізація завдання
Стосовне обмеження. Навіть якщо `size_t` дозволяє передати 2^64 - 1, адресний простір процеса та сам механізм алокації `malloc` не дозволять фактично отримати стільки пам’яті. Через ці обмеження `malloc` може працювати з максимумом у 2^63 байт (тобто 8 ексабайт) замість 16.
## Компіляція та виконання завдання
![image](https://github.com/user-attachments/assets/ccb5f33d-934e-408e-a00a-880b8bc6fb9a)

Як бачимо з попереджень компілятора - `malloc((size_t)-1` намагається виділити 18446744073709551615 байтів (тобто 2^64 - 1), що перевищує реальні ліміти ОС. Компілятор генерує попередження `-Walloc-size-larger-than`, вказуючи, що значення перевищує максимально дозволений розмір. Навіть якщо `size_t` дозволяє таке значення, операційна система обмежує максимальний розмір доступної пам’яті.

## Завдання №2
Що станеться, якщо передати `malloc(3)` від’ємний аргумент? Напишіть тестовий випадок, який обчислює кількість виділених байтів за формулою `num = xa * xb`. Що буде, якщо num оголошене як цілочисельна змінна зі знаком, а результат множення призведе до переповнення? Як себе поведе `malloc(3)`? Запустіть програму на x86_64 і x86.
## Реалізація завдання
Функція `malloc(size_t size)` приймає беззнакове значення (size_t). Якщо передати від’ємне число, наприклад, -1, воно буде неявно приведене до великого позитивного значення, що призведе до помилки виділення пам’яті.
**Переповнення під час множення.** Якщо ми обчислюємо `num = xa * xb`, де `num` є знаковою змінною, а результат множення перевищує максимальне значення, станеться переповнення. Наприклад, на 32-бітній системі `int` може містити значення до 2^31 - 1, а якщо `xa * xb > 2^31 - 1`, то `num` отримає некоректне від’ємне значення.
## Компіляція та виконання завдання
![image](https://github.com/user-attachments/assets/edf0f020-b169-4161-93d4-ea4f3ebac2ce)

## Завдання №3
Що станеться, якщо використати `malloc(0)`? Напишіть тестовий випадок, у якому `malloc(3)` повертає `NULL` або вказівник, що не є `NULL`, і який можна передати у `free()`. Відкомпілюйте та запустіть через `ltrace`. Поясніть поведінку програми.
## Реалізація завдання
Деякі реалізації виклику `malloc(0)` можуть повертати NULL, вказуючи на відсутність виділеної пам’яті, а інші можуть повертати ненульовий вказівник, який потім можна передати у free(), але фактично жодна пам’ять не буде виділена. Я написала код, який виділяє пам’ять через `malloc(3)` та `malloc(0)`, потім перевіряє, що повернулося, і викликає free().
## Компіляція та виконання завдання
![image](https://github.com/user-attachments/assets/c98af6c4-ef9e-435d-8456-91d755964926)

Отже,
* `malloc(3)` повертає нормальний вказівник, наприклад, 0x165d81008008;
* `malloc(0)` повертає ненульовий вказівник, наприклад, 0x165d81008010;
* це означає, що реалізація `malloc()` у цій системі повертає "фіктивний" вказівник при запиті 0 байтів;
* обидва значення передаються у `free()`, і програма коректно завершується.

## Завдання №4
Чи є помилки у такому коді?
```
void *ptr = NULL;
while (<some-condition-is-true>) {
    if (!ptr)
        ptr = malloc(n);
    [... <використання 'ptr'> ...]
    free(ptr);
}
```
Напишіть тестовий випадок, який продемонструє проблему та правильний варіант коду.
## Реалізація завдання
У наведеному вище коді є помилка використання пам’яті. Ось, що відбувається в циклі:
- `ptr` спочатку дорівнює `NULL`;
- якщо `ptr == NULL`, викликається `malloc(n)`, тобто виділяється пам’ять;
- `ptr` використовується в тілі циклу;
- `ptr` негайно звільняється `(free(ptr))`;
- у наступній ітерації `ptr` вже є використаним вказівником, але він передається у `if (!ptr)`;
- `if (!ptr)` перевіряє значення `ptr`, але воно не обов’язково стане `NULL` після `free()`, тому `malloc(n)` може більше не викликатися, а програма працюватиме з "висячим" вказівником.
Це може спричинити аварійне завершення, програма може друкувати непередбачувані значення,також, якщо `ptr` не обнуляється, перевірка `if (!ptr)` може не спрацювати належним чином.
Я виправила код так, що кожна ітерація виділяє пам’ять перед використанням, `ptr = NULL` після `free(ptr)` захищає від використання "висячого" вказівника. Також додала перевірка `if (!ptr)`, щоб уникнути використання `NULL` у разі невдалого `malloc()`.
## Компіляція та виконання завдання
![image](https://github.com/user-attachments/assets/2d005018-b90a-478b-8ae4-2779be89c5c0)

## Завдання 5
Що станеться, якщо `realloc(3)` не зможе виділити пам’ять? Напишіть тестовий випадок, що демонструє цей сценарій.
## Реалізація завдання
Якщо `realloc(3)` не зможе виділити новий блок пам’яті, то вона поверне `NULL`, а оригінальний вказівник `ptr` залишиться дійсним і незмінним - він не буде звільнений.
## Компіляція та виконання завдання
![image](https://github.com/user-attachments/assets/f4512eae-dd9a-463c-9f3a-c46ab9a29f2f)

Я написала код, де виділяю маленький блок через `malloc(16)`, запитуємо дуже великий розмір у `realloc()` (ця дія має провалитися). Перевіряю, що `realloc()` повернув `NULL`, але старий вказівник `ptr` все ще залишився дійсним. Уникаю втрати пам’яті, звільняючи `ptr`.

## Завдання №6
Якщо `realloc(3)` викликати з `NULL` або розміром 0, що станеться? Напишіть тестовий випадок.
## Реалізація завдання
+ `realloc(NULL, size)` еквівалентне `malloc(size)` - *виділяє новий блок пам’яті*;
+ `realloc(ptr, 0)` може поводитись по-різному:
  - У деяких реалізаціях еквівалентно `free(ptr)`, повертає `NULL`;
  - В інших реалізаціях може повертати ненульовий вказівник, який можна безпечно передати у `free()`.
## Компіляція та виконання завдання
![image](https://github.com/user-attachments/assets/142afecd-1dff-4e33-be2d-7123257b3a89)

## Завдання №7
Перепишіть наступний код, використовуючи `reallocarray(3)`:
```
struct sbar *ptr, *newptr;
ptr = calloc(1000, sizeof(struct sbar));
newptr = realloc(ptr, 500 * sizeof(struct sbar));
```
Порівняйте результати виконання з використанням `ltrace`.
## Реалізація завдання
`realloc(ptr, 500 * sizeof(struct sbar))` може спричинити переповнення при множенні. `reallocarray(500, sizeof(struct sbar))` виконує безпечну перевірку переповнення.
## Компіляція та виконання завдання
![image](https://github.com/user-attachments/assets/69161f33-6027-4f61-ac6d-a078bff8ce2b)

![image](https://github.com/user-attachments/assets/3b6af14d-cef5-43af-881a-330ac4cf334f)

## Завдання №20
Використайте `pthreads` для конкурентного доступу до `heap` та перевірте ефективність.
## Реалізація завдання
* **Короткий опис програми.** Код створює 4 потоки. Кожен потік виконує 100000 операцій виділення (`malloc`), запису та звільнення (`free`) блоків пам'яті розміром 64 байти. Використовується функція `clock_gettime` для вимірювання часу виконання всієї операції.
* **Макроси.** `NUM_THREADS` – кількість потоків; `ALLOCATIONS_PER_THREAD` – кількість викликів `malloc` та `free` у кожному потоці; `BLOCK_SIZE` – розмір кожного виділеного блоку пам’яті.
* Розглянемо **функцію потоку** більш детально. У цій функції оголошується масив покажчиків `ptrs` – він зберігає адреси виділених блоків пам’яті. Кожен потік виділяє `ALLOCATIONS_PER_THREAD` блоків по `BLOCK_SIZE` байтів. Якщо `malloc` повертає `NULL`, виводиться помилка і потік завершується. У перший байт кожного блоку записується 'A'. Це гарантує використання виділеної пам’яті. Всі виділені блоки звільняються, щоб уникнути витоків пам'яті.
* У головній функції програми створюється масив потоків `threads`. Визначаються змінні `start` і `end` для вимірювання часу. `clock_gettime(CLOCK_MONOTONIC, &start)` записує час початку виконання. Створення потоків реалізовано нижче:
```
for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, thread_function, NULL) != 0) {
            perror("pthread_create failed");
            exit(1);
        }
    }
```
* Тут `pthread_create` створює потоки. Якщо створення потоку не вдалося, програма завершується з помилкою.
* `pthread_join` очікує завершення всіх потоків.
* Вимірюється час виконання (`clock_gettime(CLOCK_MONOTONIC, &end)` – записує час завершення; обчислюється час виконання у секундах; виводиться час виконання).
## Компіляція та виконання завдання
Початок виконання прогами з `truss`:

![image](https://github.com/user-attachments/assets/7a310040-267f-4036-8e81-a91d921ee38a)

Кінець виконання програми з `truss`:

![image](https://github.com/user-attachments/assets/835c8a5d-c2d9-426b-9cab-d9ace6e35b40)
