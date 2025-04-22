/*
 * macro.h
 *
 * Macros for working with design patterns
 * Version: 2.1
 */

#ifndef CHAOS_PP_MACRO_H_
#define CHAOS_PP_MACRO_H_

// Helper macro for creating default constructor
#define _DEFAULT_CONSTRUCTOR() = default

// Helper macro for deleting copy/move operations
#define _CHAOS_DELETE_COPY_MOVE(ClassName)         		\
ClassName(const ClassName&) = delete;                   \
    ClassName& operator=(const ClassName&) = delete; 	\
    ClassName(ClassName&&) = delete;               		\
    ClassName& operator=(ClassName&&) = delete

/**
 * @brief Creates a singleton class (Singleton)
 * @param ClassName - Class name
 * @param ... Additional specifiers for the constructor
 *
 * Example of use:
 * class MyClass {
 *     SINGLETON(MyClass, = default);
 * };
 */
#define SINGLETON(ClassName, ...)                  			\
private:                                           			\
    ClassName() __VA_ARGS__;                       			\
    ~ClassName() = default;                        			\
    _CHAOS_DELETE_COPY_MOVE(ClassName);            			\
public:                                            			\
    static ClassName& instance() noexcept {        			\
        static ClassName instance;                 			\
        return instance;                           			\
    }                                              			\
private:													\
    static_assert(true, "Require semicolon after macro")

/**
* @brief Creates a static class without instances
* @param ClassName -  Class name
*
* Usage example:
* STATIC_CLASS(Utility);
*/
#define STATIC_CLASS(ClassName)                    		\
private:												\
    ClassName() = delete;                          		\
    ~ClassName() = delete;                         		\
    _CHAOS_DELETE_COPY_MOVE(ClassName);            		\
    static_assert(true, "Require semicolon after macro")

/**
 * @brief Creates a class with controlled instance creation through a static method.
 * @param ClassName - The name of the class.
 * @param ... - Additional specifiers for the constructor (e.g., = default, = delete).
 *
 * This macro defines a class where instances can only be created via the static `create` method.
 * The constructor is private, and copy/move operations are deleted to enforce controlled creation.
 *
 * Usage example:
 * CONTROLLED_CREATION_CLASS(Utility, = default);
 *
 * This will generate:
 * - A private constructor with the specified behavior (e.g., default or deleted).
 * - A default destructor.
 * - Deleted copy and move operations.
 * - A static `create` method that returns a new instance of the class.
 *
 * Note: Unlike a static class without instances, this macro allows creating instances through the `create` method.
 */
#define CONTROLLED_CREATION_CLASS(ClassName, ...)       \
private:                                                \
    ClassName() __VA_ARGS__;                            \
public:                                                 \
    ~ClassName() = default;                             \
    _CHAOS_DELETE_COPY_MOVE(ClassName);                 \
public:                                                 \
    static ClassName create() noexcept {                \
        return ClassName();                             \
    }                                                   \
private:                                                \
    static_assert(true, "Require semicolon after macro")

#endif /* CHAOS_PP_MACRO_H_ */
