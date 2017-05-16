/*!
    \file      microut.h
    \brief     Unit-testing (micro) tool (for C projects)
    \author    Alexander Kurakin <kuraga333@mail.ru>
    \copyright © Alexander Kurakin, 2016
    \license   MIT
    \see       <http://www.jera.com/techinfo/jtns/jtn002.html>
    \see       <https://git.fairy-project.org/hauspie/minunit>
*/


#ifndef __MICROUT_H
#define __MICROUT_H


#include "config.h"

#include <stddef.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


struct __ut_test_desc;

/*!
    \brief     Тип функции теста
    \protected
*/
typedef void (*__ut_test_func)(struct __ut_test_desc *desc);
/*!
    \brief     Структура теста
    \protected
*/
struct __ut_test_desc
{
    const char * const name;           //!< Указатель на строку, наименование теста
    const char * const description;    //!< Указатель на строку, описание теста
    const char * const file;           //!< Указатель на строку, файл, в котором определен набор тестов
    const unsigned int line;           //!< Строка файла, в котором определен набор тестов
    const __ut_test_func func;         //!< Указатель на функцию набора тестов
    bool started;                      //!< Флаг старта набора тестов
    unsigned int performed_count;      //!< Количество запущенныых проверок
    unsigned int successed_count;      //!< Количество успешных проверок
};

struct __ut_test_suite_desc;
/*!
    \brief     Тип функции набора тестов
    \protected
*/
typedef void (*__ut_test_suite_func)(struct __ut_test_suite_desc *desc);
/*!
    \brief     Структура набора тестов
    \protected
*/
struct __ut_test_suite_desc
{
    const char * const name;                     //!< Указатель на строку, наименование набора тестов
    const char * const description;              //!< Указатель на строку, описание набора тестов
    const __ut_test_suite_func startup;          //!< Указатель на функцию, которая будет выполнена перед запуском набора тестов
    const __ut_test_suite_func teardown;         //!< Указатель на функцию, которая будет выполнена после отработки набора тестов
    const __ut_test_func before_each;            //!< Указатель на функцию, которая будет выполнена перед каждым тестом
    const __ut_test_func after_each;             //!< Указатель на функцию, которая будет выполнена после каждого теста
    struct __ut_test_desc * const test_descs;    //!< Массив структур тестов
    bool started;                                //!< Флаг запуска набора тестов
    unsigned int performed_count;                //!< Количество запущенныых тестов
    unsigned int successed_count;                //!< Количество успешных тестов
};

/*!
    \brief     Совершить проверку
    \details
        - Если значение выражения истинно, то вызывает #UT_ON_SUCCESSFUL_ASSERT
        - Иначе вызывает #UT_ON_FAILED_ASSERT и завершает текущую функцию
        - Сообщает запустившему тесту/набору тестов, о запуске проверки и ее успешности
    \param[in] assertion выражение, значение которого проверяется
    \param[in] message   указатель на строку-сообщение
    \pre       Может быть вызван на любой стадии тестирования
*/
#define UT_ASSERT(assertion, message) do {               \
        /* Помечаем, что проверка запущена            */ \
        desc->performed_count++;                         \
                                                         \
        /* Проверяем значение выражения...            */ \
        if ((assertion)) {                               \
            /* ...если истинно,                       */ \
            /* то помечаем это...                     */ \
            desc->successed_count++;                     \
            /* и вызываем макрос-обработчик успеха    */ \
            UT_ON_SUCCESSFUL_ASSERT(desc, message);      \
        }                                                \
        else                                             \
        {                                                \
            /* ...иначе                               */ \
            /* запускаем макрос-обработчик неудачи... */ \
            UT_ON_FAILED_ASSERT(desc, message);          \
            /* и прерываем выполнение текущей функции */ \
            return;                                      \
        }                                                \
    } while (0)
#define UT_FAIL(message) UT_ASSERT(false, message)

/*!
    \brief     Определить функцию, которая будет вызвана перед запуском каждого
        из тестов (из указанного набора тестов)
    \param[in] test_suite набор тестов
*/
#define UT_BEFORE_EACH(test_suite) void test_suite##_before_each(struct __ut_test_desc *desc)

/*!
    \brief     Определить функцию, которая будет вызвана после запуска каждого
        из тестов (из указанного набора тестов)
    \param[in] test_suite набор тестов
*/
#define UT_AFTER_EACH(test_suite) void test_suite##_after_each(struct __ut_test_desc *desc)

/*!
    \brief     Определить функцию теста
    \param[in] test_suite набор тестов
    \param[in] test       тест
*/
#define UT_TEST(test_suite, test) void test_suite##_##test(struct __ut_test_desc *desc)

/*!
    \brief     Определить функцию, которая будет вызвана перед запуском набора тестов
    \param[in] test_suite набор тестов
*/
#define UT_STARTUP(test_suite) void test_suite##_startup(struct __ut_test_suite_desc *desc)

/*!
    \brief     Определить функцию, которая будет вызвана перед запуском набора тестов
    \param[in] test_suite набор тестов
*/
#define UT_TEARDOWN(test_suite) void test_suite##_teardown(struct __ut_test_suite_desc *desc)

/*!
    \brief     Макрос для добавления теста в список тестов набора тестов
    \param[in] test_suite  набор тестов
    \param[in] test        тест
    \param[in] description указатель на строку-описание теста
*/
#define UT_ADD_TEST(test_suite, test, description) { #test, description, __FILE__, __LINE__, test_suite##_##test, false, 0, 0 }

/*!
    \brief Макрос для завершения списка тестов
*/
#define UT_TEST_SUITE_END { NULL, NULL, __FILE__, __LINE__, NULL, false, 0, 0 }

/*!
    \brief     Макрос для определения набора тестов
    \param[in] test_suite  набор тестов
    \param[in] description указатель на строку-описание набора тестов
    \param[in] ...         список тестов.
        Формулируется путем вызова, через запятую, макросов #UT_ADD_TEST,
        для каждого из тестов, и завершающим макросом #UT_TEST_SUITE_END.
    \warning  Необходимо вызывать макрос без заверщающего разделителя \p ;
*/
#define UT_DECLARE_TEST_SUITE(test_suite, description, ...)       \
    /* Объявляем массив структур тесов                         */ \
    static struct __ut_test_desc test_suite##_test_descs[] = {    \
        __VA_ARGS__                                               \
    };                                                            \
                                                                  \
    /* Объявляем структуру набора тестов                       */ \
    static struct __ut_test_suite_desc test_suite##_desc = {      \
        #test_suite, description,                                 \
        test_suite##_startup, test_suite##_teardown,              \
        test_suite##_before_each, test_suite##_after_each,        \
        test_suite##_test_descs,                                  \
        false, 0, 0                                               \
    };

/*!
    \brief     Получить структуру набора тестов
    \param[in] test_suite  набор тестов
*/
#define UT_TEST_SUITE_DESC(test_suite)    ( test_suite##_desc )

/*!
    \brief     Проверить, был ли тест запущен
    \param[in] test_desc структура теста
    \return    \p true, если тест был запущен; \p false иначе
*/
#define UT_IS_TEST_STARTED(test_desc)   ( (test_desc)->started )

/*!
    \brief     Проверить, был ли тест пропущен
    \param[in] test_desc структура теста
    \return    \p true, если тест не был запущен; \p false иначе
*/
#define UT_IS_TEST_SKIPPED(test_desc)   ( !UT_IS_TEST_STARTED(test_desc) )

/*!
    \brief     Проверить, был ли тест успешен
    \param[in] test_desc структура теста
    \return    \p true, если тест был запущен и успешно завершился; \p false иначе
*/
#define UT_IS_TEST_SUCCESSED(test_desc) ( UT_IS_TEST_STARTED(test_desc) && (test_desc)->performed_count == (test_desc)->successed_count )

/*!
    \brief     Проверить, был ли тест провален
    \param[in] test_desc структура теста
    \return    \p true, если тест был запущен и не завершился успешно; \p false иначе
*/
#define UT_IS_TEST_FAILED(test_desc)    ( !UT_IS_TEST_SUCCESSED(test_desc) )

/*!
    \brief     Проверить, был ли набор тестов запущен
    \param[in] test_suite_desc структура набора тестов
    \return    \p true, если набор тестов был запущен; \p false иначе
*/
#define UT_IS_TEST_SUITE_STARTED(test_suite_desc)   ( (test_suite_desc)->started )

/*!
    \brief     Проверить, был ли набор тестов пропущен
    \param[in] test_suite_desc структура набора тестов
    \return    \p true, если набор тестов не был запущен; \p false иначе
*/
#define UT_IS_TEST_SUITE_SKIPPED(test_suite_desc)   ( !UT_IS_TEST_SUITE_STARTED(test_suite_desc) )

/*!
    \brief     Проверить, был ли набор тестов успешен
    \param[in] test_suite_desc структура набора тестов
    \return    \p true, если набор тестов был запущен и завершился успешно; \p false иначе
*/
#define UT_IS_TEST_SUITE_SUCCESSED(test_suite_desc) ( UT_IS_TEST_SUITE_STARTED(test_suite_desc) && (test_suite_desc)->performed_count == (test_suite_desc)->successed_count )

/*!
    \brief     Проверить, был ли набор тестов провален
    \param[in] test_suite_desc структура набора тестов
    \return    \p true, если набор тестов был запущен и не завершился успешно; \p false иначе
*/
#define UT_IS_TEST_SUITE_FAILED(test_suite_desc)    ( !UT_IS_TEST_SUITE_SUCCESSED(test_suite_desc) )

/*!
    \brief     Запустить набор тестов
    \param[in] test_suite_desc указатель на структуру набора тестов
    \return    Флаг успешного выполнения набора тестов
    \protected
*/
static bool __ut_run_test_suite(struct __ut_test_suite_desc *test_suite_desc)
{
    // \name Объявляем набор тестов запущенным
    // @{
    test_suite_desc->started = true;
    test_suite_desc->successed_count = test_suite_desc->performed_count = 0;
    // @}

    // Вызываем startu-функцию
    test_suite_desc->startup(test_suite_desc);
    // Если в ходе ее выполнения были неуспешные проверки, то прерываем выполнение набора тестов
    if (!UT_IS_TEST_SUITE_SUCCESSED(test_suite_desc))
    {
        return false;
    }

    // Запускаем тесты
    for (unsigned int i = 0; test_suite_desc->test_descs[i].func != NULL; ++i)
    {
        // Получаем указатель на структуру теста
        struct __ut_test_desc *test_desc = &(test_suite_desc->test_descs[i]);
        // Объявляем тест запущенным
        // @{
        test_desc->started = true;
        test_desc->successed_count = test_desc->performed_count = 0;
        // @}

        // Объявляем (в наборе тестов) тест запущенным
        ++test_suite_desc->performed_count;

        // Запускаем before each-функцию
        test_suite_desc->before_each(test_desc);
        // Если в ходе ее выполнения не было неуспешных проверки, то продолжаем выполнение теста
        if (UT_IS_TEST_SUCCESSED(test_desc))
        {
            // Запускаем функцию теста
            test_desc->func(test_desc);
        }

        // Запускаем after each-функцию
        test_suite_desc->after_each(test_desc);

        // Если тест был успешен, то помачаем этот факт в наборе тестов 
        // Выполняем обработчик успехов
        if (UT_IS_TEST_SUCCESSED(test_desc))
        {
            ++test_suite_desc->successed_count;
            UT_ON_SUCCESSFUL_TEST(test_desc);
        }
        // Иначе выполняем обработчик неудач
        else
        {
            UT_ON_FAILED_TEST(test_desc);
        }
    }

    // Запускаем teardown-функцию набора тестов
    test_suite_desc->teardown(test_suite_desc);

    // Возвращаем флаг успешности набора тестов
    return UT_IS_TEST_SUITE_SUCCESSED(test_suite_desc);
}

/*!
    \brief     Запустить набор тестов
    \param[in] test_suite набор тестов
*/
#define UT_RUN_TEST_SUITE(test_suite) __ut_run_test_suite(&test_suite##_desc)


/*!
    \brief     Проверить равенство двух знаковых десятичных чисел
    \details   Проводит проверку, формируя сообщение, специфичное для сравнения
        двух знаковых десятичных чисел
    \param[in] actual   реальное значение
    \param[in] expected ожидаемое значение
    \param[in] message  указатель на строку-сообщение
*/
#define UT_DECIMAL_EQUALS(actual, expected, message) do {                              \
        const __auto_type actual_ = (actual);                                          \
        const __auto_type expected_ = (expected);                                      \
        char __ut_buf[UT_BUFFER_SIZE];                                                 \
                                                                                       \
        sprintf(__ut_buf, "%s (status equality check failed: expected %d, got %d)",    \
            message, expected_, actual_);                                              \
        UT_ASSERT(expected == actual, __ut_buf);                                       \
    } while(0)

/*!
    \brief     Проверить равенство двух беззнаковых десятичных чисел
    \details   Проводит проверку, формируя сообщение, специфичное для сравнения
        двух беззнаковых десятичных чисел
    \param[in] actual   реальное значение
    \param[in] expected ожидаемое значение
    \param[in] message  указатель на строку-сообщение
*/
#define UT_UNSIGNED_DECIMAL_EQUALS(actual, expected, message) do                       \
        const __auto_type actual_ = (actual);                                          \
        const __auto_type expected_ = (expected);                                      \
        char __ut_buf[UT_BUFFER_SIZE];                                                 \
                                                                                       \
        sprintf(__ut_buf, "%s (status equality check failed: expected %u, got %u)",    \
            message, expected_, actual_);                                              \
        UT_ASSERT(expected == actual, __ut_buf);                                       \
    } while(0)

/*!
    \brief     Проверить равенство двух беззнаковых шестнадцатиричных чисел
    \details   Проводит проверку, формируя сообщение, специфичное для сравнения
        двух беззнаковых шестнадцатиричных чисел
    \param[in] actual   реальное значение
    \param[in] expected ожидаемое значение
    \param[in] message  указатель на строку-сообщение
*/
#define UT_UNSIGNED_HEXADECIMAL_EQUALS(actual, expected, message) do {                 \
        const __auto_type actual_ = (actual);                                          \
        const __auto_type expected_ = (expected);                                      \
        char __ut_buf[UT_BUFFER_SIZE];                                                 \
                                                                                       \
        sprintf(__ut_buf, "%s (status equality check failed: expected %X, got %X)",    \
            message, expected_, actual_);                                              \
        UT_ASSERT(expected == actual, __ut_buf);                                       \
    } while(0)

/*!
    \brief     Проверить равенство двух символов
    \details   Проводит проверку, формируя сообщение, специфичное для сравнения
        двух символов
    \param[in] actual   реальное значение
    \param[in] expected ожидаемое значение
    \param[in] message  указатель на строку-сообщение
*/
#define UT_CHAR_EQUALS(actual, expected, message) do {                                 \
        const __auto_type actual_ = (actual);                                          \
        const __auto_type expected_ = (expected);                                      \
        char __ut_buf[UT_BUFFER_SIZE];                                                 \
                                                                                       \
        sprintf(__ut_buf, "%s (status equality check failed: expected %c, got %c)",    \
            message, expected_, actual_);                                              \
        UT_ASSERT(expected == actual, __ut_buf);                                       \
    } while(0)


#ifdef __cplusplus
}
#endif


#endif  // __MICROUT_H
